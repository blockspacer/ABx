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

#include "BanManager.h"
#include "Utils.h"
#include <AB/Entities/IpBan.h>
#include <AB/Entities/AccountBan.h>
#include <AB/Entities/AccountBanList.h>
#include "DataClient.h"
#include "Profiler.h"
#include "Subsystems.h"
#include "UuidUtils.h"
#include <sa/time.h>

namespace Auth {

uint32_t BanManager::LoginTries = 5;
uint32_t BanManager::LoginRetryTimeout = 5000;

bool BanManager::AcceptConnection(uint32_t clientIP)
{
    if (clientIP == 0)
        return false;

    // Too many unsuccessful login attempts -> refuse connection
    // Only the login server counts login attempts so this is not relevant for
    // the game server.
    if (IsIpDisabled(clientIP))
        return false;

    int64_t currentTime = sa::time::tick();
    std::map<uint32_t, ConnectBlock>::iterator it = ipConnects_.find(clientIP);
    if (it == ipConnects_.end())
    {
        std::scoped_lock lock(lock_);
        ipConnects_.emplace(clientIP, ConnectBlock(currentTime, 0, 1));
        return true;
    }

    ConnectBlock& cb = it->second;
    {
        std::scoped_lock lock(lock_);
        ++cb.count;
    }
    if (cb.blockTime > currentTime)
        return false;

    if (currentTime - cb.startTime > 1000)
    {
        std::scoped_lock lock(lock_);
        uint32_t connectionsPerSec = cb.count;
        cb.startTime = currentTime;
        cb.count = 0;
        cb.blockTime = 0;

        if (connectionsPerSec > 10)
        {
            cb.blockTime = currentTime + 10000;
            return false;
        }
    }

    return true;
}

bool BanManager::IsIpBanned(uint32_t clientIP, uint32_t mask /* = 0xFFFFFFFF */) const
{
    if (clientIP == 0)
        return false;

    IO::DataClient* client = GetSubsystem<IO::DataClient>();
    if (!client)
        return false;

    AB::Entities::IpBan ban;
    ban.ip = clientIP;
    ban.mask = mask;
    if (!client->Read(ban))
        return false;
    AB::Entities::Ban _ban;
    _ban.uuid = ban.banUuid;
    if (!client->Read(_ban))
        return false;
    if (!_ban.active)
        return false;
    bool result = (_ban.expires <= 0) || (_ban.expires >= sa::time::tick());
    if (result)
    {
        ++_ban.hits;
        client->Update(_ban);
    }
    return result;
}

bool BanManager::IsAccountBanned(const uuids::uuid& accountUuid)
{
    IO::DataClient* client = GetSubsystem<IO::DataClient>();
    if (!client)
        return false;

    AB::Entities::AccountBanList banlist;
    banlist.uuid = accountUuid.to_string();
    if (!client->Read(banlist))
        return false;
    for (const auto& uuid : banlist.uuids)
    {
        AB::Entities::Ban _ban;
        _ban.uuid = uuid;
        if (!client->Read(_ban))
            continue;
        if (!_ban.active)
            continue;
        if ((_ban.expires <= 0) || (_ban.expires >= sa::time::tick()))
        {
            ++_ban.hits;
            client->Update(_ban);
            return true;
        }
    }
    return false;
}

bool BanManager::IsIpDisabled(uint32_t clientIP) const
{
    if (clientIP == 0)
        return false;

    if (BanManager::LoginTries == 0)
        return false;

    const auto it = ipLogins_.find(clientIP);
    if (it == ipLogins_.end())
        return false;

    if ((it->second.numberOfLogins >= BanManager::LoginTries) &&
        sa::time::time_elapsed(it->second.lastLoginTime) < BanManager::LoginRetryTimeout)
        return true;

    return false;
}

void BanManager::AddLoginAttempt(uint32_t clientIP, bool success)
{
    if (clientIP == 0)
        return;

    int64_t currentTime = sa::time::tick();
    std::scoped_lock lock(lock_);
    auto it = ipLogins_.find(clientIP);
    if (it == ipLogins_.end())
    {
        LoginBlock lb{ 0, 0 };
        ipLogins_[clientIP] = lb;
        it = ipLogins_.find(clientIP);
    }

    if (it->second.numberOfLogins >= BanManager::LoginTries)
        it->second.numberOfLogins = 0;

    if (!success || (currentTime < it->second.lastLoginTime + BanManager::LoginRetryTimeout))
        ++it->second.numberOfLogins;
    else
        it->second.numberOfLogins = 0;
    it->second.lastLoginTime = currentTime;
}

bool BanManager::AddIpBan(uint32_t ip, uint32_t mask, int64_t expires, const std::string& adminUuid,
    const std::string& comment, AB::Entities::BanReason reason /* = AB::Entities::BanReasonOther */)
{
    if (ip == 0 || mask == 0)
        return false;

    IO::DataClient* client = GetSubsystem<IO::DataClient>();
    if (!client)
        return false;

    AB::Entities::Ban ban;
    ban.uuid = Utils::Uuid::New();
    ban.expires = expires;
    ban.added = sa::time::tick();
    ban.reason = reason;
    ban.adminUuid = adminUuid;
    ban.comment = comment;
    if (!client->Create(ban))
        return false;

    AB::Entities::IpBan ipBan;
    ipBan.banUuid = ban.uuid;
    ipBan.ip = ip;
    ipBan.mask = mask;
    return client->Create(ipBan);
}

bool BanManager::AddAccountBan(const std::string& accountUuid, int64_t expires, const std::string& adminUuid,
    const std::string& comment, AB::Entities::BanReason reason /* = AB::Entities::BanReasonOther */)
{
    if (Utils::Uuid::IsEmpty(accountUuid))
        return false;

    IO::DataClient* client = GetSubsystem<IO::DataClient>();
    AB::Entities::Ban ban;
    ban.uuid = Utils::Uuid::New();
    ban.expires = expires;
    ban.added = sa::time::tick();
    ban.reason = reason;
    ban.adminUuid = adminUuid;
    ban.comment = comment;
    if (!client->Create(ban))
        return false;

    AB::Entities::AccountBan accBan;
    accBan.banUuid = ban.uuid;
    accBan.accountUuid = accountUuid;
    bool result = client->Create(accBan);
    if (result)
    {
        AB::Entities::AccountBanList banlist;
        banlist.uuid = accountUuid;
        client->Invalidate(banlist);
    }
    return result;
}

}
