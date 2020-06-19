#include "AssetHelper.h"


void normalizeModel(std::shared_ptr<std::vector<Shape>> s) {
    glm::vec3 maxExt = glm::vec3(-INFINITY, -INFINITY, -INFINITY);
        glm::vec3 minExt = glm::vec3(INFINITY, INFINITY, INFINITY);
        for (auto shp : *s) {
            if (shp.min.x < minExt.x) {
                minExt.x = shp.min.x;
            }
            if (shp.min.y < minExt.y) {
                minExt.y = shp.min.y;
            }
            if (shp.min.z < minExt.z) {
                minExt.z = shp.min.z;
            }

            if (shp.max.x > maxExt.x) {
                maxExt.x = shp.max.x;
            }
            if (shp.max.y > maxExt.y) {
                maxExt.y = shp.max.y;
            }
            if (shp.max.z > maxExt.z) {
                maxExt.z = shp.max.z;
            }
        }
        auto modelOffset = -(minExt + ((maxExt - minExt) * 0.5f));
        glm::vec3 modelSize = maxExt - minExt;
        float maxDimension = std::max(modelSize.x, std::max(modelSize.y, modelSize.z));
        auto modelResize = glm::vec3(1 / maxDimension);

        for (auto shp : *s) {
            shp.shift(modelOffset);
            shp.scale(modelResize);
        }
}


void importModelsFrom(std::string directory, std::string prefix) {
    std::vector<tinyobj::shape_t> TOshapes;
    std::vector<tinyobj::material_t> objMaterials;
    std::string errStr;
    DIR *dir = opendir(directory.c_str());
    struct dirent *entry = nullptr;
    struct stat statbuf = {'\0'};
    if (dir != nullptr) {
        while ((entry = readdir(dir))) {
            if (entry->d_name[0] == '.') {
                continue;
            }
            std::string current_path = directory + "/" + std::string(entry->d_name);
            if (stat(current_path.c_str(), &statbuf) < 0) {
                std::cerr << "Failed to stat file " << current_path <<std::endl;
                exit(1);
            }
            if (S_ISDIR(statbuf.st_mode)) {
                if (prefix == "") {
                    importModelsFrom(current_path, entry->d_name);
                } else {
                    importModelsFrom(current_path, prefix + "/" + std::string(entry->d_name));
                }
            } else {
                if (std::string(entry->d_name).find(".obj") != std::string::npos) {
                    bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (directory + "/" + entry->d_name).c_str(), (directory + "/").c_str());
                    if (!rc) {
                        std::cerr << errStr << std::endl;
                        std::cerr << "Failed to load object file " << entry->d_name << std::endl;
                        exit(1);
                    } else {
                        auto result = std::make_shared<std::vector<Shape>>();
                        for (auto s : TOshapes) {
                            Shape m;
                            m.createShape(s);
                            m.measure();
                            m.init();
                            result->push_back(m);
                        }
                        normalizeModel(result);
                        for (auto om : objMaterials) {
                            auto m = std::make_shared<Material>();
                            m->Ambient = glm::vec3(om.ambient[0], om.ambient[1], om.ambient[2]);
                            m->Diffuse = glm::vec3(om.diffuse[0], om.diffuse[1], om.diffuse[2]);
                            m->Specular = glm::vec3(om.specular[0], om.specular[1], om.specular[2]);
                            m->shine = om.shininess;
                            if (om.name != "") {
                                materials[om.name] = m;
                            }
                        }
                        if (prefix == "") {
                            meshes[entry->d_name] = result;
                        } else {
                            meshes[(prefix + "/" + std::string(entry->d_name)).c_str()] = result;
                        }
                        
                    }
                }
            }
            
        }
    }
}