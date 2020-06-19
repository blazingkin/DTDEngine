#include "Material.h"
#include <string>

std::map<std::string, std::shared_ptr<Material>> materials;
Material::Material() {
    shine = 1;
    Diffuse = glm::vec3(0.3, 0.3, 0.3);
    Ambient = glm::vec3(0.1, 0.1, 0.1);
    Specular = glm::vec3(0.3, 0.3, 0.3);
}

void addMaterial(std::string s, Material m) {
    materials[s] = std::make_shared<Material>(m);
} 