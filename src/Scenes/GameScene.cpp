#include "GameScene.h"


double lastUpdate = 0;
void UpdateGameScene(BScene *scene, WindowManager *window) {
    double timeDelta = glfwGetTime() - lastUpdate;
    lastUpdate += timeDelta;
    
    UpdateScenePhysics(scene,(float) timeDelta);
    
    CheckPhysicsCollisions(scene, timeDelta); // After UpdateScenePhysics
}

void InitGameScene(BScene *scene, int map) {
    lastUpdate = glfwGetTime();
    scene->camera = new Camera();
    scene->lightLocation = vec3(-85, 80, 0);
   

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


    scene->skybox = skyboxes["neutral"];
}
