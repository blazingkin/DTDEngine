#ifndef SYSTEM_APPLY_POST_PROCESING_H
#define SYSTEM_APPLY_POST_PROCESING_H 1
#include "../BScene.h"
#include "../GLSL.h"
#include "../Program.h"

void initializePostprocessing();
void applyPostprocessing(BScene *scene, GLuint tex);
void cleanupPostprocessing();

#endif