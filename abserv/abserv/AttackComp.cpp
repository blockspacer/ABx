#include "stdafx.h"
#include "AttackComp.h"
#include "Actor.h"
#include "Subsystems.h"
#include "Random.h"
#include "Game.h"

namespace Game {
namespace Components {

bool AttackComp::CheckRange()
{
    auto target = target_.lock();
    if (!target)
        return false;

    Item* item = owner_.GetWeapon();
    if (item)
    {
        float dist = item->GetWeaponRange();
        if (owner_.GetDistance(target.get()) <= dist)
            return true;
    }
    return owner_.IsInRange(Ranges::Touch, target.get());
}

void AttackComp::StartHit(Actor& target)
{
    // New attack
    attackSpeed_ = owner_.GetAttackSpeed();
    interrupted_ = false;
    owner_.FaceObject(&target);
    if (Utils::TimeElapsed(lastAttackTime_) >= attackSpeed_ / 2)
    {
        lastAttackTime_ = Utils::Tick();
        hitting_ = true;
        FireWeapon(target);
        damageType_ = owner_.GetAttackDamageType();
    }
}

void AttackComp::Hit(Actor& target)
{
    // Done attack -> apply damage
    hitting_ = false;
    if (interrupted_)
    {
        lastError_ = AB::GameProtocol::AttackErrorInterrupted;
        owner_.CallEvent<void(void)>(EVENT_ON_INTERRUPTEDATTACK);
    }
    else
    {
        const float criticalChance = owner_.GetCriticalChance(&target);
        auto rnd = GetSubsystem<Crypto::Random>();
        bool critical = criticalChance >= rnd->GetFloat();
        // Critical hit -> always weapons max damage
        int32_t damage = owner_.GetAttackDamage(critical);
        // Source effects may modify the damage
        owner_.effectsComp_->GetDamage(damageType_, damage, critical);
        bool canGettingAttacked = true;
        target.CallEvent<void(Actor*, DamageType, int32_t, bool&)>(EVENT_ON_ATTACKED,
            &owner_, damageType_, damage, canGettingAttacked);
        if (canGettingAttacked)
        {
            // Some effects may prevent attacks, e.g. blocking
            if (critical)
                // Some effect may prevent critical hits
                target.CallEvent<void(Actor*,bool&)>(EVENT_ON_GET_CRITICAL_HIT, &owner_, critical);
            if (critical)
                damage = static_cast<int>(static_cast<float>(damage) * std::sqrt(2.0f));
            target.damageComp_->ApplyDamage(&owner_, 0, damageType_, damage, owner_.GetArmorPenetration(), true);
        }
        else
        {
            lastError_ = AB::GameProtocol::AttackErrorInterrupted;
            owner_.CallEvent<void(void)>(EVENT_ON_INTERRUPTEDATTACK);
        }
    }
}

void AttackComp::FireWeapon(Actor& target)
{
    auto* weapon = owner_.GetWeapon();
    if (!weapon || !weapon->IsWeaponProjectile())
        return;
    owner_.GetGame()->AddProjectile(weapon->data_.spawnItemUuid,
        owner_.GetThis<Actor>(), target.GetThis<Actor>());
    if (!owner_.IsObjectInSight(target))
    {
        lastError_ = AB::GameProtocol::AttackErrorTargetObstructed;
    }
}

void AttackComp::MoveToTarget(std::shared_ptr<Actor> target)
{
    if (!owner_.autorunComp_->IsAutoRun())
    {
        Item* item = owner_.GetWeapon();
        float dist = item ? item->GetWeaponRange() : RANGE_TOUCH;
        if (owner_.autorunComp_->Follow(target, false, dist))
        {
            owner_.followedObject_ = target;
            owner_.stateComp_.SetState(AB::GameProtocol::CreatureStateMoving);
            owner_.autorunComp_->SetAutoRun(true);
        }
        else
        {
            // No way to get to the target
            attacking_ = false;
            SetAttackState(false);
        }
    }
}

void AttackComp::Update(uint32_t /* timeElapsed */)
{
    if (!attacking_ || pause_)
        return;

    auto target = target_.lock();
    if (target)
    {
        if (target->IsDead())
        {
            // We can stop hitting to this target now :(
            // Poor target!
            attacking_ = false;
            SetAttackState(false);
            return;
        }
    }
    else
    {
        // Gone
        attacking_ = false;
        SetAttackState(false);
        return;
    }
    // We need to move to the target
    if (!CheckRange())
    {
        MoveToTarget(target);
        return;
    }
    else
    {
        owner_.autorunComp_->Reset();
        SetAttackState(true);
    }

    // We are in range of the target -> can start attacking it
    if (IsAttackState())
    {
        if (!hitting_)
        {
            StartHit(*target);
        }
        else
        {
            // Now we are really attacking. This can be interrupted.
            if (Utils::TimeElapsed(lastAttackTime_) >= attackSpeed_)
                Hit(*target);
        }
    }
}

void AttackComp::Write(Net::NetworkMessage& message)
{
    if (lastError_ != AB::GameProtocol::AttackErrorNone)
    {
        message.AddByte(AB::GameProtocol::GameObjectAttackFailure);
        message.Add<uint32_t>(owner_.id_);
        message.AddByte(lastError_);
        lastError_ = AB::GameProtocol::AttackErrorNone;
    }
}

void AttackComp::Cancel()
{
    attacking_ = false;
    SetAttackState(false);
}

void AttackComp::Attack(std::shared_ptr<Actor> target, bool ping)
{
    bool canAttack = true;
    owner_.CallEvent<void(Actor*,bool&)>(EVENT_ON_ATTACK, target.get(), canAttack);
    if (!canAttack)
    {
        lastError_ = AB::GameProtocol::AttackErrorInvalidTarget;
        return;
    }
    if (!target)
    {
        // Attack needs a target
        lastError_ = AB::GameProtocol::AttackErrorInvalidTarget;
        return;
    }
    bool canGettingAttacked = true;
    target->CallEvent<void(Actor*, bool&)>(EVENT_ON_GETTING_ATTACKED, &owner_, canGettingAttacked);
    if (target->IsUndestroyable() && canGettingAttacked)
    {
        // Can not attack an destroyable target
        lastError_ = AB::GameProtocol::AttackErrorTargetUndestroyable;
        return;
    }

    target_ = target;
    if (ping)
        owner_.CallEvent<void(uint32_t, AB::GameProtocol::ObjectCallType, int)>(EVENT_ON_PINGOBJECT,
            target ? target->id_ : 0, AB::GameProtocol::ObjectCallTypeAttack, 0);
    attacking_ = true;
    lastAttackTime_ = 0;
}

bool AttackComp::IsAttackingTarget(Actor* target) const
{
    if (!IsAttackState())
        return false;
    if (!target)
        return false;
    if (auto t = target_.lock())
        return t->id_ == target->id_;
    return false;
}

bool AttackComp::IsAttackState() const
{
    return owner_.stateComp_.GetState() == AB::GameProtocol::CreatureStateAttacking;
}

void AttackComp::SetAttackState(bool value)
{
    if (IsAttackState() != value)
    {
        if (value)
            owner_.stateComp_.SetState(AB::GameProtocol::CreatureStateAttacking);
        else
            owner_.stateComp_.Reset();
    }
}

bool AttackComp::Interrupt()
{
    if (hitting_)
    {
        interrupted_ = true;
        return true;
    }
    return false;
}

void AttackComp::Pause(bool value)
{
    pause_ = value;
}

bool AttackComp::IsTarget(const Actor* target) const
{
    if (!target)
        return false;
    if (const auto t = target_.lock())
        return t->id_ == target->id_;
    return false;
}

}
}
