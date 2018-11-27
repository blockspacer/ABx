#include "stdafx.h"
#include "SkillsComp.h"
#include "Actor.h"
#include "Skill.h"

namespace Game {
namespace Components {

void SkillsComp::Update(uint32_t timeElapsed)
{
    SkillBar* sb = owner_.GetSkillBar();
    sb->Update(timeElapsed);
    if (usingSkill_)
    {
        if (auto ls = lastSkill_.lock())
        {
            if (!ls->IsUsing())
            {
                usingSkill_ = false;
                endDirty_ = true;
                newRecharge_ = ls->recharge_;
                lastSkill_.reset();
            }
        }
    }
}

AB::GameProtocol::SkillError SkillsComp::UseSkill(int index)
{
    std::shared_ptr<Actor> target;
    if (auto selObj = owner_.selectedObject_.lock())
        target = selObj->GetThisDynamic<Actor>();
    // Can use skills only on Creatures not all GameObjects.
    // But a target is not mandatory, the Skill script will decide
    // if it needs a target, and may fail.
    SkillBar* sb = owner_.GetSkillBar();
    lastSkill_ = sb->GetSkill(index);
    lastError_ = sb->UseSkill(index, target);
    usingSkill_ = lastError_ == AB::GameProtocol::SkillErrorNone;
    lastSkillIndex_ = index;
    startDirty_ = true;
    endDirty_ = false;
    return lastError_;
}

void SkillsComp::Write(Net::NetworkMessage& message)
{
    // The server has 0 based skill indices but for the client these are 1 based
    if (startDirty_)
    {
        if (lastError_ == AB::GameProtocol::SkillErrorNone)
        {
            message.AddByte(AB::GameProtocol::GameObjectUseSkill);
            message.Add<uint32_t>(owner_.id_);
            message.Add<int>(lastSkillIndex_ + 1);
            if (auto ls = lastSkill_.lock())
            {
                message.Add<uint16_t>(ls->GetRealEnergy());
                message.Add<uint16_t>(ls->GetRealAdrenaline());
                message.Add<uint16_t>(ls->GetRealActivation());
                message.Add<uint16_t>(ls->GetRealOvercast());
            }
            else
            {
                message.Add<uint16_t>(0);
                message.Add<uint16_t>(0);
                message.Add<uint16_t>(0);
                message.Add<uint16_t>(0);
            }
        }
        else
        {
            message.AddByte(AB::GameProtocol::GameObjectSkillFailure);
            message.Add<uint32_t>(owner_.id_);
            message.Add<int>(lastSkillIndex_ + 1);
            message.AddByte(lastError_);
        }
        startDirty_ = false;
    }

    if (endDirty_)
    {
        message.AddByte(AB::GameProtocol::GameObjectEndUseSkill);
        message.Add<uint32_t>(owner_.id_);
        message.Add<int>(lastSkillIndex_ + 1);
        message.Add<uint16_t>(newRecharge_);
        endDirty_ = false;
    }
}

}
}
