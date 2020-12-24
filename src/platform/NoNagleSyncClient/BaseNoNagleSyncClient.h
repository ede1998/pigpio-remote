#ifndef PIGPIO_REMOTE_SRC_PLATFORM_NONAGLESYNCCLIENT_BASENONAGLESYNCCLIENT_H 
#define PIGPIO_REMOTE_SRC_PLATFORM_NONAGLESYNCCLIENT_BASENONAGLESYNCCLIENT_H 

#include <cstdint>
#include <cstddef>

enum class ConnectionError
{
    SUCCESS = 0,
    TIME_OUT = -1,
    INVALID_SERVER = -2,
    TRUNCATED = -3,
    INVALID_RESPONSE = -4,
    ALREADY_CONNECTED = -5
};

template <class T>
struct BaseNoNagleSyncClient
{
    inline ConnectionError Connect(const char *addr, uint16_t port)
    {
        return static_cast<T *>(this)->InternalConnect(addr, port);
    }

    inline void Stop()
    {
        static_cast<T*>(this)->InternalStop();
    }

    inline bool Connected() const
    {
        return static_cast<const T *>(this)->InternalConnected();
    }

    inline int Available() const
    {
        return static_cast<const T *>(this)->InternalAvailable();
    }

    inline int Read(uint8_t *data, size_t len)
    {
        return static_cast<T *>(this)->InternalRead(data, len);
    }

    inline size_t Write(const uint8_t *data, size_t len)
    {
        return static_cast<T *>(this)->InternalWrite(data, len);
    }
};

#endif // PIGPIO_REMOTE_SRC_PLATFORM_NONAGLESYNCCLIENT_BASENONAGLESYNCCLIENT_H