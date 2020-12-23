#ifndef __NONAGLESYNCCLIENT_H__
#define __NONAGLESYNCCLIENT_H__

#include "../../include/pigpio-remote/configuration.h"

#ifdef PIGPIO_REMOTE_PLATFORM_ARDUINO
#include "NoNagleSyncClientArduino.h"
#endif

#ifdef PIGPIO_REMOTE_PLATFORM_POSIX
#include "NoNagleSyncClientPosix.h"
#endif

#endif // __NONAGLESYNCCLIENT_H__