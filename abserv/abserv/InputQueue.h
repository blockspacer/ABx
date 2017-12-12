#pragma once

#include "Variant.h"
#include <queue>
#include <mutex>

namespace Game {

enum InputType
{
    InputTypeNone = 0,
    InputTypeMove = 1,
    InputTypeUseSkill = 2,
    InputTypeCancelSkill = 3,
    InputTypeAttack = 4,
    InputTypeCancelAttack = 5,
    InputTypeSelect = 6,
};

enum InputDataType
{
    InputDataDirection = 1,
    InputDataSkillId = 2,
    InputDataObjectId = 3,
};

struct InputItem
{
    InputType type;
    Utils::VariantMap data;
};

class InputQueue
{
private:
    std::queue<InputItem> queue_;
    std::mutex lock_;
public:
    InputQueue() = default;
    ~InputQueue() = default;

    void Add(InputType type, const Utils::VariantMap& data)
    {
        std::lock_guard<std::mutex> lockClass(lock_);
        queue_.push({ type, data });
    }
    bool Get(InputItem& item)
    {
        if (queue_.empty())
            return false;
        item = queue_.front();
        queue_.pop();
        return true;
    }
};

}

