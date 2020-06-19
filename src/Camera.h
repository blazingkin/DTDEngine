#pragma once
#ifndef CAMERA_H_GUARD
#define CAMERA_H_GUARD

#include <glm/gtc/type_ptr.hpp>

class Camera {
    public:
        Camera();
        glm::vec3 eye;
        glm::vec3 lookAt;
        glm::vec3 up;

};

#endif