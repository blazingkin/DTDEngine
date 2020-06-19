#include "Audio.h"
using namespace irrklang;
ISoundEngine *soundEngine;
std::string audioDirectory;
ISound *background_music = nullptr;
float musicVolume = 0.5f;

void stopMusic() {
    if (background_music != nullptr) {
        background_music->stop();
        background_music->drop();
    }
    background_music = nullptr;
}

void startMusic(std::string sound) {
    if (background_music != nullptr) {
        stopMusic();
    }
    background_music = soundEngine->play2D((audioDirectory + "/" + sound).c_str(), true, false, true);
    background_music->setVolume(0.5f);
}

void play2DSound(std::string sound) {
    soundEngine->play2D((audioDirectory + "/" + sound).c_str());
}

void play3DSound(std::string sound, glm::vec3 location) {
    soundEngine->play3D((audioDirectory + "/" + sound).c_str(), vec3df(location.x, location.y, location.z));
}

int initAudio(std::string resourceDirectory) {
    soundEngine = createIrrKlangDevice();
    if (!soundEngine) {
        return -1;
    }
    audioDirectory = resourceDirectory + "/" + "audio";
    return 0;
}
