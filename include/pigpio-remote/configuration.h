#ifndef __PIGPIO_CONFIGURATIONH_H__
#define __PIGPIO_CONFIGURATIONH_H__

#if !(defined(PIGPIO_REMOTE_PLATFORM_ARDUINO) ^ defined(PIGPIO_REMOTE_PLATFORM_POSIX))
#    ifdef ARDUINO
#        define PIGPIO_REMOTE_PLATFORM_ARDUINO
#    else
#        error "Unique target platform macro PIGPI_REMOTE_PLATFORM must be defined."
#    endif
#endif

namespace version
{
    constexpr unsigned int PIGPIO_INTERFACE = 78;
    constexpr unsigned int MAJOR = 0;
    constexpr unsigned int MINOR = 3;
    constexpr unsigned int PATCH = 0;
};


#endif // __PIGPIO_CONFIGURATIONH_H__