#include "stdafx.h"
#include "SkillManager.h"
#include "DataProvider.h"
#include <AB/Entities/Skill.h>

namespace Game {

SkillManager SkillManager::Instance;

SkillManager::SkillManager()
{
}

std::shared_ptr<Skill> SkillManager::Get(uint32_t index)
{
    std::shared_ptr<Skill> result;
    auto it = skillCache_.find(index);
    if (it != skillCache_.end())
    {
        result = std::make_shared<Skill>((*it).second);
    }
    else
    {
        IO::DataClient* client = Application::Instance->GetDataClient();
        AB::Entities::Skill skill;
        skill.index = index;
        if (!client->Read(skill))
        {
            LOG_ERROR << "Error reading skill with index " << index << std::endl;
            return std::shared_ptr<Skill>();
        }
        result = std::make_shared<Skill>(skill);
        // Move to cache
        skillCache_.emplace(index, skill);
    }

    if (result)
    {
        if (result->LoadScript(IO::DataProvider::Instance.GetDataFile(result->data_.script)))
            return result;
    }

    return std::shared_ptr<Skill>();
}

}
