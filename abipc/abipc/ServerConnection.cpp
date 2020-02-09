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

#include "stdafx.h"
#include "ServerConnection.h"
#include "IpcServer.h"

namespace IPC {

ServerConnection::ServerConnection(asio::io_service& ioService, Server& server) :
    socket_(ioService),
    server_(server)
{ }

void ServerConnection::Start()
{
    server_.AddConnection(shared_from_this());
    asio::async_read(socket_,
        asio::buffer(readBuffer_.Data(), MessageBuffer::HeaderLength),
        std::bind(
            &ServerConnection::HandleRead, shared_from_this(), std::placeholders::_1
        ));
}

void ServerConnection::HandleRead(const asio::error_code& error)
{
    if (error)
    {
        server_.RemoveConnection(shared_from_this());
        return;
    }

    asio::read(socket_,
        asio::buffer(readBuffer_.Body(), readBuffer_.BodyLength()));

    // Client sent a message
    if (readBuffer_.DecodeHeader())
        server_.HandleMessage(readBuffer_);

    asio::async_read(socket_,
        asio::buffer(readBuffer_.Data(), MessageBuffer::HeaderLength),
        std::bind(
            &ServerConnection::HandleRead, shared_from_this(), std::placeholders::_1
        ));
}

void ServerConnection::HandleWrite(const asio::error_code& error)
{
    if (error)
        return;

    writeBuffers_.pop_front();
    if (!writeBuffers_.empty())
    {
        asio::async_write(socket_,
            asio::buffer(writeBuffers_.front().Data(), writeBuffers_.front().Length()),
            std::bind(&ServerConnection::HandleWrite, shared_from_this(), std::placeholders::_1));
    }
}

void ServerConnection::Send(const MessageBuffer& msg)
{
    bool writeInProgress = !writeBuffers_.empty();
    writeBuffers_.push_back(msg);
    if (!writeInProgress)
    {
        asio::async_write(socket_,
            asio::buffer(writeBuffers_.front().Data(), writeBuffers_.front().Length()),
            std::bind(&ServerConnection::HandleWrite, shared_from_this(), std::placeholders::_1));
    }
}

}