#include "Camera.h"



Camera::Camera() {
    lookAt = glm::vec3(0, 0, 1);
    up = glm::vec3(0, 1, 0);
    eye = glm::vec3(0,2,0);
} 
