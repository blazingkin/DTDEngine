#ifndef SYSTEM_PHYSICS_H
#define SYSTEM_PHYSICS_H
#include "../BScene.h"
#include "../EntityHelper.h"
#include "../Particles.h"
#include <thread>
#include <mutex>

void UpdateScenePhysics(BScene *scene, float timeDelta);
void CheckPhysicsCollisions(BScene *scene, float timeDelta);
void CheckPhysicsCollisionsHelper(std::vector<std::shared_ptr<BLZEntity>> entities, int start, int end, BScene *scene, float timeDelta);
bool intersect(c_collider_t *a, c_collider_t *b);

#endif