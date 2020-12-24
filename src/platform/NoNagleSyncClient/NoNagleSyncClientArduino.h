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
    SyncClientInternal _client;

    inline ConnectionError InternalConnect(const char *ip, uint16_t port)
    {
        auto result = this->_client.connect(ip, port);
        return static_cast<ConnectionError>(result == 1 ? 0 : result);
    }

    inline bool InternalConnected() const
    {
        return const_cast<SyncClient &>(this->_client).connected();
    }
    inline int InternalAvailable() const
    {
        return const_cast<SyncClient &>(this->_client).available();
    }

    inline int InternalRead(uint8_t *data, size_t len)
    {
        return this->_client.read(data, len);
    }

    size_t InternalWrite(const uint8_t *data, size_t len)
    {
        auto result = this->_client.write(data, len);
        if (result != sizeof(data))
        {
            return result;
        }

        this->_client.flush();
        return result;
    }

    inline void InternalStop()
    {
        this->_client.stop();
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