
#ifndef RENDER_COMPONENTS_H
#define RENDER_COMPONENTS_H

#include "../_components.h"
#include <memory>
#include <vector>
#include <map>
#include "../RenderableWithPosition.h"
#include "../Texture.h"
#include "../Material.h"
#include "../Shape.h"
#include "../Program.h"
#include "../BScene.h"
typedef std::shared_ptr<std::vector<Shape>> model_t;

typedef struct _model_obj : component_t {
    glm::vec3 lowerBound = glm::vec3(0, 0, 0);
    glm::vec3 upperBound = glm::vec3(0, 0, 0);
    model_t model = nullptr;
    float radius = 1;
} c_model_t;
#define COMPONENT_MODEL (std::type_index(typeid(c_model_t)))

typedef struct _renderable_obj : component_t {
    std::shared_ptr<Material> material = std::make_shared<Material>();
    std::shared_ptr<Texture> tex = nullptr;
    std::shared_ptr<Texture> normalMap = nullptr;
    std::shared_ptr<Program> program = nullptr;
    std::map<std::string, glm::vec4> shaderUniforms;
    bool drawShadow = false;
    std::shared_ptr<Texture> bump = nullptr;
    std::vector<std::shared_ptr<RenderableWithPosition>> children = {};
} c_render_t;
#define COMPONENT_RENDERABLE (std::type_index(typeid(c_render_t)))

#endif