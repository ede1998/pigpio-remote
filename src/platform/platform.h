#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "../include/pigpio-remote/configuration.h"

#ifdef PIGPIO_REMOTE_PLATFORM_ARDUINO
#include "Arduino.h"

void wait(unsigned long ms) inline
{
    delay(ms);
}
#endif

#ifdef PIGPIO_REMOTE_PLATFORM_POSIX
#include <ctime>
#include <cerrno>    
#include <cassert>
void wait(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        assert(false && "Cannot wait less than 0 millseconds.");
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);
}
#endif


#endif // __PLATFORM_H__