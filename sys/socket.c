#include "socket.h"

ssize_t send(int socket, const void *buffer, size_t length, int flags)
{
    return 0;
}

int connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
    return 0;
}

int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len)
{
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