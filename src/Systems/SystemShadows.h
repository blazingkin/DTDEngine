#ifndef SYSTEM_SHADOWS_H
#define SYSTEM_SHADOWS_H 1
#include <memory>
#include <functional>
#include "../GLSL.h"
#include "../Program.h"
#include "../BLZEntity.h"
#include <glm/gtc/type_ptr.hpp>

void initShadow();
void RenderDepthMap(std::vector<std::shared_ptr<BLZEntity>> occludedBy, BScene *scene, float map_size);
extern GLuint depthMap;
#endif