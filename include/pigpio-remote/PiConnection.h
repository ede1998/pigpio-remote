#ifndef PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_PICONNECTION_H
#define PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_PICONNECTION_H

#include "../../src/platform/NoNagleSyncClient.h"
#include "PigpioError.h"
#include "../../src/communication/Command.h"
#include "../../src/PiConnectionInternals.h"
#include <string>
#include <chrono>

namespace pigpio_remote
{
    using platform::ConnectionError;

    class PiConnection
    {
    private:
        platform::NoNagleSyncClient _client;

    public:
        PiConnection() = default;
        ~PiConnection();
        ConnectionError Connect(uint16_t port = 0);
        ConnectionError Connect(const std::string &addr, uint16_t port = 0);
        ConnectionError Connect(const char *addr, uint16_t port = 0);
        bool Connected() const;

        void Stop();

        internal::CommandResult SendCommand(communication::Command command, uint32_t parameter1, uint32_t parameter2 = UNUSED_PARAMETER);

        platform::NoNagleSyncClient GetConnection();
        static constexpr uint16_t DEFAULT_PORT = 8888;
        static constexpr const char *DEFAULT_ADDRESS = "localhost";
        static constexpr const char *ENV_ADDRESS = "PIGPIO_ADDR";
        static constexpr const char *ENV_PORT = "PIGPIO_PORT";
        static constexpr uint32_t UNUSED_PARAMETER = 0;
    };

} // namespace pigpio_remote
#endif // PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_PICONNECTION_H