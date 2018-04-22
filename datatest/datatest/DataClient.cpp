#include "stdafx.h"
#include "DataClient.h"
#include <chrono>
#include <thread>

DataClient::DataClient(asio::io_service& io_service) :
    resolver_(io_service),
    socket_(io_service),
    connected_(false)
{
}

DataClient::~DataClient()
{
    socket_.close();
}

void DataClient::Connect(const std::string& host, uint16_t port)
{
    host_ = host;
    port_ = port;
    TryConnect(false);
}

bool DataClient::ReadData(const std::vector<uint8_t>& key, std::vector<uint8_t>& data)
{
    uint8_t ksize1 = static_cast<uint8_t>(key.size());
    uint8_t ksize2 = static_cast<uint8_t>(key.size() >> 8);
    uint8_t header[] = { OpCodes::Read, ksize1, ksize2 };
    if (!TryWrite(asio::buffer(header)))
        return false;

    if (!TryWrite(asio::buffer(key)))
        return false;

    uint8_t dsize4 = static_cast<uint8_t>(data.size() >> 24);
    uint8_t dsize3 = static_cast<uint8_t>(data.size() >> 16);
    uint8_t dsize2 = static_cast<uint8_t>(data.size() >> 8);
    uint8_t dsize1 = static_cast<uint8_t>(data.size());

    uint8_t data_header[] = { dsize1, dsize2, dsize3, dsize4 };
    if (!TryWrite(asio::buffer(data_header)))
        return false;

    if (!TryWrite(asio::buffer(data)))
        return false;

    std::vector<uint8_t> dataheader(5);
    size_t read = asio::read(socket_, asio::buffer(dataheader, 5), asio::transfer_at_least(5));
    if (dataheader[0] != OpCodes::Data)
    {
        // Read the rest of the staus
        uint8_t result[128];
        socket_.read_some(asio::buffer(result));
        return false;
    }

    const int size = ToInt32(dataheader, 1);
    data.resize(size);
    size_t read2 = asio::read(socket_, asio::buffer(data), asio::transfer_at_least(size));
    if (read2 == 0)
        return false;

    return true;
}

bool DataClient::DeleteData(const std::vector<uint8_t>& key)
{
    uint8_t ksize1 = static_cast<uint8_t>(key.size());
    uint8_t ksize2 = static_cast<uint8_t>(key.size() >> 8);
    uint8_t header[] = { OpCodes::Delete, ksize1, ksize2 };
    if (!TryWrite(asio::buffer(header)))
        return false;

    if (!TryWrite(asio::buffer(key)))
        return false;

    uint8_t result[128];
    size_t read = socket_.read_some(asio::buffer(result));
    if (read < 2)
        return false;

    if (result[0] == OpCodes::Status && result[1] == ErrorCodes::Ok)
        return true;
    return false;
}

bool DataClient::UpdateData(const std::vector<uint8_t>& key, std::vector<uint8_t>& data)
{
    uint8_t ksize1 = static_cast<uint8_t>(key.size());
    uint8_t ksize2 = static_cast<uint8_t>(key.size() >> 8);
    uint8_t header[] = { OpCodes::Update, ksize1, ksize2 };

    if (!TryWrite(asio::buffer(header)))
        return false;

    if (!TryWrite(asio::buffer(key)))
        return false;

    uint8_t dsize4 = static_cast<uint8_t>(data.size() >> 24);
    uint8_t dsize3 = static_cast<uint8_t>(data.size() >> 16);
    uint8_t dsize2 = static_cast<uint8_t>(data.size() >> 8);
    uint8_t dsize1 = static_cast<uint8_t>(data.size());

    uint8_t data_header[] = { dsize1, dsize2, dsize3, dsize4 };
    if (!TryWrite(asio::buffer(data_header)))
        return false;

    if (!TryWrite(asio::buffer(data)))
        return false;

    uint8_t result[128];
    size_t read = socket_.read_some(asio::buffer(result));
    if (read < 2)
        return false;
    return result[0] == OpCodes::Status && result[1] == ErrorCodes::Ok;
}

bool DataClient::CreateData(const std::vector<uint8_t>& key, std::vector<uint8_t>& data)
{
    uint8_t ksize1 = static_cast<uint8_t>(key.size());
    uint8_t ksize2 = static_cast<uint8_t>(key.size() >> 8);
    uint8_t header[] = { OpCodes::Create, ksize1, ksize2 };

    if (!TryWrite(asio::buffer(header)))
        return false;

    if (!TryWrite(asio::buffer(key)))
        return false;

    uint8_t dsize4 = static_cast<uint8_t>(data.size() >> 24);
    uint8_t dsize3 = static_cast<uint8_t>(data.size() >> 16);
    uint8_t dsize2 = static_cast<uint8_t>(data.size() >> 8);
    uint8_t dsize1 = static_cast<uint8_t>(data.size());

    uint8_t data_header[] = { dsize1, dsize2, dsize3, dsize4 };
    if (!TryWrite(asio::buffer(data_header)))
        return false;

    if (!TryWrite(asio::buffer(data)))
        return false;

    uint8_t result[128];
    size_t read = socket_.read_some(asio::buffer(result));
    if (read < 2)
        return false;

    return result[0] == OpCodes::Status && result[1] == ErrorCodes::Ok;
}

void DataClient::InternalConnect()
{
    if (connected_)
        return;

    asio::ip::tcp::resolver::query query(asio::ip::tcp::v4(), host_, std::to_string(port_));
    asio::ip::tcp::resolver::iterator endpoint = resolver_.resolve(query);
    asio::error_code error;
    socket_.connect(*endpoint, error);
    if (!error)
        connected_ = true;
}

bool DataClient::TryConnect(bool force)
{
    if (force)
    {
        connected_ = false;
        socket_.close();
    }
    // Try for 1 seconds, 10 tries
    int tries = 0;
    while (!connected_ && tries < 10)
    {
        ++tries;
        InternalConnect();
        if (connected_)
            return true;
        if (tries >= 10)
            return false;

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms);
    }
    return connected_;
}
