#ifndef __SYS_SOCKET_H
#define __SYS_SOCKET_H

#include <stddef.h>
#include <sys/types.h>

typedef unsigned int socklen_t;
/* Types of sockets.  */
enum __socket_type
{
    SOCK_STREAM = 1, /* Sequenced, reliable, connection-based
				   byte streams.  */
#define SOCK_STREAM SOCK_STREAM
    SOCK_DGRAM = 2, /* Connectionless, unreliable datagrams
				   of fixed maximum length.  */
#define SOCK_DGRAM SOCK_DGRAM
    SOCK_RAW = 3, /* Raw protocol interface.  */
#define SOCK_RAW SOCK_RAW
    SOCK_RDM = 4, /* Reliably-delivered messages.  */
#define SOCK_RDM SOCK_RDM
    SOCK_SEQPACKET = 5, /* Sequenced, reliable, connection-based,
				   datagrams of fixed maximum length.  */
#define SOCK_SEQPACKET SOCK_SEQPACKET
    SOCK_DCCP = 6, /* Datagram Congestion Control Protocol.  */
#define SOCK_DCCP SOCK_DCCP
    SOCK_PACKET = 10, /* Linux specific way of getting packets
				   at the dev level.  For writing rarp and
				   other similar things on the user level. */
#define SOCK_PACKET SOCK_PACKET

    /* Flags to be ORed into the type parameter of socket and socketpair and
     used for the flags parameter of paccept.  */

    SOCK_CLOEXEC = 02000000, /* Atomically set close-on-exec flag for the
				   new descriptor(s).  */
#define SOCK_CLOEXEC SOCK_CLOEXEC
    SOCK_NONBLOCK = 00004000 /* Atomically mark descriptor(s) as
				   non-blocking.  */
#define SOCK_NONBLOCK SOCK_NONBLOCK
};

/* Bits in the FLAGS argument to `send', `recv', et al.  */
enum
{
    MSG_OOB = 0x01, /* Process out-of-band data.  */
#define MSG_OOB MSG_OOB
    MSG_PEEK = 0x02, /* Peek at incoming messages.  */
#define MSG_PEEK MSG_PEEK
    MSG_DONTROUTE = 0x04, /* Don't use local routing.  */
#define MSG_DONTROUTE MSG_DONTROUTE
    MSG_CTRUNC = 0x08, /* Control data lost before delivery.  */
#define MSG_CTRUNC MSG_CTRUNC
    MSG_PROXY = 0x10, /* Supply or ask second address.  */
#define MSG_PROXY MSG_PROXY
    MSG_TRUNC = 0x20,
#define MSG_TRUNC MSG_TRUNC
    MSG_DONTWAIT = 0x40, /* Nonblocking IO.  */
#define MSG_DONTWAIT MSG_DONTWAIT
    MSG_EOR = 0x80, /* End of record.  */
#define MSG_EOR MSG_EOR
    MSG_WAITALL = 0x100, /* Wait for a full request.  */
#define MSG_WAITALL MSG_WAITALL
    MSG_FIN = 0x200,
#define MSG_FIN MSG_FIN
    MSG_SYN = 0x400,
#define MSG_SYN MSG_SYN
    MSG_CONFIRM = 0x800, /* Confirm path validity.  */
#define MSG_CONFIRM MSG_CONFIRM
    MSG_RST = 0x1000,
#define MSG_RST MSG_RST
    MSG_ERRQUEUE = 0x2000, /* Fetch message from error queue.  */
#define MSG_ERRQUEUE MSG_ERRQUEUE
    MSG_NOSIGNAL = 0x4000, /* Do not generate SIGPIPE.  */
#define MSG_NOSIGNAL MSG_NOSIGNAL
    MSG_MORE = 0x8000, /* Sender will send more.  */
#define MSG_MORE MSG_MORE
    MSG_WAITFORONE = 0x10000, /* Wait for at least one packet to return.*/
#define MSG_WAITFORONE MSG_WAITFORONE
    MSG_BATCH = 0x40000, /* sendmmsg: more messages coming.  */
#define MSG_BATCH MSG_BATCH
    MSG_ZEROCOPY = 0x4000000, /* Use user data in kernel path.  */
#define MSG_ZEROCOPY MSG_ZEROCOPY
    MSG_FASTOPEN = 0x20000000, /* Send data in TCP SYN.  */
#define MSG_FASTOPEN MSG_FASTOPEN

    MSG_CMSG_CLOEXEC = 0x40000000 /* Set close_on_exit for file
					   descriptor received through
					   SCM_RIGHTS.  */
#define MSG_CMSG_CLOEXEC MSG_CMSG_CLOEXEC
};

/* Structure describing a generic socket address.  */
struct sockaddr
{
    unsigned short int sa_family; /* Common data: address family and length.  */
    char sa_data[14];             /* Address data.  */
};

#define PF_UNSPEC 0 /* Unspecified.  */

ssize_t send(int socket, const void *buffer, size_t length, int flags);
int connect(int socket, const struct sockaddr *address, socklen_t address_len);
int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
int socket(int domain, int type, int protocol);
ssize_t recv(int socket, void *buffer, size_t length, int flags);

#endif // __SYS_SOCKET_H