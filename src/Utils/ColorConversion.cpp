#include "ColorConversion.h"

glm::vec3 RGBToVec3(int r, int g, int b) {
    return glm::vec3(r / 255.0, g / 255.0, b / 255.0);
}

glm::vec3 RGBToVec3(int rgb) {
    return glm::vec3(((rgb >> 16) & 0xFF) / 255.0, ((rgb >> 8) & 0xFF), ((rgb) & 0xFF));
}