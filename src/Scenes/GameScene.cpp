#include "GameScene.h"
#include <gethostbyname/gethostbyname.h>
#include "../UI/FreeCameraInput.h"
std::string data = "ABCDEFGH";

double lastUpdate = 0;
void UpdateGameScene(BScene *scene, WindowManager *window) {
    double timeDelta = glfwGetTime() - lastUpdate;
    lastUpdate += timeDelta;
    
    UpdateScenePhysics(scene,(float) timeDelta);
    
    CheckPhysicsCollisions(scene, timeDelta); // After UpdateScenePhysics
    updateGameCameraLook(scene, timeDelta, window->getHandle());
    /*
    auto network_packet = network_packet_t{};
    network_packet.data.size = data.size();
    network_packet.data.bytes = (char *) data.c_str();
    network_packet.host = new network_host_t{};
    gethostbyname6("localhost", &network_packet.host->remote);
    network_packet.host->port = 3333;
    network_packet.host->remote.sin6_port = ntohs(3333);
    network_packet.host->remote.sin6_family = AF_INET6;
    scene->singleton()->getComponent<c_network_queue_t>()->out.push(network_packet);
    */
}

void InitGameScene(BScene *scene, int map) {
    lastUpdate = glfwGetTime();
    scene->camera = new Camera();
    scene->lightLocation = vec3(-85, 80, 0);
    scene->skybox = skyboxes["neutral"];
    SetupNetworkConnection(scene);
    auto floor = BLZEntity::newEntity(meshes["cube.obj"], scene);
    // Todo call the resize function
    float bound = 500;
    auto floorRenderableComponent = floor->getComponent<c_render_t>();
        floorRenderableComponent->material = materials["floor"];
    auto floorLocationComponent = floor->getComponent<c_location_t>();
        floorLocationComponent->position = vec3(0, -0.31, 0);
    if (floor->hasComponent<c_collider_t>()) {
        floor->removeComponent<c_collider_t>(scene);
    }
    resize(floor,vec3(bound, 0.7, bound),scene);
    setRotationTo(floor, vec3(0, M_PI/2, M_PI), scene);
    

    scene->update = UpdateGameScene;
    scene->handleKey = freeCameraHandleKey;
    scene->handleClick = freeCameraHandleClick;
    scene->skybox = skyboxes["something"];

}
