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

#include "MessageBuffer.h"
#include <asio.hpp>
#include <memory>
#include <sa/IdGenerator.h>
#include <sa/Noncopyable.h>
#include <mutex>

namespace IPC {

class Server;

class ServerConnection : public std::enable_shared_from_this<ServerConnection>
{
    NON_COPYABLE(ServerConnection)
private:
    static sa::IdGenerator<uint32_t> sIdGen;
    asio::ip::tcp::socket socket_;
    asio::io_service::strand strand_;
    Server& server_;
    uint32_t id_;
    MessageBuffer readBuffer_;
    BufferQueue writeBuffers_;
    void HandleRead(const asio::error_code& error);
    void HandleWrite(const asio::error_code& error, size_t bytesTransferred);
    void WriteImpl(const MessageBuffer& msg);
    void Write();
public:
    ServerConnection(asio::io_service& ioService, Server& server);
    ~ServerConnection() = default;
    asio::ip::tcp::socket& GetSocket() { return socket_; }

    void Start();
    void Close();
    // Send a message to this client
    void Send(const MessageBuffer& msg);
    // This ID can be used with the IPC::Server SendTo() function
    uint32_t GetId() const { return id_; }
    uint32_t GetIP() const;
    uint16_t GetPort() const;
};

}
