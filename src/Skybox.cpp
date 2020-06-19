#include "Skybox.h"
#include <vector>
#include "GLSL.h"
#include <stb_image.h>
#include <iostream>
std::map<std::string, std::shared_ptr<Skybox>> skyboxes;
const std::vector<std::string> faces {
    "vc_rt.jpg",
    "vc_lf.jpg",
    "vc_up.jpg",
    "vc_dn.jpg",
    "vc_ft.jpg",
    "vc_bk.jpg"
};

Skybox::Skybox() {
    
}


void Skybox::init (std::string dir) {
    unit = texUnit++;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false);
    for(GLuint i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load((dir+faces[i]).c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i ,0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else {
            std::cout << "failed to load: " << (dir+faces[i]).c_str() << std::endl;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}