#ifndef COMPONENTS_H
#define COMPONENTS_H 1
#include <typeindex>
#include <thread>
#include <typeinfo>
#include <glm/gtc/type_ptr.hpp>
#include <stdint.h>
#include <map>
#include <memory>
#include <queue>
#include "_components.h"
class BLZEntity;
class BScene;
typedef struct _location_obj : component_t {
    glm::vec3 position = glm::vec3(0,0,0);
    glm::vec3 rotationAmount = glm::vec3(0,0,0);
    glm::vec3 size = glm::vec3(1,1,1);
} c_location_t;
#define COMPONENT_LOCATION (std::type_index(typeid(c_location_t)))

typedef void (*collisionFunction)(std::shared_ptr<BLZEntity> self, std::shared_ptr<BLZEntity> other, BScene *scene, float timeDelta);
typedef struct _collider_obj : component_t {
    // Relative to position
    glm::vec3 lowerBound = glm::vec3(0, 0, 0);
    glm::vec3 upperBound = glm::vec3(0, 0, 0);
    collisionFunction collisionReaction = nullptr;
} c_collider_t;
#define COMPONENT_COLLIDER (std::type_index(typeid(c_collider_t)))

#include "components/render_components.h"

typedef struct _physics_obj : component_t {
    glm::vec3 velocity = glm::vec3(0,0,0);
    float mass = 1;
} c_physics_t;
#define COMPONENT_PHYSICS (std::type_index(typeid(c_physics_t)))



typedef struct _singleton_obj : component_t {
} c_singleton_t;
#define COMPONENT_SINGLETON (std::type_index(typeid(c_singleton_t)))

#include "components/network_components.h"

#endif // Component header guard
