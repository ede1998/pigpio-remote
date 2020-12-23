#ifndef __BASENONAGLESYNCCLIENT_H__
#define __BASENONAGLESYNCCLIENT_H__

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
    inline ConnectionError connect(const char *addr, uint16_t port)
    {
        return static_cast<T *>(this)->_connect(addr, port);
    }

    inline void stop()
    {
        static_cast<T*>(this)->_stop();
    }

    inline bool connected() const
    {
        return static_cast<const T *>(this)->_connected();
    }

    inline int available() const
    {
        return static_cast<const T *>(this)->_available();
    }

    inline int read(uint8_t *data, size_t len)
    {
        return static_cast<T *>(this)->_read(data, len);
    }

    inline size_t write(const uint8_t *data, size_t len)
    {
        return static_cast<T *>(this)->_write(data, len);
    }
};

#endif // __BASENONAGLESYNCCLIENT_H__