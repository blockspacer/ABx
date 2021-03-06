/**
 * Copyright 2017-2020 Stefan Ascher
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <AB/Entities/Guild.h>
#include <AB/Entities/GuildMembers.h>
#include <sa/Iteration.h>
#include <sa/Noncopyable.h>
#include <eastl.hpp>

namespace Game {

class Guild
{
    NON_COPYABLE(Guild)
    NON_MOVEABLE(Guild)
public:
    explicit Guild(AB::Entities::Guild&& data);
    ~Guild() = default;

    size_t GetAccounts(ea::vector<std::string>& uuids) const;
    bool GetMembers(AB::Entities::GuildMembers& members) const;
    bool IsMember(const std::string& accountUuid) const;
    bool GetMember(const std::string& accountUuid, AB::Entities::GuildMember& member) const;

    template <typename Callback>
    void VisitAll(Callback&& callback) const
    {
        AB::Entities::GuildMembers members;
        if (!GetMembers(members))
            return;

        for (const auto& member : members.members)
            if (callback(member) != Iteration::Continue)
                break;
    }

    AB::Entities::Guild data_;
};

}
