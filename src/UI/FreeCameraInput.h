#ifndef FREE_CAMERA_INPUT_H
#define FREE_CAMERA_INPUT_H 1
#include "../BLZEntity.h"
#include "../BScene.h"

void freeCameraHandleKey(GLFWwindow *window, int key, int scancode, int action, int mods, BScene *scene);
void freeCameraHandleClick(GLFWwindow *window, int button, int action, int mods, BScene *scene);
void updateGameCameraLook(BScene *scene, double timeDelta, GLFWwindow *window);
#endif