#ifndef _INSTANCE_DATA_H
#define _INSTANCE_DATA_H

#include <map>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

typedef struct {
    std::map<std::string, std::shared_ptr<std::vector<glm::vec4>>> attributes;
    std::vector<glm::mat4> M;
} instance_render_data_t;
#endif