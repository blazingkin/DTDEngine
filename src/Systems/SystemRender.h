#ifndef SYSTEM_RENDER_H
#define SYSTEM_RENDER_H 1

#define GLM_ENABLE_EXPERIMENTAL

#include <memory>
#include "../MatrixStack.h"
#include "../GLSL.h"
#include "../Program.h"
#include "../BScene.h"
#include "../Camera.h"
#include <vector>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "../components.h"
#include "../BLZEntity.h"
#include "../EntityHelper.h"
#include "SystemShadows.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <thread>
#include "../_instance_data.h"
#include "../Particles.h"
#include "../Utils/FieldOfViewHelpers.h"

void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>M);
void printVec3(glm::vec3 v);
GLuint RenderScene(BScene *scene, int width, int height);
void makeViewFrust(glm::mat4 P, glm::mat4 V);
float DistToPlane(float A, float B, float C, float D, glm::vec3 point);
bool ViewFrustCull(glm::vec3 center, float radius);
void setupInstanceRenderData(std::vector<std::shared_ptr<BLZEntity>> *entities, instance_render_data_t *result);
void RenderInstances(instance_render_data_t *data, std::shared_ptr<Program> program, std::shared_ptr<BLZEntity> entity, std::shared_ptr<MatrixStack> Projection, std::shared_ptr<MatrixStack> View, BScene *scene);
float calculateRadius(c_model_t *model_comp, c_location_t *location_comp);
void initSystemRender();

#endif