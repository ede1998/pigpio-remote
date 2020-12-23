#ifndef _PIGPIO_WRAPPER_NONAGLESYNCCLIENT
#define _PIGPIO_WRAPPER_NONAGLESYNCCLIENT 1

#include "ESPAsyncTCP.h"
#include "SyncClient.h"

class NoNagleSyncClient : public SyncClient
{
public:
    size_t immediate_write(uint8_t data)
    {
        auto result = this->write(data);
        if (result != sizeof(data))
        {
            return result;
        }

        this->flush();
        return result;
    }
    size_t immediate_write(const uint8_t *data, size_t len)
    {
        auto result = this->write(data, len);
        if (result != sizeof(data))
        {
            return result;
        }

        this->flush();
        return result;
    }
};

#endif // _PIGPIO_WRAPPER_NONAGLESYNCCLIENT
