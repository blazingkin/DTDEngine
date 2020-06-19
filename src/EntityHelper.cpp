#include "EntityHelper.h"

#include "Systems/SystemRender.h"

void updateBoundingBox(std::shared_ptr<BLZEntity> e, BScene *scene) {

    if (e->hasComponent(COMPONENT_LOCATION) && e->hasComponent(COMPONENT_MODEL) && e->hasComponent(COMPONENT_COLLIDER)) {
        auto modelComponent = e->getComponent<c_model_t>();
        auto locationComponent = e->getComponent<c_location_t>();
        auto colliderComponent = e->getComponent<c_collider_t>();
        scene->removeFromGrid(e, *colliderComponent);
        // Generate 8 bounding vertices
        std::vector<glm::vec3> bounds;
        // Bitwise count from lower bound to upper bound
        bounds.push_back(glm::vec3(modelComponent->lowerBound.x, modelComponent->lowerBound.y, modelComponent->lowerBound.z));
        bounds.push_back(glm::vec3(modelComponent->lowerBound.x, modelComponent->lowerBound.y, modelComponent->upperBound.z));
        bounds.push_back(glm::vec3(modelComponent->lowerBound.x, modelComponent->upperBound.y, modelComponent->lowerBound.z));
        bounds.push_back(glm::vec3(modelComponent->lowerBound.x, modelComponent->upperBound.y, modelComponent->upperBound.z));
        bounds.push_back(glm::vec3(modelComponent->upperBound.x, modelComponent->lowerBound.y, modelComponent->lowerBound.z));
        bounds.push_back(glm::vec3(modelComponent->upperBound.x, modelComponent->lowerBound.y, modelComponent->upperBound.z));
        bounds.push_back(glm::vec3(modelComponent->upperBound.x, modelComponent->upperBound.y, modelComponent->lowerBound.z));
        bounds.push_back(glm::vec3(modelComponent->upperBound.x, modelComponent->upperBound.y, modelComponent->upperBound.z));

        // Generate rotation matrix
        // https://en.wikipedia.org/wiki/Rotation_matrix#General_rotations
        auto alpha = locationComponent->rotationAmount.x;
        auto beta = locationComponent->rotationAmount.y;
        auto gamma = locationComponent->rotationAmount.z;


        auto rotationMatrix = glm::mat3(1.f);
        rotationMatrix[0][0] = cos(alpha) * cos(beta);
        rotationMatrix[0][1] = sin(alpha) * cos(beta);
        rotationMatrix[0][2] = -sin(beta);
        rotationMatrix[1][0] = (cos(alpha) * sin(beta) * sin(gamma)) - (sin(alpha) * cos(gamma));
        rotationMatrix[1][1] = (sin(alpha) * sin(beta) * sin(gamma)) + (cos(alpha) * cos(gamma));
        rotationMatrix[1][2] = cos(beta) * sin(gamma);
        rotationMatrix[2][0] = (cos(alpha) * sin(beta) * cos(gamma)) + (sin(alpha) * sin(gamma));
        rotationMatrix[2][1] = (sin(alpha) * sin(beta) * cos(gamma)) - (cos(alpha) * sin(gamma));
        rotationMatrix[2][2] = cos(beta) * cos(gamma);
        
        // Multiply to get new bounding vertices
        for (int i = 0; i < bounds.size(); i++) {
            bounds[i] = rotationMatrix * (locationComponent->size * bounds[i]);

        }


        // Find new max / min
        glm::vec3 min = glm::vec3(INFINITY, INFINITY, INFINITY);
        glm::vec3 max = glm::vec3(-INFINITY, -INFINITY, -INFINITY);
        for (auto bound : bounds) {
            if (bound.x < min.x) {
                min.x = bound.x;
            }
            if (bound.y < min.y) {
                min.y = bound.y;
            }
            if (bound.z < min.z) {
                min.z = bound.z;
            }
            if (bound.x > max.x) {
                max.x = bound.x;
            }
            if (bound.y > max.y) {
                max.y = bound.y;
            }
            if (bound.z > max.z) {
                max.z = bound.z;
            }
        }
        colliderComponent->lowerBound = min + locationComponent->position;
        colliderComponent->upperBound = max + locationComponent->position;
        min = colliderComponent->lowerBound;
        max = colliderComponent->upperBound;
        
        scene->addToGrid(e, *colliderComponent);

    }
}
void resize(std::shared_ptr<BLZEntity> e, glm::vec3 newSize, BScene *scene) {
    if (e->hasComponent(COMPONENT_LOCATION)) {
        auto location = e->getComponent<c_location_t>();
        location->size = newSize;
        if (e->hasComponent(COMPONENT_MODEL)) {
            auto modelComponent = e->getComponent<c_model_t>();
            modelComponent->radius = calculateRadius(modelComponent, location);
        }
        updateBoundingBox(e, scene);
    }
}


void move(std::shared_ptr<BLZEntity> e, glm::vec3 newPosition, BScene *scene) {
    if (e->hasComponent(COMPONENT_LOCATION)) {
        auto location = e->getComponent<c_location_t>();
        int wallCollision = scene->posInWorld(newPosition);
        if (wallCollision == INSIDE_GRID){
            location->position = newPosition;
        } else if (e->hasComponent<c_physics_t>()) {
            auto physics = e->getComponent<c_physics_t>();
            if (wallCollision == OUTSIDE_X_BOUNDS) {
                physics->velocity = glm::vec3(-physics->velocity.x, 0, physics->velocity.z);
            } else if (wallCollision == OUTSIDE_Z_BOUNDS) {
                physics->velocity = glm::vec3(physics->velocity.x, 0, -physics->velocity.z);
            } else {
                physics->velocity = -physics->velocity;
            }
        }
        updateBoundingBox(e, scene);
    }
}

void moveRelative(std::shared_ptr<BLZEntity> e, glm::vec3 newPosition, BScene *scene) {
    if (e->hasComponent(COMPONENT_LOCATION)) {
        auto location = e->getComponent<c_location_t>();
        move(e, location->position + newPosition, scene);
        // location->position += newPosition;
        // updateBoundingBox(e, scene);
    }
}

void setRotationTo(std::shared_ptr<BLZEntity> e, glm::vec3 newRotation, BScene *scene) {
    if (e->hasComponent(COMPONENT_LOCATION)) {
        auto location = e->getComponent<c_location_t>();
        location->rotationAmount = newRotation;
        updateBoundingBox(e, scene);
    }
}

void rotateRelative(std::shared_ptr<BLZEntity> e, glm::vec3 newRotation, BScene *scene) {
    if (e->hasComponent<c_location_t>()) {
        auto location = e->getComponent<c_location_t>();
        setRotationTo(e, location->rotationAmount + newRotation, scene);
    }
}
