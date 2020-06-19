#pragma once
#ifndef MATERIAL_H_GUARD
#define MATERIAL_H_GUARD 1
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <map>

class Material {
    public:
        Material();
        float shine;
        glm::vec3 Ambient;
        glm::vec3 Diffuse;
        glm::vec3 Specular;

};

extern std::map<std::string, std::shared_ptr<Material>> materials;

#endif