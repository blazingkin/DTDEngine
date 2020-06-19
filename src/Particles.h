#ifndef PARTICLES_H
#define PARTICLES_H
#include "Program.h"
#include <vector>
#include <memory>
#include "BScene.h"
#include "EntityHelper.h"
#include "Camera.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "_instance_data.h"
#include <mutex>

using namespace glm;
using namespace std;

typedef struct {
    vector<vec4> startingPosition;
    vector<vec4> velocity;
    vector<vec4> acceleration;
    vector<vec4> size;
    vector<float> life;
    vector<vec4> animationFrame;
    vector<vec4> lifeStart;
} particle_state_t;

void setupParticleSystem();
void updateParticles(float deltaTime);
void renderParticles(BScene *scene, int width, int height);
void addParticle(vec3 initialPosition, vec3 velocity, vec3 acceleration, float size, float atlasPosition, float lifetime);
void renderInfectionStatus(BScene *scene);
void resetParticles();

#endif // LAB471_PARTICLE_H_INCLUDED
