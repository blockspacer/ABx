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

#include "Errors.h"
#include <list>
#include <memory>
#include <functional>
#include <asio.hpp>
#include <sa/time.h>

namespace Client {

class Connection : public std::enable_shared_from_this<Connection>
{
private:
    typedef std::function<void(ConnectionError, const asio::error_code&)> ErrorCallback;
    typedef std::function<void(uint8_t*, size_t)> RecvCallback;
    enum {
        ConnectTimeout = 10,
        ReadTimeout = 30,
        WriteTimeout = 30,
        SendBufferSize = 65536,
        RecvBufferSize = 65536
    };
    void OnResolve(const asio::error_code& error, asio::ip::tcp::resolver::iterator endpointIterator);
    void OnConnectTimeout(const asio::error_code& error);
    void OnReadTimeout(const asio::error_code& error);
    void OnWriteTimeout(const asio::error_code& error);
    void OnConnect(const asio::error_code& error);
    void OnWrite(const asio::error_code& error, size_t writeSize, std::shared_ptr<asio::streambuf> outputStream);
    void OnCanWrite(const asio::error_code& error);
    void OnRecv(const asio::error_code& error, size_t recvSize);
    void HandleError(ConnectionError connectionError, const asio::error_code& error);
    void InternalConnect(asio::ip::basic_resolver<asio::ip::tcp>::iterator endpointIterator);
    void InternalWrite();
protected:
    std::function<void()> connectCallback_;
    ErrorCallback errorCallback_;
    RecvCallback recvCallback_;
    asio::steady_timer readTimer_;
    asio::steady_timer connectTimer_;
    asio::steady_timer writeTimer_;
    asio::steady_timer delayedWriteTimer_;
    asio::ip::tcp::resolver resolver_;
    asio::ip::tcp::socket socket_;
    bool connected_;
    bool connecting_;
    asio::error_code error_;
    static std::list<std::shared_ptr<asio::streambuf>> outputStreams_;
    std::shared_ptr<asio::streambuf> outputStream_;
    asio::streambuf inputStream_;
    sa::time::timer activityTimer_;
public:
    explicit Connection(asio::io_service& ioService);
    ~Connection();

    static void Terminate();

    void Connect(const std::string& host, uint16_t port, const std::function<void()>& connectCallback);
    void Close();

    void Write(uint8_t* buffer, size_t size);
    void Read(size_t bytes, const RecvCallback& callback);

    void SetErrorCallback(const ErrorCallback& errorCallback) { errorCallback_ = errorCallback; }

    bool IsConnecting() const { return connecting_; }
    bool IsConnected() const { return connected_; }
    uint32_t GetIp() const {
        if (connected_)
        {
            asio::ip::tcp::endpoint remote_ep = socket_.remote_endpoint();
            asio::ip::address remote_ad = remote_ep.address();
            return remote_ad.to_v4().to_uint();
        }
        return 0;
    }
};

}
