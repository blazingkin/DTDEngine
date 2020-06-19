#ifndef ASSET_HELPER_H
#define ASSET_HELPER_H 1
#include <dirent.h>
#include <iostream>
#include <memory>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <iostream>
#include "BScene.h"
#include "Shape.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

void importModelsFrom(std::string directory, std::string prefix);

#endif