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

#include "IpcClient.h"

namespace IPC {

void Client::InternalConnect()
{
    if (connected_)
        return;

    const asio::ip::tcp::resolver::query query(asio::ip::tcp::v4(), host_, std::to_string(port_));
    asio::ip::tcp::resolver::iterator endpoint = resolver_.resolve(query);
    asio::error_code error;
    socket_.connect(*endpoint, error);
    if (!error)
    {
        connected_ = true;
        AsyncReceive(&readBuffer_,
            std::bind(&Client::HandleRead,
                this, std::placeholders::_1, std::placeholders::_2));
    }
}

void Client::HandleReadHeader(MessageBuffer* msg, const ReadHandler& handler, const asio::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        if (msg->DecodeHeader())
        {
            asio::async_read(socket_,
                asio::buffer(msg->Body(), msg->BodyLength()),
                handler);
        }
    }
    else
    {
        handler(error, bytes_transferred);
    }
}

void Client::AsyncReceive(MessageBuffer* msg, const ReadHandler& handler)
{
    asio::async_read(socket_,
        asio::buffer(msg->Data(), MessageBuffer::HeaderLength),
        std::bind(
            &Client::HandleReadHeader,
            this, msg, handler, std::placeholders::_1, std::placeholders::_2));
}

void Client::HandleRead(const asio::error_code& error, size_t)
{
    if (!connected_)
        return;
    if (error)
    {
        Close();
        return;
    }

    if (!readBuffer_.IsEmpty())
        HandleMessage(readBuffer_);
    readBuffer_.Empty();
    AsyncReceive(&readBuffer_,
        std::bind(&Client::HandleRead,
            this, std::placeholders::_1, std::placeholders::_2));
}

void Client::HandleWrite(const asio::error_code& error)
{
    if (!connected_)
        return;
    if (error)
    {
        Close();
        return;
    }

    writeBuffers_.pop_front();
    if (error)
        return;
    if (!writeBuffers_.empty())
        Write();
}

void Client::HandleMessage(const MessageBuffer& msg)
{
    if (handlers_.Exists(msg.type_))
        handlers_.Call(msg.type_, msg);
}

bool Client::Connect(const std::string& host, uint16_t port)
{
    host_ = host;
    port_ = port;
    InternalConnect();
    return connected_;
}

bool Client::InternalSend(const MessageBuffer& msg)
{
    if (connected_)
    {
        strand_.post(std::bind(&Client::WriteImpl, this, msg));
        return true;
    }
    return false;
}

void Client::WriteImpl(const MessageBuffer& msg)
{
    writeBuffers_.push_back(msg);
    if (writeBuffers_.size() > 1)
        // Write in progress
        return;
    Write();
}

void Client::Write()
{
    const MessageBuffer& msg = writeBuffers_[0];
    asio::async_write(
        socket_,
        asio::buffer(msg.Data(), msg.Length()),
        strand_.wrap(
            std::bind(
                &Client::HandleWrite,
                this,
                std::placeholders::_1
            )
        )
    );
}

}
