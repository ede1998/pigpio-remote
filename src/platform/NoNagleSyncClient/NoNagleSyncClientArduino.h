#ifndef PIGPIO_REMOTE_SRC_PLATFORM_NONAGLESYNCCLIENT_NONAGLESYNCCLIENTARDUINO_H
#define PIGPIO_REMOTE_SRC_PLATFORM_NONAGLESYNCCLIENT_NONAGLESYNCCLIENTARDUINO_H

#ifdef PIGPIO_REMOTE_PLATFORM_ARDUINO

#include "ESPAsyncTCP.h"
#include "SyncClient.h"
#include "Arduino.h"
#include "BaseNoNagleSyncClient.h"
#include <chrono>

namespace pigpio_remote
{

    namespace platform
    {

        class NoNagleSyncClient : public BaseNoNagleSyncClient<NoNagleSyncClient>
        {
        public:
            static constexpr int DEFAULT_SOCKET_TIMEOUT_SECONDS = 5;
            NoNagleSyncClient()
            {
                this->_client.setTimeout(DEFAULT_SOCKET_TIMEOUT_SECONDS);
            }

        private:
            friend class BaseNoNagleSyncClient;
            SyncClient _client;
            using int_milliseconds_t = std::chrono::duration<int, std::milli>;
            int_milliseconds_t _timeout_milliseconds = int_milliseconds_t(DEFAULT_SOCKET_TIMEOUT_SECONDS * 1000);

            inline ConnectionError InternalConnect(const char *ip, uint16_t port)
            {
                auto result = this->_client.connect(ip, port);
                return static_cast<ConnectionError>(result == 1 ? 0 : result);
            }

            inline bool InternalConnected() const
            {
                return const_cast<SyncClient &>(this->_client).connected();
            }
            inline int InternalAvailable() const
            {
                return const_cast<SyncClient &>(this->_client).available();
            }

            inline int InternalRead(uint8_t *data, size_t len)
            {
                auto start = std::chrono::steady_clock::now();
                auto elapsed_milliseconds = int_milliseconds_t::zero();
                while ((this->_client.available() < len) && elapsed_milliseconds < this->_timeout_milliseconds)
                    {
                        delay(5);
                        auto current = std::chrono::steady_clock::now();
                        elapsed_milliseconds = std::chrono::duration_cast<int_milliseconds_t>(current - start);
                    }

                return this->_client.read(data, len);
            }

            size_t InternalWrite(const uint8_t *data, size_t len)
            {
                auto result = this->_client.write(data, len);
                if (result != sizeof(data))
                {
                    return result;
                }

                this->_client.flush();
                return result;
            }

            inline void InternalStop()
            {
                this->_client.stop();
            }
        };

    } // namespace platform

} // namespace pigpio_remote

#endif // PIGPIO_REMOTE_PLATFORM_ARDUINO

#endif // PIGPIO_REMOTE_SRC_PLATFORM_NONAGLESYNCCLIENT_NONAGLESYNCCLIENTARDUINO_H
