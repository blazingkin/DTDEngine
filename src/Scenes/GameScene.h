#ifndef GAME_SCENE_H
#define GAME_SCENE_H 1

#include "../BScene.h"
#include "../WindowManager.h"
#include "../BLZEntity.h"
#include "../Systems/SystemPhysics.h"
#include "../Particles.h"
#include "../Audio.h"
#include "../Systems/SystemNetwork.h"


void UpdateGameScene(BScene *scene, WindowManager *window);
void InitGameScene(BScene *scene, int map);
void InitGameShaders();
#endif