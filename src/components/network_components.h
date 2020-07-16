#ifndef NETWORK_COMPONENTS_H
#define NETWORK_COMPONENTS_H 1

#include "../_components.h"
#include "../network.h"
#include <queue>
#include <vector>

// Singleton
typedef struct _network_connections : component_t {
    bool shouldTerminate = false;
    std::vector<port_t> listening_port;
} c_network_connections_t;
#define COMPONENT_NETWORK_CONNECTIONS (std::type_index(typeid(c_network_connections_t)))


//Singleton
typedef struct _network_queue : component_t {
    std::queue<network_packet_t> in;
    std::queue<network_packet_t> out;
} c_network_queue_t;
#define COMPONENT_NETWORK_QUEUE (std::type_index(typeid(c_network_queue_t)))

#endif