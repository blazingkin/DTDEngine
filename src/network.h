#ifndef NETWORK_H
#define NETWORK_H 1
#include <stdint.h>

/* Workaround for syntax highlighting in VSCode */
#ifdef __linux__
#undef _WIN32
#endif

#ifdef _WIN32
#include <winsock2.h>
typedef SOCKET socket_t;
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define INVALID_SOCKET -1
typedef int socket_t;
#endif /* ifdef _WIN32 */

typedef uint32_t port_t;

typedef struct network_packet_data {
    char *bytes;
    uint32_t size;
} network_packet_data_t;

typedef struct network_host {
    struct sockaddr_in6 remote;
    port_t port;
} network_host_t;

typedef struct network_packet {
    network_host_t *host;
    network_packet_data_t data;
} network_packet_t;

#endif