#ifndef __NONAGLESYNCCLIENT_H__
#define __NONAGLESYNCCLIENT_H__

#include "../../include/pigpio-remote/Configuration.h"

#ifdef PIGPIO_REMOTE_PLATFORM_ARDUINO
#include "NoNagleSyncClient/NoNagleSyncClientArduino.h"
#endif

#ifdef PIGPIO_REMOTE_PLATFORM_POSIX
#include "NoNagleSyncClient/NoNagleSyncClientPosix.h"
#endif

#endif // __NONAGLESYNCCLIENT_H__