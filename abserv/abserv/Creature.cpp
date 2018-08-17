#include "stdafx.h"
#include "Creature.h"
#include "EffectManager.h"
#include "Game.h"
#include <AB/ProtocolCodes.h>
#include "Logger.h"
#include "OctreeQuery.h"
#include "GameManager.h"
#include "MathUtils.h"

#include "DebugNew.h"

namespace Game {

void Creature::InitializeLua()
{
    GameManager::RegisterLuaAll(luaState_);
    luaState_["self"] = this;
    luaInitialized_ = true;
}

void Creature::RegisterLua(kaguya::State& state)
{
    state["Creature"].setClass(kaguya::UserdataMetatable<Creature, GameObject>()
        .addFunction("GetLevel", &Creature::GetLevel)
        .addFunction("GetSkillBar", &Creature::GetSkillBar)
        .addFunction("GetSelectedObject", &Creature::GetSelectedObject)
        .addFunction("SetSelectedObject", &Creature::SetSelectedObject)

        .addFunction("GetSpeed", &Creature::GetSpeed)
        .addFunction("SetSpeed", &Creature::SetSpeed)
        .addFunction("GetEnergy", &Creature::GetEnergy)
        .addFunction("SetEnergy", &Creature::SetEnergy)
        .addFunction("AddEffect", &Creature::AddEffect)
        .addFunction("RemoveEffect", &Creature::RemoveEffect)
    );
}

Creature::Creature() :
    GameObject(),
    moveComp_(*this),
    autorunComp_(*this),
    collisionComp_(*this),
    skills_(this),
    luaInitialized_(false)
{
    // Creature always collides
    static const Math::Vector3 CREATURTE_BB_MIN(-0.2f, 0.0f, -0.2f);
    static const Math::Vector3 CREATURTE_BB_MAX(0.2f, 1.7f, 0.2f);
    SetCollisionShape(
        std::make_unique<Math::CollisionShapeImpl<Math::BoundingBox>>(Math::ShapeTypeBoundingBox,
            CREATURTE_BB_MIN, CREATURTE_BB_MAX)
    );
    occluder_ = true;
}

void Creature::SetSelectedObject(std::shared_ptr<GameObject> object)
{
    Utils::VariantMap data;
    data[InputDataObjectId] = GetId();    // Source
    if (object)
        data[InputDataObjectId2] = object->GetId();   // Target
    else
        data[InputDataObjectId2] = 0;   // Target
    inputs_.Add(InputType::Select, data);
}

bool Creature::Serialize(IO::PropWriteStream& stream)
{
    if (!GameObject::Serialize(stream))
        return false;
    stream.Write<uint32_t>(GetLevel());
    stream.Write<uint8_t>(GetSex());
    stream.Write<uint32_t>(GetProfIndex());
    stream.Write<uint32_t>(GetProf2Index());
    stream.Write<uint32_t>(GetModelIndex());
    return true;
}

void Creature::OnSelected(std::shared_ptr<Creature> selector)
{
    GameObject::OnSelected(selector);
    if (luaInitialized_)
        luaState_["onSelected"](selector);
}

void Creature::OnCollide(std::shared_ptr<Creature> other)
{
    GameObject::OnCollide(other);
    if (luaInitialized_)
        luaState_["onCollide"](other);
}

void Creature::AddEffect(std::shared_ptr<Creature> source, uint32_t index, uint32_t baseDuration)
{
    RemoveEffect(index);

    auto effect = EffectManager::Instance.Get(index);
    if (effect)
    {
        effects_.push_back(effect);
        effect->Start(source, GetThis<Creature>(), baseDuration);
    }
}

void Creature::DeleteEffect(uint32_t index)
{
    auto it = std::find_if(effects_.begin(), effects_.end(), [&](std::shared_ptr<Effect> const& current)
    {
        return current->data_.index == index;
    });
    if (it != effects_.end())
    {
        effects_.erase(it);
    }
}

void Creature::RemoveEffect(uint32_t index)
{
    auto it = std::find_if(effects_.begin(), effects_.end(), [&](std::shared_ptr<Effect> const& current)
    {
        return current->data_.index == index;
    });
    if (it != effects_.end())
    {
        (*it)->Remove();
        DeleteEffect((*it)->data_.index);
    }
}

void Creature::Update(uint32_t timeElapsed, Net::NetworkMessage& message)
{
    GameObject::Update(timeElapsed, message);

    Skill* skill = nullptr;
    int skillIndex = -1;
    moveComp_.turned_ = false;
    moveComp_.directionSet_ = false;
    bool newDirection = false;
    float worldAngle = 0.0f;

    InputItem input;
//    AB::GameProtocol::CreatureState newState = creatureState_;

    stateComp_.Update(timeElapsed);

    // Multiple inputs of the same type overwrite previous
    while (inputs_.Get(input))
    {
        switch (input.type)
        {
        case InputType::Move:
        {
            moveComp_.moveDir_ = static_cast<AB::GameProtocol::MoveDirection>(input.data[InputDataDirection].GetInt());
            if (moveComp_.moveDir_ > AB::GameProtocol::MoveDirectionNone)
            {
                stateComp_.SetState(AB::GameProtocol::CreatureStateMoving);
            }
            else
            {
                // Reset to Idle when neither moving nor turning
                if (stateComp_.GetState() == AB::GameProtocol::CreatureStateMoving &&
                    moveComp_.turnDir_ == AB::GameProtocol::TurnDirectionNone)
                    stateComp_.SetState(AB::GameProtocol::CreatureStateIdle);
            }
            autorunComp_.Reset();
            break;
        }
        case InputType::Turn:
        {
            moveComp_.turnDir_ = static_cast<AB::GameProtocol::TurnDirection>(input.data[InputDataDirection].GetInt());
            if (moveComp_.turnDir_ > AB::GameProtocol::TurnDirectionNone)
            {
                stateComp_.SetState(AB::GameProtocol::CreatureStateMoving);
            }
            else
            {
                if (stateComp_.GetState() == AB::GameProtocol::CreatureStateMoving &&
                    moveComp_.moveDir_ == AB::GameProtocol::MoveDirectionNone)
                    stateComp_.SetState(AB::GameProtocol::CreatureStateIdle);
            }
            autorunComp_.Reset();
            break;
        }
        case InputType::Direction:
        {
            worldAngle = input.data[InputDataDirection].GetFloat();
            newDirection = true;
            break;
        }
        case InputType::Goto:
        {
            // TODO: Just adjust move direction to next points
            const Math::Vector3 dest = {
                input.data[InputDataVertexX].GetFloat(),
                input.data[InputDataVertexY].GetFloat(),
                input.data[InputDataVertexZ].GetFloat()
            };
            bool succ = autorunComp_.FindPath(dest);
            if (succ)
            {
                stateComp_.SetState(AB::GameProtocol::CreatureStateMoving);
                autorunComp_.autoRun_ = true;
            }

            break;
        }
        case InputType::Follow:
        {
            uint32_t targetId = static_cast<uint32_t>(input.data[InputDataObjectId].GetInt());
            followedObject_ = GetGame()->GetObjectById(targetId);
            break;
        }
        case InputType::Attack:
            stateComp_.SetState(AB::GameProtocol::CreatureStateAttacking);
            break;
        case InputType::UseSkill:
        {
            skillIndex = static_cast<uint32_t>(input.data[InputDataSkillIndex].GetInt());
            skill = GetSkill(skillIndex);
            if (skill)
            {
                std::shared_ptr<Creature> target = std::dynamic_pointer_cast<Creature>(selectedObject_.lock());
                if (target)
                {
                    // Can use skills only on Creatures not all GameObjects
                    skills_.UseSkill(skillIndex, target);
                    // These do not change the state
                    if (skill->IsChangingState())
                        stateComp_.SetState(AB::GameProtocol::CreatureStateUsingSkill);
                }
            }
            break;
        }
        case InputType::Select:
        {
            // If a player could control a NPC (e.g. Hero), the player can select
            // targets for this NPC, so we also need the source ID.
            uint32_t sourceId = static_cast<uint32_t>(input.data[InputDataObjectId].GetInt());
            uint32_t targetId = static_cast<uint32_t>(input.data[InputDataObjectId2].GetInt());

            Creature* source = nullptr;
            if (sourceId == id_)
                source = this;
            else
                source = dynamic_cast<Creature*>(GetGame()->GetObjectById(sourceId).get());

            if (source)
            {
                if (targetId != source->GetSelectedObjectId())
                {
                    source->selectedObject_ = GetGame()->GetObjectById(targetId);
                    message.AddByte(AB::GameProtocol::GameObjectSelectTarget);
                    message.Add<uint32_t>(source->id_);
                    if (auto sel = source->selectedObject_.lock())
                    {
                        sel->OnSelected(source->GetThis<Creature>());
                        message.Add<uint32_t>(sel->id_);
                    }
                    else
                        // Clear Target
                        message.Add<uint32_t>(0);
                }
            }
            break;
        }
        case InputType::CancelSkill:
            if (stateComp_.GetState() == AB::GameProtocol::CreatureStateUsingSkill)
                stateComp_.SetState(AB::GameProtocol::CreatureStateIdle);
            break;
        case InputType::CancelAttack:
            if (stateComp_.GetState() == AB::GameProtocol::CreatureStateAttacking)
                stateComp_.SetState(AB::GameProtocol::CreatureStateIdle);
            break;
        case InputType::Command:
        {
            AB::GameProtocol::CommandTypes type = static_cast<AB::GameProtocol::CommandTypes>(input.data[InputDataCommandType].GetInt());
            const std::string& cmd = input.data[InputDataCommandData].GetString();
            HandleCommand(type, cmd, message);
            break;
        }
        }
    }
    if (autorunComp_.autoRun_ && !autorunComp_.HasWaypoints())
    {
        stateComp_.SetState(AB::GameProtocol::CreatureStateIdle);
        autorunComp_.Reset();
    }

    if (stateComp_.IsStateChanged())
    {
#ifdef DEBUG_GAME
        LOG_DEBUG << "New state " << (int)newState << std::endl;
#endif
        stateComp_.Apply();
        message.AddByte(AB::GameProtocol::GameObjectStateChange);
        message.Add<uint32_t>(id_);
        message.AddByte(stateComp_.GetState());
    }

    switch (stateComp_.GetState())
    {
    case AB::GameProtocol::CreatureStateIdle:
        break;
    case AB::GameProtocol::CreatureStateMoving:
    {
        moveComp_.moved_ = false;
        moveComp_.MoveTo(timeElapsed);

        if (autorunComp_.autoRun_ && autorunComp_.HasWaypoints())
        {
            autorunComp_.MoveToNext(timeElapsed);
        }
        if (moveComp_.moved_)
        {
            message.AddByte(AB::GameProtocol::GameObjectPositionChange);
            message.Add<uint32_t>(id_);
            message.Add<float>(transformation_.position_.x_);
            message.Add<float>(transformation_.position_.y_);
            message.Add<float>(transformation_.position_.z_);
        }

        if (!newDirection)
        {
            moveComp_.TurnTo(timeElapsed);
        }
        else
        {
            if (!autorunComp_.autoRun_ && transformation_.rotation_ != worldAngle)
            {
                SetDirection(worldAngle);
            }
        }
        break;
    }
    case AB::GameProtocol::CreatureStateUsingSkill:
        break;
    case AB::GameProtocol::CreatureStateAttacking:
        break;
    case AB::GameProtocol::CreatureStateEmote:
        break;
    }

    // The rotation may change in 2 ways: Turn and SetWorldDirection
    if (moveComp_.turned_ || moveComp_.directionSet_)
    {
        message.AddByte(AB::GameProtocol::GameObjectRotationChange);
        message.Add<uint32_t>(id_);
        message.Add<float>(transformation_.rotation_);
        message.Add<uint8_t>(moveComp_.directionSet_ ? 1 : 0);
    }

    skills_.Update(timeElapsed);
    for (const auto& effect : effects_)
    {
        if (effect->cancelled_ || effect->ended_)
        {
            DeleteEffect(effect->data_.index);
            continue;
        }
        effect->Update(timeElapsed);
    }

}

bool Creature::Move(float speed, const Math::Vector3& amount)
{
    return moveComp_.Move(speed, amount);
}

void Creature::Turn(float angle)
{
    moveComp_.Turn(angle);
}

void Creature::SetDirection(float worldAngle)
{
    moveComp_.SetDirection(worldAngle);
}

}
