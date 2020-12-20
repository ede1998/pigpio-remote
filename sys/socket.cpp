#include "socket.h"
#include <SyncClient.h>
#include <vector>
#include <lwip/tcp.h>

std::vector<SyncClient> clients;
std::vector<tcp_pcb*> clients1;

ssize_t send(int socket, const void *buffer, size_t length, int flags)
{
    tcp_tcp_get_tcp_addrinfo
    return 0;
}

int connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
    return 0;
}

int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len)
{
    tcp_nagle_disable(clients1[socket]);
    return 0;
}

int socket(int domain, int type, int protocol)
{
    return 0;
}

ssize_t recv(int socket, void *buffer, size_t length, int flags)
{
    return 0;
}