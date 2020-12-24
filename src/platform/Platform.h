#ifndef PIGPIO_REMOTE_SRC_PLATFORM_PLATFORM_H 
#define PIGPIO_REMOTE_SRC_PLATFORM_PLATFORM_H 

#include "../include/pigpio-remote/Configuration.h"

#ifdef PIGPIO_REMOTE_PLATFORM_ARDUINO
#include "Arduino.h"

inline void Wait(unsigned long ms)
{
    delay(ms);
}
#endif

#ifdef PIGPIO_REMOTE_PLATFORM_POSIX
#include <ctime>
#include <cerrno>
#include <cassert>
void Wait(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        assert(false && "Cannot wait less than 0 millseconds.");
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do
    {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);
}
#endif

#endif // PIGPIO_REMOTE_SRC_PLATFORM_PLATFORM_H