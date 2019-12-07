#include "stdafx.h"
#include "Group.h"
#include "Actor.h"
#include "Subsystems.h"
#include "Random.h"
#include "Utils.h"

namespace Game {

sa::IdGenerator<uint32_t> Group::groupIds_;

void Group::RegisterLua(kaguya::State& state)
{
    state["Group"].setClass(kaguya::UserdataMetatable<Group>()
        .addFunction("GetId", &Group::GetId)
        .addFunction("GetLeader", &Group::GetLeader)
        .addFunction("Add", &Group::_LuaAdd)
        .addFunction("Remove", &Group::_LuaRemove)
        .addFunction("GetColor", &Group::GetColor)
        .addFunction("SetColor", &Group::SetColor)
        .addFunction("GetName", &Group::GetName)
        .addFunction("SetName", &Group::SetName)
        .addFunction("IsAlly", &Group::IsAlly)
        .addFunction("IsEnemy", &Group::IsEnemy)
        .addFunction("GetMember", &Group::_LuaGetMember)
        .addFunction("GetMemberCount", &Group::_LuaGetMemberCount)
        .addFunction("GetRandomMember", &Group::GetRandomMember)
        .addFunction("GetRandomMemberInRange", &Group::GetRandomMemberInRange)
    );
}

Group::Group(uint32_t id) :
    id_(id)
{ }

Group::Group(uint32_t id, TeamColor color) :
    color_(color),
    id_(id)
{ }

bool Group::_LuaRemove(Actor* actor)
{
    if (!actor)
        return false;
    return Remove(actor->id_);
}

bool Group::_LuaAdd(Actor* actor)
{
    if (!actor)
        return false;
    return Add(actor->GetPtr<Actor>());
}

bool Group::IsEnemy(const Group* other) const
{
    auto* l1 = GetLeader();
    if (!l1)
        return false;
    auto* l2 = other->GetLeader();
    if (!l2)
        return false;
    return l1->IsEnemy(l2);
}

bool Group::IsAlly(const Group* other) const
{
    auto* l1 = GetLeader();
    if (!l1)
        return false;
    auto* l2 = other->GetLeader();
    if (!l2)
        return false;
    return l1->IsAlly(l2);
}

bool Group::Remove(uint32_t id)
{
    auto it = std::find_if(members_.begin(), members_.end(), [&](const std::weak_ptr<Actor>& current)
    {
        if (auto c = current.lock())
            return c->id_ == id;
        return false;
    });
    if (it == members_.end())
        return false;
    members_.erase(it);
    return true;
}

bool Group::Add(std::shared_ptr<Actor> actor)
{
    const auto it = std::find_if(members_.begin(), members_.end(), [&](const std::weak_ptr<Actor>& current)
    {
        if (auto c = current.lock())
            return c->id_ == actor->id_;
        return false;
    });
    if (it != members_.end())
        return false;
    actor->SetGroupId(id_);
    members_.push_back(actor);
    return true;
}

Actor* Group::GetLeader() const
{
    if (members_.size() == 0)
        return nullptr;
    if (auto l = members_.front().lock())
        return l.get();
    return nullptr;
}

Actor* Group::GetRandomMember() const
{
    if (members_.size() == 0)
        return nullptr;

    auto* rng = GetSubsystem<Crypto::Random>();
    const float rnd = rng->GetFloat();
    using iterator = std::vector<std::weak_ptr<Actor>>::const_iterator;
    auto it = Utils::SelectRandomly<iterator>(members_.begin(), members_.end(), rnd);
    if (it != members_.end())
    {
        if (auto p = (*it).lock())
            return p.get();
    }
    return nullptr;
}

Actor* Group::GetRandomMemberInRange(const Actor* actor, Ranges range) const
{
    if (members_.size() == 0 || actor == nullptr)
        return nullptr;
    std::vector<Actor*> actors;
    VisitMembers([&](Actor& current) {
        if (actor->IsInRange(range, &current))
            actors.push_back(&current);
        return Iteration::Continue;
    });
    if (actors.size() == 0)
        return nullptr;

    auto* rng = GetSubsystem<Crypto::Random>();
    const float rnd = rng->GetFloat();
    using iterator = std::vector<Actor*>::const_iterator;
    auto it = Utils::SelectRandomly<iterator>(actors.begin(), actors.end(), rnd);
    if (it != actors.end())
        return (*it);

    return nullptr;
}

Actor* Group::_LuaGetMember(int index)
{
    if (index >= static_cast<int>(members_.size()))
        return nullptr;
    if (auto m = members_.at(static_cast<size_t>(index)).lock())
        return m.get();
    return nullptr;
}

int Group::_LuaGetMemberCount()
{
    return static_cast<int>(members_.size());
}

}
