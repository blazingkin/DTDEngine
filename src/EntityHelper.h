#ifndef B_ENTITY_HELPER_H
#define B_ENTITY_HELPER_H 1

#include "components.h"
#include "BLZEntity.h"
#include <vector>

void resize(std::shared_ptr<BLZEntity> e, glm::vec3 newSize, BScene *scene);
void move(std::shared_ptr<BLZEntity> e, glm::vec3 newPosition, BScene *scene);
void setRotationTo(std::shared_ptr<BLZEntity> e, glm::vec3 newRotation, BScene *scene);
void moveRelative(std::shared_ptr<BLZEntity> e, glm::vec3 newPosition, BScene *scene);
void rotateRelative(std::shared_ptr<BLZEntity> e, glm::vec3 newRotation, BScene *scene);
#endif