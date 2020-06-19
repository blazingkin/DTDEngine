
#ifndef AUDIO_H_
#define AUDIO_H_
#include <irrKlang.h>
#include <string>
#include <glm/glm.hpp>

int initAudio(std::string resourceDirectory);
void play2DSound(std::string sound);
void startMusic(std::string sound);
void stopMusic();
void play3DSound(std::string sound, glm::vec3 location);
extern irrklang::ISoundEngine *soundEngine;
#endif