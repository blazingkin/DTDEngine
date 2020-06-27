#include "SystemNetworkInitialization.h"
#include "../BLZEntity.h"
// This function is run in a separate thread, one per scene
void manageNetworkQueues(BScene *scene) {
    
    while (scene->singleton()->hasComponent<c_network_connections_t>() &&
           !scene->singleton()->getComponent<c_network_connections_t>()->shouldTerminate) {
          // Run as long as there is still a requested network connection
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
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