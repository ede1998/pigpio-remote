#ifndef PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_PICONNECTION_H
#define PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_PICONNECTION_H

#include "../../src/PiConnectionInternals.h"
#include "../../src/communication/Command.h"
#include "../../src/platform/NoNagleSyncClient.h"
#include "PigpioError.h"
#include <array>
#include <string>
#include <vector>

namespace pigpio_remote
{
    using platform::ConnectionError;

    /**
     * @brief Connection to a Pigpio daemon running on some Raspberry Pi.
     * 
     * Depending on the compile flag, this connection uses either
     * - POSIX sockets
     * - the Arduino library ESPAsyncTCP
     * to open a TCP connection and send data.
     * 
     * As of now, this connection is only synchronous and does not support multi-threading.
     */
    class PiConnection
    {
    private:
        platform::NoNagleSyncClient _client;

    public:
        /**
         * @brief Destroy the Pi Connection object and close an open connection.
         */
        ~PiConnection();

        /**
          * @name Connection methods
          * @brief This set of methods open a connection to the Pigpio daemon.
          * 
          * All the parameters are optional. If they are omitted, the library finds its own values.
          * If defined, it reads the environment variables `PiConnection::ENV_ADDRESS = PIGPIO_ADDR`
          * and `PiConnection::ENV_PORT = PIGPIO_PORT`. If this doesn't help, the default values
          * `PiConnection::DEFAULT_PORT = 8888` and `PiConnection::DEFAULT_ADDRESS = localhost` are
          * used.
          * 
          * If the library is compiled with \p PIGPIO_REMOTE_PLATFORM_POSIX , the parameter \p addr
          * may be either an IP address or a DNS name. Both will work. If the library is compiled
          * with \p PIGPIO_REMOTE_PLATFORM_ARDUINO , it might be required to specify an IP address
          * (and not a DNS name).
          * 
          * @param addr The IP adress to connect to.
          * @param port Port to connect to.
          * @return `ConnectionError::SUCCESS` connection sucessfully established
          * @return otherwise error code why connection could not be created.
          */
        ///@{
        ConnectionError Connect(uint16_t port = 0);
        ConnectionError Connect(const std::string &addr, uint16_t port = 0);
        ConnectionError Connect(const char *addr, uint16_t port = 0);
        ///@}

        /**
         * @brief Checks if a connection has been successfully established.
         * 
         * @return true Connection established.
         * @return false No connection.
         */
        bool Connected() const;

        /**
         * @brief Closes the connection if any is open.
         */
        void Stop();

        /**
         * @brief Internal library method.
         * 
         * This method sends a basic command to the Pigpio daemon if connected. It should not be used directly.
         * 
         * @param command command to send
         * @param parameter1 parameter 1 of the command
         * @param parameter2 optional second parameter
         * @return Result of the command, either an error or an int representing the received value.
         */
        internal::CommandResult SendCommand(communication::Command command, uint32_t parameter1, uint32_t parameter2 = UNUSED_PARAMETER);

        /**
         * @brief Internal library method.
         * 
         * This method sends an extended command to the Pigpio daemon if connected. It should not be used directly.
         * 
         * @param command command to send
         * @param parameters basic parameters to the command
         * @param data arbitrary data buffer to accompany the command
         * @return internal::CommandResult result of the command, either an error or an int representing the received value.
         */
        internal::CommandResult SendCommand(communication::Command command, const std::array<uint32_t, 3> &parameters, const std::vector<uint8_t> &data);

        /**
         * @brief Internal library method.
         * 
         * Receives \p receive_bytes over the connection and returns at most the first \p return_bytes of them.
         * The remaining bytes are discarded.
         * 
         * @param receive_bytes number of bytes to receive
         * @param return_bytes number of bytes to return
         * @return std::vector<uint8_t> the data that was received (`return_bytes` at most)
         */
        std::vector<uint8_t> ReceiveExtraData(uint32_t receive_bytes, uint32_t return_bytes);

        /**
         * @brief Get the Connection object
         * 
         * @return platform::NoNagleSyncClient Returns the interInternal class that abstract 
         */
        platform::NoNagleSyncClient GetConnection();

        /**
          * @name Connection target fallbacks
          * @brief Defines the fallbacks for opening a connection if nothing else was specified.
          * 
          * The values given in the environment variables is preferred. If not given, the constant's
          * value is used.
          */
        ///@{
        static constexpr uint16_t DEFAULT_PORT = 8888;
        static constexpr const char *DEFAULT_ADDRESS = "localhost";
        static constexpr const char *ENV_ADDRESS = "PIGPIO_ADDR";
        static constexpr const char *ENV_PORT = "PIGPIO_PORT";
        ///@}

        static constexpr uint32_t UNUSED_PARAMETER = 0;
    };

} // namespace pigpio_remote

#endif // PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_PICONNECTION_H