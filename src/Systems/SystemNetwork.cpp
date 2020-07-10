#include "SystemNetwork.h"
#include "../BLZEntity.h"

std::mutex readingPacketIn;

void sendPacket(network_packet_t packet, socket_t sock) {
    if (sendto(sock, packet.data.bytes, packet.data.size, 0, (struct sockaddr *) (&packet.host->remote), sizeof(struct sockaddr_in6)) < 0) {
        perror("Send to");
    }
}

// This function is run in a separate thread, one per scene
void manageNetworkQueues(BScene *scene) {
    
    // Start a thread for each port that we want to listen on
    socket_t sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Unable to spawn socket" << std::endl;
        exit(1);
    }

    while (scene->singleton()->hasComponent<c_network_connections_t>() &&
           scene->singleton()->hasComponent<c_network_queue_t>() &&
           !scene->singleton()->getComponent<c_network_connections_t>()->shouldTerminate) {
          // Run as long as there is still a requested network connection

        auto network_queue = scene->singleton()->getComponent<c_network_queue_t>();
        while (!network_queue->out.empty()) {
            // While we have a packet to send, send it
            sendPacket(network_queue->out.front(), sock);
            network_queue->out.pop();
        }


        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // Clean up after ourselves before quitting
}

void SetupNetworkConnection(BScene *scene) {
    auto connections = c_network_connections_t{};
    auto queue = c_network_queue_t{};
    // We set up the singleton components before we spawn the worker thread so that it has access to them
    scene->singleton()->addComponent<c_network_connections_t>(scene, connections);
    scene->singleton()->addComponent<c_network_queue_t>(scene, queue);

    // Spawn the network monitor thread
    auto manager = std::thread(manageNetworkQueues, scene);
    manager.detach(); // Keep the thread running in the background
}

void CloseNetworkConnections(BScene *scene) {
    if (scene->singleton()->hasComponent<c_network_connections_t>()) {
        auto connections = scene->singleton()->getComponent<c_network_connections_t>();
        connections->shouldTerminate = true; // Request that the network thread quit
    }
    if (scene->singleton()->hasComponent<c_network_queue_t>()) {

    }
}
