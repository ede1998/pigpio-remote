#ifndef PIGPIO_REMOTE_SRC_PLATFORM_NONAGLESYNCCLIENT_NONAGLESYNCCLIENTPOSIX_H
#define PIGPIO_REMOTE_SRC_PLATFORM_NONAGLESYNCCLIENT_NONAGLESYNCCLIENTPOSIX_H

#ifdef PIGPIO_REMOTE_PLATFORM_POSIX

#include "BaseNoNagleSyncClient.h"
#include <arpa/inet.h>
#include <cassert>
#include <cstring>
#include <netdb.h>
#include <netinet/tcp.h>
#include <string>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

namespace pigpio_remote
{

    namespace platform
    {

        class NoNagleSyncClient : public BaseNoNagleSyncClient<NoNagleSyncClient>
        {
        private:
            friend class BaseNoNagleSyncClient;
            int _socket = -1;

            ConnectionError InternalConnect(const char *ip, uint16_t port)
            {
                int sock, err, opt;
                struct addrinfo hints, *res, *rp;

                std::memset(&hints, 0, sizeof(hints));

                hints.ai_family = PF_UNSPEC;
                hints.ai_socktype = SOCK_STREAM;
                hints.ai_flags |= AI_CANONNAME;

                err = getaddrinfo(ip, std::to_string(port).c_str(), &hints, &res);

                if (err == 0)
                {
                    return ConnectionError::INVALID_SERVER;
                }

                for (rp = res; rp != nullptr; rp = rp->ai_next)
                {
                    sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

                    if (sock == -1)
                    {
                        continue;
                    }

                    /* Disable the Nagle algorithm. */
                    opt = 1;
                    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char *>(&opt), sizeof(int));

                    if (::connect(sock, rp->ai_addr, rp->ai_addrlen) != -1)
                    {
                        break;
                    }
                }

                freeaddrinfo(res);

                if (rp == nullptr)
                {
                    return ConnectionError::INVALID_RESPONSE;
                }

                this->_socket = sock;

                return ConnectionError::SUCCESS;
            }

            inline bool InternalConnected() const
            {
                return this->_socket >= 0;
            }

            static inline int InternalAvailable()
            {
                assert(false); // Not implemented.
                return -1;
            }

            inline int InternalRead(uint8_t *data, size_t len) const
            {
                return recv(this->_socket, data, len, MSG_WAITALL);
            }

            inline size_t InternalWrite(const uint8_t *data, size_t len) const
            {
                return send(this->_socket, data, len, 0);
            }

            inline void InternalStop()
            {
                close(this->_socket);
                this->_socket = -1;
            }

        };

    } // namespace platform

} // namespace pigpio_remote

#endif // PIGPIO_REMOTE_PLATFORM_POSIX

#endif // PIGPIO_REMOTE_SRC_PLATFORM_NONAGLESYNCCLIENT_NONAGLESYNCCLIENTPOSIX_H