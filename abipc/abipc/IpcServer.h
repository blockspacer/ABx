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

#include "Message.h"
#include "MessageBuffer.h"
#include <asio.hpp>
#include <mutex>
#include <sa/CallableTable.h>
#include <sa/StringHash.h>
#include <sa/TypeName.h>
#include <set>
#include <stdint.h>

namespace IPC {

class ServerConnection;

class ServerMessageHandlers
{
private:
    sa::CallableTable<uint64_t, void, ServerConnection&, const MessageBuffer&> handlers_;
public:
    template<typename _Msg>
    void Add(std::function<void(ServerConnection&, const _Msg& msg)>&& func)
    {
        static constexpr uint64_t message_type = static_cast<uint64_t>(sa::StringHash(sa::TypeName<_Msg>::Get()));
        handlers_.Add(message_type, [handler = std::move(func)](ServerConnection& client, const MessageBuffer& buffer)
        {
            // Get() doesn't take const stuff, but the buffer isn't modified by it so just cast it away.
            auto packet = Get<_Msg>(const_cast<MessageBuffer&>(buffer));
            handler(client, packet);
        });
    }
    bool Exists(uint64_t type) const { return handlers_.Exists(type); }
    void Call(uint64_t type, ServerConnection& client, const MessageBuffer& buffer)
    {
        handlers_.Call(type, client, buffer);
    }
};

class Server
{
    friend class ServerConnection;
private:
    asio::io_service& ioService_;
    asio::ip::tcp::acceptor acceptor_;
    std::set<std::shared_ptr<ServerConnection>> clients_;
    std::mutex lock_;
    void StartAccept();
    void HandleAccept(std::shared_ptr<ServerConnection> connection, const asio::error_code& error);
    // Send a message to all clients
    void InternalSend(const MessageBuffer& msg);
    void InternalSendTo(ServerConnection& client, const MessageBuffer& msg);
    void AddConnection(std::shared_ptr<ServerConnection> conn);
    ServerConnection* GetConnection(uint32_t id);
    void RemoveConnection(std::shared_ptr<ServerConnection> conn);
    void HandleMessage(ServerConnection& client, MessageBuffer& msg);
public:
    Server(asio::io_service& ioService, const asio::ip::tcp::endpoint& endpoint);
    ~Server() = default;
    void DisconnectClient(ServerConnection& client);
    void DisconnectClient(uint32_t id);
    // Send the message to all connected clients
    template <typename _Msg>
    void Send(_Msg& msg)
    {
        static constexpr uint64_t message_type = static_cast<uint64_t>(sa::StringHash(sa::TypeName<_Msg>::Get()));
        MessageBuffer buff;
        buff.type_ = message_type;
        Add(msg, buff);
        buff.EncodeHeader();
        InternalSend(buff);
    }
    // Send the message to the given client
    template <typename _Msg>
    void SendTo(ServerConnection& client, _Msg& msg)
    {
        static constexpr uint64_t message_type = static_cast<uint64_t>(sa::StringHash(sa::TypeName<_Msg>::Get()));
        MessageBuffer buff;
        buff.type_ = message_type;
        Add(msg, buff);
        buff.EncodeHeader();
        InternalSendTo(client, buff);
    }
    // Send the message to the given client identified by the clients ID.
    template <typename _Msg>
    void SendTo(uint32_t clientId, _Msg& msg)
    {
        static constexpr uint64_t message_type = static_cast<uint64_t>(sa::StringHash(sa::TypeName<_Msg>::Get()));
        auto* client = GetConnection(clientId);
        if (client == nullptr)
            return;
        MessageBuffer buff;
        buff.type_ = message_type;
        Add(msg, buff);
        buff.EncodeHeader();
        InternalSendTo(*client, buff);
    }
    ServerMessageHandlers handlers_;
    // Client attempts to connect. Return false to refuse connection.
    std::function<bool(const ServerConnection&)> onAccept;
    std::function<void(ServerConnection&)> onClientConnect;
    std::function<void(ServerConnection&)> onClientDisconnect;
};

}
