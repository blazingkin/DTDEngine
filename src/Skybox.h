#pragma once
#ifndef SKYBOX_H_GUARD
#define SKYBOX_H_GUARD 1
#include <string>
#include <map>
#include <memory>
#include "Texture.h"

class Skybox {

    public:
        Skybox();
        void init(std::string dir);
        unsigned int textureID;
        int unit;
};

extern std::map<std::string, std::shared_ptr<Skybox>> skyboxes;

#endif