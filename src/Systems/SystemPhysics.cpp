#include "SystemPhysics.h"

void UpdateScenePhysics(BScene *scene, float timeDelta) {
	auto entities = scene->getEntitiesWithComponents({ COMPONENT_LOCATION, COMPONENT_PHYSICS});
	
	for (auto entity : entities) {
		auto entityPosition = entity->getComponent<c_location_t>();
		auto entityPhysics = entity->getComponent<c_physics_t>();
		move(entity, entityPosition->position + (entityPhysics->velocity * (float)timeDelta), scene);
	}
}



void CheckPhysicsCollisionsHelper(std::vector<std::shared_ptr<BLZEntity>> entities, int start, int end, BScene *scene, float timeDelta){

    for (size_t i = start; i < end && i < entities.size(); i++) {
        auto entityA = entities[i];
        auto BoxA = entityA->getComponent<c_collider_t>();
        // Get entities in same and neighbor occupancy cells
        for (auto entityB : scene->getEntitiesCloseToOrWithinExtents(*BoxA)) {
            if (!entityB->hasComponent<c_collider_t>()) {
                continue;
            }
            /*if(entityB->hasComponent<c_virus_t>() && entityA->hasComponent<c_virus_t>()){
                continue;
            }*/
            auto BoxB = entityB->getComponent<c_collider_t>();
            if(entityA->tag != entityB->tag) {
                if(intersect(BoxA, BoxB)){
                    // Only apply the collision result on BoxA
                    // we will detect that BoxB is detected when we check it
                    // "Trigger"
                    if (BoxA->collisionReaction != nullptr) {
                        BoxA->collisionReaction(entityA, entityB, scene, timeDelta);
                    }

                    auto selflocationComponent = entityA->getComponent<c_location_t>();
                    auto otherLocationComponent = entityB->getComponent<c_location_t>();

                    glm::vec3 A_to_B = otherLocationComponent->position - selflocationComponent->position;
                    // "Rebound" from the collision
                    

                    if (entityA->hasComponent<c_physics_t>()) {
                        auto physicsComponent = entityA->getComponent<c_physics_t>();
                        glm::vec3 tempVel = physicsComponent->velocity;
                        //if two viruses are colliding, point velocity away from other virus
                        
                        float randAngle = (rand() % 90) - 45;
                        glm::mat4 rotationMat(1);
                        rotationMat = glm::rotate(rotationMat, randAngle, glm::vec3(0.0, 1.0, 0.0));
                        tempVel = glm::vec3(rotationMat * glm::vec4(tempVel, 1.0));

                        if (glm::dot(A_to_B, tempVel) > glm::dot(A_to_B, (-1.0f * tempVel))){
                            tempVel = -1.0f * tempVel;
                        }

                        physicsComponent->velocity = tempVel;
                        
                        // if (entityB->hasComponent<c_physics_t>()) {
                        //     // Calculate an elastic collision
                        //     auto otherPhysicsComponent = entityB->getComponent<c_physics_t>();
                        //     auto newVelocity = ((physicsComponent->mass - otherPhysicsComponent->mass) * physicsComponent->mass) +
                        //                         ((2 * otherPhysicsComponent->mass) * otherPhysicsComponent->velocity);
                        //     newVelocity /= physicsComponent->mass + otherPhysicsComponent->mass;
                        //     newVelocities.push_back(newVelocity);
                        //     toRebound.push_back(entityA);
                        // } else {
                        //     // Entity b does not have a mass, treat it as if it has infinite mass
                        //     newVelocities.push_back(-physicsComponent->velocity);
                        //     toRebound.push_back(entityA);
                        // }
                    }
                }
            }
        }
    }
}

void CheckPhysicsCollisions(BScene *scene, float timeDelta) {
	auto colliders = scene->getEntitiesWithComponents({ COMPONENT_COLLIDER });
    auto len = colliders.size() / 4;
    std::thread first (CheckPhysicsCollisionsHelper, colliders, 0, len, scene, timeDelta);
    std::thread second (CheckPhysicsCollisionsHelper, colliders, len + 1, len * 2, scene, timeDelta);
    std::thread third (CheckPhysicsCollisionsHelper, colliders, (len * 2) + 1, len * 3, scene, timeDelta);
    std::thread fourth (CheckPhysicsCollisionsHelper, colliders, (len * 3) + 1, colliders.size(), scene, timeDelta);
	first.join();
    second.join();
    third.join();
    fourth.join();
}


bool intersect(c_collider_t *a, c_collider_t *b) {
  // Check for AABB vs. AABB collisions
  return (a->lowerBound.x <= b->upperBound.x && a->upperBound.x >= b->lowerBound.x) &&
         (a->lowerBound.y <= b->upperBound.y && a->upperBound.y >= b->lowerBound.y) &&
         (a->lowerBound.z <= b->upperBound.z && a->upperBound.z >= b->lowerBound.z);
}


