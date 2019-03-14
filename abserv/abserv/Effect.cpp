#include "stdafx.h"
#include "Effect.h"
#include "Actor.h"
#include "ScriptManager.h"
#include "Utils.h"
#include "DataProvider.h"
#include "Subsystems.h"
#include "Skill.h"
#include "Item.h"

namespace Game {

void Effect::RegisterLua(kaguya::State& state)
{
    state["Effect"].setClass(kaguya::UserdataMetatable<Effect>()
        .addFunction("GetStartTime", &Effect::GetStartTime)
        .addFunction("GetEndTime", &Effect::GetEndTime)
        .addFunction("GetTicks", &Effect::GetTicks)
    );
}

void Effect::InitializeLua()
{
    ScriptManager::RegisterLuaAll(luaState_);
    luaState_["self"] = this;
}

bool Effect::LoadScript(const std::string& fileName)
{
    script_ = GetSubsystem<IO::DataProvider>()->GetAsset<Script>(fileName);
    if (!script_)
        return false;
    if (!script_->Execute(luaState_))
        return false;

    persistent_ = luaState_["isPersistent"];
    if (ScriptManager::IsBool(luaState_, "internal"))
        internal_ = luaState_["internal"];

    if (ScriptManager::IsFunction(luaState_, "onUpdate"))
        functions_ |= FunctionUpdate;
    if (ScriptManager::IsFunction(luaState_, "getSkillCost"))
        functions_ |= FunctionGetSkillCost;
    if (ScriptManager::IsFunction(luaState_, "getDamage"))
        functions_ |= FunctionGetDamage;
    if (ScriptManager::IsFunction(luaState_, "getAttackSpeed"))
        functions_ |= FunctionGetAttackSpeed;
    if (ScriptManager::IsFunction(luaState_, "getAttackDamageType"))
        functions_ |= FunctionGetAttackDamageType;
    if (ScriptManager::IsFunction(luaState_, "getAttackDamage"))
        functions_ |= FunctionGetAttackDamage;
    if (ScriptManager::IsFunction(luaState_, "onAttack"))
        functions_ |= FunctionOnAttack;
    if (ScriptManager::IsFunction(luaState_, "onGettingAttacked"))
        functions_ |= FunctionOnGettingAttacked;
    if (ScriptManager::IsFunction(luaState_, "onUseSkill"))
        functions_ |= FunctionOnUseSkill;
    if (ScriptManager::IsFunction(luaState_, "onSkillTargeted"))
        functions_ |= FunctionOnSkillTargeted;
    if (ScriptManager::IsFunction(luaState_, "onAttacked"))
        functions_ |= FunctionOnAttacked;
    return true;
}

void Effect::Update(uint32_t timeElapsed)
{
    if (endTime_ == 0)
        return;

    auto source = source_.lock();
    auto target = target_.lock();
    if (HaveFunction(FunctionUpdate))
        luaState_["onUpdate"](source.get(), target.get(), timeElapsed);
    if (endTime_ <= Utils::Tick())
    {
        luaState_["onEnd"](source.get(), target.get());
        ended_ = true;
    }
}

bool Effect::Start(std::shared_ptr<Actor> source, std::shared_ptr<Actor> target)
{
    target_ = target;
    source_ = source;
    startTime_ = Utils::Tick();
    ticks_ = luaState_["getDuration"](source.get(), target.get());
    endTime_ = startTime_ + ticks_;
    bool succ = luaState_["onStart"](source.get(), target.get());
    if (!succ)
    {
        endTime_ = 0;
    }
    return succ;
}

void Effect::Remove()
{
    // The Effect was removed before it ended
    auto source = source_.lock();
    auto target = target_.lock();
    ScriptManager::CallFunction(luaState_, "onRemove",
        source.get(), target.get());
    cancelled_ = true;
}

void Effect::GetSkillCost(Skill* skill,
    int32_t& activation, int32_t& energy, int32_t& adrenaline, int32_t& overcast, int32_t& hp)
{
    if (!HaveFunction(FunctionGetSkillCost))
        return;

    kaguya::tie(activation, energy, adrenaline, overcast, hp) =
        luaState_["getSkillCost"](skill, activation, energy, adrenaline, overcast, hp);
}

void Effect::GetDamage(DamageType type, int32_t& value)
{
    if (!HaveFunction(FunctionGetDamage))
        return;

    value = luaState_["getDamage"](static_cast<int>(type), value);
}

void Effect::GetAttackSpeed(Item* weapon, uint32_t& value)
{
    if (!HaveFunction(FunctionGetAttackSpeed))
        return;
    value = luaState_["getAttackSpeed"](weapon, value);
}

void Effect::GetAttackDamageType(DamageType& type)
{
    if (!HaveFunction(FunctionGetAttackDamageType))
        return;
    type = luaState_["getAttackDamageType"](type);
}

void Effect::GetAttackDamage(int32_t& value)
{
    if (!HaveFunction(FunctionGetAttackDamage))
        return;
    value = luaState_["getAttackDamage"](value);
}

void Effect::OnAttack(Actor* source, Actor* target, bool& value)
{
    if (HaveFunction(FunctionOnAttack))
        value = luaState_["onAttack"](source, target);
}

void Effect::OnAttacked(Actor* source, Actor* target, DamageType type, int32_t damage, bool& success)
{
    if (HaveFunction(FunctionOnAttacked))
        success = luaState_["onAttacked"](source, target, type, damage);
}

void Effect::OnGettingAttacked(Actor* source, Actor* target, bool& value)
{
    if (HaveFunction(FunctionOnGettingAttacked))
        value = luaState_["onGettingAttacked"](source, target);
}

void Effect::OnUseSkill(Actor* source, Actor* target, Skill* skill, bool& value)
{
    if (HaveFunction(FunctionOnUseSkill))
        value = luaState_["canUseSkill"](source, target, skill);
}

void Effect::OnSkillTargeted(Actor* source, Actor* target, Skill* skill, bool& value)
{
    if (HaveFunction(FunctionOnSkillTargeted))
        value = luaState_["onSkillTargeted"](source, target, skill);
}

bool Effect::Serialize(IO::PropWriteStream& stream)
{
    stream.Write<uint8_t>(EffectAttrId);
    stream.Write<uint32_t>(data_.index);

    stream.Write<uint8_t>(EffectAttrTicks);
    stream.Write<uint32_t>(ticks_);

    return true;
}

bool Effect::Unserialize(IO::PropReadStream& stream)
{
    uint8_t attr;
    while (stream.Read<uint8_t>(attr) && attr != EffectAttrEnd)
    {
        if (!UnserializeProp(static_cast<EffectAttr>(attr), stream))
            return false;
    }
    return true;
}

bool Effect::UnserializeProp(EffectAttr attr, IO::PropReadStream& stream)
{
    switch (attr)
    {
    case EffectAttrId:
        break;
    case EffectAttrTicks:
        return stream.Read<uint32_t>(ticks_);
    }
    return false;
}

}
