#ifndef PIGPIO_REMOTE_SRC_PLATFORM_NONAGLESYNCCLIENT_H
#define PIGPIO_REMOTE_SRC_PLATFORM_NONAGLESYNCCLIENT_H

#include "../../include/pigpio-remote/Configuration.h"

#ifdef PIGPIO_REMOTE_PLATFORM_ARDUINO
#include "NoNagleSyncClient/NoNagleSyncClientArduino.h"
#endif

#ifdef PIGPIO_REMOTE_PLATFORM_POSIX
#include "NoNagleSyncClient/NoNagleSyncClientPosix.h"
#endif

#endif // PIGPIO_REMOTE_SRC_PLATFORM_NONAGLESYNCCLIENT_H