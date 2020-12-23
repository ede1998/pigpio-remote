#ifndef _PIGPIO_WRAPPER_NONAGLESYNCCLIENT
#define _PIGPIO_WRAPPER_NONAGLESYNCCLIENT 1

#ifdef PIGPIO_REMOTE_PLATFORM_ARDUINO

#include "ESPAsyncTCP.h"
#include "SyncClient.h"
#include "Arduino.h"
#include "BaseNoNagleSyncClient.h"

class NoNagleSyncClient : public BaseNoNagleSyncClient<NoNagleSyncClient>
{
private:
    friend class BaseNoNagleSyncClient;
    SyncClient _client;

    inline ConnectionError _connect(const char *ip, uint8_t port)
    {
        return this->_client.connect(ip, port);
    }

    inline bool _connected() const
    {
        return const_cast<const SyncClient &>(this->_client).connected();
    }
    inline int _available() const
    {
        return const_cast<const SyncClient &>(this->_client).available();
    }

    inline int _read(uint8_t *data, size_t len)
    {
        // auto start = std::chrono::steady_clock::now();
        // auto elapsed_milliseconds = int_milliseconds_t::zero();
        // while (this->_client.available() < static_cast<int>(sizeof(cmd)) &&
        //        elapsed_milliseconds < this->_timeout_milliseconds)
        // {
        //     delay(1);
        //     auto current = std::chrono::steady_clock::now();
        //     elapsed_milliseconds = std::chrono::duration_cast<int_milliseconds_t>(current - start);
        // }

        return this->_client.read(data, len);
    }

    size_t _write(const uint8_t *data, size_t len)
    {
        auto result = this->_client.write(data, len);
        if (result != sizeof(data))
        {
            return result;
        }

        this->_client.flush();
        return result;
    }

public:
    static constexpr int DEFAULT_SOCKET_TIMEOUT_SECONDS = 5;
    NoNagleSyncClient()
    {
        this->_client.setTimeout(DEFAULT_SOCKET_TIMEOUT_SECONDS);
    }
};

#endif // PIGPIO_REMOTE_PLATFORM_ARDUINO

#endif // _PIGPIO_WRAPPER_NONAGLESYNCCLIENT
