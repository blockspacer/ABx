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

#include <abscommon/ServerApp.h>
#include <AB/Entities/Account.h>
#if __cplusplus < 201703L
#   if !defined(__clang__) && !defined(__GNUC__)
#       include <filesystem>
#   else
#       include <experimental/filesystem>
#   endif
#else
#   include <filesystem>
#endif
#include <abscommon/MessageClient.h>
#include "Servers.h"
#include <numeric>
#include <sa/CircularQueue.h>
#include <sa/http_range.h>

#if __cplusplus < 201703L
// C++14
namespace fs = std::experimental::filesystem;
#else
// C++17
namespace fs = std::filesystem;
#endif

using HttpsServer = SimpleWeb::Server<SimpleWeb::HTTPS>;

namespace IO {
class DataClient;
}

template<typename T>
inline size_t stream_size(T& s)
{
    s.seekg(0, std::ios::end);
    size_t size = s.tellg();
    s.seekg(0, std::ios::beg);
    return size;
}

class Application final : public ServerApp, public std::enable_shared_from_this<Application>
{
private:
    bool requireAuth_;
    std::shared_ptr<asio::io_service> ioService_;
    int64_t startTime_;
    size_t bytesSent_;
    uint32_t uptimeRound_;
    int64_t statusMeasureTime_;
    int64_t lastLoadCalc_;
    bool temporary_;

    std::unique_ptr<HttpsServer> server_;
    std::string root_;
    std::string dataHost_;
    uint16_t dataPort_;
    /// Byte/sec
    uint64_t maxThroughput_;
    sa::CircularQueue<unsigned, 10> loads_;
    std::mutex mutex_;
    void HandleMessage(const Net::MessageMsg& msg);
    void UpdateBytesSent(size_t bytes);
    void HeartBeatTask();
    bool IsAllowed(std::shared_ptr<HttpsServer::Request> request);
    static SimpleWeb::CaseInsensitiveMultimap GetDefaultHeader();
    void SendFileRange(std::shared_ptr<HttpsServer::Response> response,
        const std::string& path,
        const sa::http::range& range,
        bool multipart, const std::string& boundary);
    void GetHandlerDefault(std::shared_ptr<HttpsServer::Response> response,
        std::shared_ptr<HttpsServer::Request> request);
    void GetHandlerFiles(std::shared_ptr<HttpsServer::Response> response,
        std::shared_ptr<HttpsServer::Request> request);
    void GetHandlerGames(std::shared_ptr<HttpsServer::Response> response,
        std::shared_ptr<HttpsServer::Request> request);
    void GetHandlerSkills(std::shared_ptr<HttpsServer::Response> response,
        std::shared_ptr<HttpsServer::Request> request);
    void GetHandlerProfessions(std::shared_ptr<HttpsServer::Response> response,
        std::shared_ptr<HttpsServer::Request> request);
    void GetHandlerAttributes(std::shared_ptr<HttpsServer::Response> response,
        std::shared_ptr<HttpsServer::Request> request);
    void GetHandlerEffects(std::shared_ptr<HttpsServer::Response> response,
        std::shared_ptr<HttpsServer::Request> request);
    void GetHandlerItems(std::shared_ptr<HttpsServer::Response> response,
        std::shared_ptr<HttpsServer::Request> request);
    void GetHandlerQuests(std::shared_ptr<HttpsServer::Response> response,
        std::shared_ptr<HttpsServer::Request> request);
    void GetHandlerMusic(std::shared_ptr<HttpsServer::Response> response,
        std::shared_ptr<HttpsServer::Request> request);
    void GetHandlerVersion(std::shared_ptr<HttpsServer::Response> response,
        std::shared_ptr<HttpsServer::Request> request);
    void GetHandlerVersions(std::shared_ptr<HttpsServer::Response> response,
        std::shared_ptr<HttpsServer::Request> request);
    void GetHandlerNews(std::shared_ptr<HttpsServer::Response> response,
        std::shared_ptr<HttpsServer::Request> request);
    void HandleError(std::shared_ptr<HttpsServer::Request> /*request*/,
        const SimpleWeb::error_code& ec);
    bool HandleOnAccept(const asio::ip::tcp::endpoint& endpoint);

    unsigned GetAvgLoad() const
    {
        if (loads_.IsEmpty())
            return 0;
        return std::accumulate(loads_.begin(), loads_.end(), 0u) / static_cast<unsigned>(loads_.Size());
    }
    void ShowLogo();
protected:
    bool ParseCommandLine() override;
    void ShowVersion() override;
public:
    Application();
    ~Application() override;

    bool Initialize(const std::vector<std::string>& args) override;
    void Run() override;
    void Stop() override;
};

