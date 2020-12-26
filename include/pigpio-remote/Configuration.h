#ifndef PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_CONFIGURATION_H
#define PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_CONFIGURATION_H

#if !(defined(PIGPIO_REMOTE_PLATFORM_ARDUINO) ^ defined(PIGPIO_REMOTE_PLATFORM_POSIX))
#ifdef ARDUINO
#define PIGPIO_REMOTE_PLATFORM_ARDUINO
#else
#error "Unique target platform macro PIGPI_REMOTE_PLATFORM must be defined."
#endif
#endif

namespace pigpio_remote
{
    namespace version
    {
        /**
         * @brief INTERFACE version of the Pigpio daemon to communicate with.
         */
        constexpr unsigned int PIGPIO_INTERFACE = 78;

        /**
         * @brief Major version number of pigpio_remote library.
         */
        constexpr unsigned int MAJOR = 1;

        /**
         * @brief Minor version number of pigpio_remote library.
         */
        constexpr unsigned int MINOR = 0;

        /**
         * @brief Patch number of pigpio_remote library.
         */
        constexpr unsigned int PATCH = 0;

    } // namespace version

} // namespace pigpio_remote

#endif // PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_CONFIGURATION_H
