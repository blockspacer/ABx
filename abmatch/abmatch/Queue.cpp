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

#include "Queue.h"
#include <AB/Entities/Account.h>
#include <AB/Entities/Character.h>
#include <AB/Entities/Game.h>
#include <abscommon/DataClient.h>
#include <abscommon/MessageClient.h>
#include <abscommon/Subsystems.h>
#include <abscommon/UuidUtils.h>
#include <algorithm>
#include <sa/Transaction.h>

AB::Entities::ProfessionPosition Queue::GetPlayerPosition(const std::string& uuid)
{
    auto* dataclient = GetSubsystem<IO::DataClient>();
    AB::Entities::Character c;
    c.uuid = uuid;
    if (!dataclient->Read(c))
    {
        LOG_ERROR << "Error reading character " << uuid << std::endl;
        return AB::Entities::ProfessionPosition::None;
    }
    AB::Entities::Profession prof;
    prof.uuid = c.professionUuid;
    if (!dataclient->Read(prof))
    {
        LOG_ERROR << "Error reading profession " << c.professionUuid << std::endl;
        return AB::Entities::ProfessionPosition::None;
    }

    return prof.position;
}

std::string Queue::FindServerForMatch(const MatchTeams& teams)
{
    auto* dataclient = GetSubsystem<IO::DataClient>();

    std::vector<std::string> servers;
    std::map<std::string, unsigned> sorting;
    for (const auto& team : teams)
    {
        for (const auto& m : team.members)
        {
            AB::Entities::Character ch;
            ch.uuid = m;
            if (!dataclient->Read(ch))
                continue;
            AB::Entities::Account acc;
            acc.uuid = ch.accountUuid;
            if (!dataclient->Read(acc))
                continue;

            servers.push_back(acc.currentServerUuid);
            sorting[acc.currentServerUuid] += 1;
        }
    }
    if (servers.size() == 0)
    {
        LOG_ERROR << "No server found" << std::endl;
        return "";
    }

    // Make server with most players on it the first
    std::sort(servers.begin(), servers.end(), [&sorting](const std::string& i1, const std::string& i2)
    {
        const auto& p1 = sorting[i1];
        const auto& p2 = sorting[i2];
        return p1 > p2;
    });
    return servers[0];
}

Queue::Queue(const std::string& mapUuid) :
    mapUuid_(mapUuid)
{
    uuid_ = Utils::Uuid::New();
}

bool Queue::Load()
{
    auto* dataclient = GetSubsystem<IO::DataClient>();
    AB::Entities::Game game;
    game.uuid = mapUuid_;
    if (!dataclient->Read(game))
    {
        LOG_ERROR << "Error reading map with UUID " << mapUuid_ << std::endl;
        return false;
    }
    partySize_ = game.partySize;
    if (partySize_ == 0)
    {
        LOG_ERROR << "Party size can not be 0" << std::endl;
        return false;
    }
    partyCount_ = game.partyCount;
    if (partyCount_ == 0)
    {
        LOG_ERROR << "Party count can not be 0" << std::endl;
        return false;
    }
    randomParty_ = game.randomParty;
    return true;
}

void Queue::Add(const std::string& uuid)
{
    entries_.push_back({
       uuid,
       randomParty_ ? GetPlayerPosition(uuid) : AB::Entities::ProfessionPosition::None
   });

    auto* client = GetSubsystem<Net::MessageClient>();
    Net::MessageMsg msg;
    msg.type_ = Net::MessageType::PlayerAddedToQueue;

    sa::PropWriteStream stream;
    stream.WriteString(uuid);
    stream.WriteString(uuid_);
    msg.SetPropStream(stream);
    client->Write(msg);
}

void Queue::Remove(const std::string& uuid)
{
    auto it = ea::find_if(entries_.begin(), entries_.end(), [&](const QueueEntry& current) {
        return Utils::Uuid::IsEqual(current.uuid, uuid);
    });
    if (it != entries_.end())
    {
        entries_.erase(it);

        auto* client = GetSubsystem<Net::MessageClient>();
        Net::MessageMsg msg;
        msg.type_ = Net::MessageType::PlayerRemovedFromQueue;

        sa::PropWriteStream stream;
        stream.WriteString(uuid);
        stream.WriteString(uuid_);
        msg.SetPropStream(stream);
        client->Write(msg);
    }
}

bool Queue::SendEnterMessage(const MatchTeams& teams)
{
    auto* client = GetSubsystem<Net::MessageClient>();
    Net::MessageMsg msg;
    msg.type_ = Net::MessageType::TeamsEnterMatch;

    sa::PropWriteStream stream;
    // This server will host the game
    std::string server = FindServerForMatch(teams);
    if (server.empty())
    {
        // This should really not happen
        LOG_ERROR << "Did not find any suitable server" << std::endl;
        return false;
    }
    stream.WriteString(uuid_);
    stream.WriteString(server);
    stream.WriteString(mapUuid_);
    // This will be the instance UUID so that all teams enter the same game instance
    const std::string instanceUuid = Utils::Uuid::New();
    stream.WriteString(instanceUuid);

    stream.Write<uint8_t>(static_cast<uint8_t>(teams.size()));
    for (const auto& team : teams)
    {
        stream.Write<uint8_t>(static_cast<uint8_t>(team.members.size()));
        for (const auto& member : team.members)
            stream.WriteString(member);
    }
    msg.SetPropStream(stream);
    return client->Write(msg);
}

std::optional<QueueEntry> Queue::GetPlayerByPos(AB::Entities::ProfessionPosition pos)
{
    if (entries_.empty())
        return {};

    auto it = ea::find_if(entries_.begin(), entries_.end(), [&](const auto& current) {
        return current.position == pos;
    });
    if (it != entries_.end())
    {
        std::optional<QueueEntry> result = (*it);
        entries_.erase(it);
        return result;
    }
    // No player for the position, return the first in the queue
    std::optional<QueueEntry> result = entries_.front();
    entries_.pop_front();
    return result;
}

bool Queue::MakeRandomTeam(Team& team)
{
    team.members.reserve(partySize_);
    if (partySize_ == 1)
    {
        QueueEntry& e = entries_.front();
        team.members.push_back(e.uuid);
        entries_.pop_front();
        return true;
    }

    const auto addPlayers = [&team, this](unsigned count, AB::Entities::ProfessionPosition pos) -> bool
    {
        for (unsigned i = 0; i < count; ++i)
        {
            auto player = GetPlayerByPos(pos);
            if (!player.has_value())
            {
                LOG_ERROR << "Not enough players" << std::endl;
                return false;
            }
            team.members.push_back(player.value().uuid);
        }
        return true;
    };

    // 1/4 front liners
    const unsigned frontlinecount = partySize_ / 4;
    // 1/4 back liners
    const unsigned backlinecount = partySize_ / 4;
    // Rest is mid line
    const unsigned midlinecount = partySize_ - frontlinecount - backlinecount;
    if (!addPlayers(frontlinecount, AB::Entities::ProfessionPosition::Frontline))
        return false;
    if (!addPlayers(midlinecount, AB::Entities::ProfessionPosition::Midline))
        return false;
    if (!addPlayers(backlinecount, AB::Entities::ProfessionPosition::Backline))
        return false;

    return true;
}

bool Queue::MakeRandomTeams(MatchTeams& teams)
{
    teams.reserve(partyCount_);

    for (unsigned i = 0; i < partyCount_; ++i)
    {
        Team team;
        if (!MakeRandomTeam(team))
        {
            LOG_ERROR << "Not enough players" << std::endl;
            return false;
        }
        teams.push_back(std::move(team));
    }

    return true;
}

bool Queue::MakePartyTeams(MatchTeams& teams)
{
    teams.reserve(partyCount_);

    // Not a random team, so only the party leader queued for a game
    for (unsigned i = 0; i < partyCount_; ++i)
    {
        if (entries_.empty())
        {
            // Shouldn't happen because we check first if there are enough players
            LOG_ERROR << "Not enough players" << std::endl;
            return false;
        }
        Team team;
        QueueEntry& e = entries_.front();
        team.members.push_back(e.uuid);
        entries_.pop_front();
        teams.push_back(std::move(team));
    }
    return true;
}

bool Queue::MakeTeams(MatchTeams& teams)
{
    if (randomParty_)
        return MakeRandomTeams(teams);
    return MakePartyTeams(teams);
}

void Queue::Update(uint32_t, const std::function<void(const std::string& playerUuid)>& callback)
{
    // Create as many matches as possible.
    while (EnoughPlayers())
    {
        sa::Transaction transaction(entries_);
        MatchTeams teams;
        if (MakeTeams(teams))
        {
            if (SendEnterMessage(teams))
            {
                transaction.Commit();
                for (const auto& team : teams)
                {
                    for (const auto& member : team.members)
                        callback(member);
                }
            }
        }
    }
}
