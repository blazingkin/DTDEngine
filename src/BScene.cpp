#include "BLZEntity.h"
#include <vector>
#include <glad/glad.h>

#include <iostream>
#include "BScene.h"
#include "Program.h"

#include "Camera.h"


using namespace std;
using namespace glm;

std::map<std::string, std::shared_ptr<Texture>> textures;
std::map<std::string, std::shared_ptr<std::vector<Shape>>> meshes;
BScene *activeScene;
std::map<std::string, BScene> scenes;

BScene::BScene() {
    m_singleton = BLZEntity::newSimpleEntity();
    m_singleton->addComponent<c_singleton_t>(this, c_singleton_t{});
}

BScene::BScene(vec2 mS) {
    occupancyGridCellSize = glm::vec2(mS.x / NUM_GRID_CELLS, mS.y / NUM_GRID_CELLS);
    gridEntryCount = std::tuple<int, int>(NUM_GRID_CELLS, NUM_GRID_CELLS);
    for (int i = 0; i < get<0>(gridEntryCount); i++) {
        occupancyGrid.push_back({});
        for (int j = 0; j < get<1>(gridEntryCount); j++) {
            occupancyGrid[i].push_back({});
        }   
    }
    mapSize = mS;
    camera = new Camera();

    m_singleton = BLZEntity::newSimpleEntity();
    m_singleton->addComponent<c_singleton_t>(this, c_singleton_t{});
}

std::shared_ptr<BLZEntity> BScene::singleton() {
    return m_singleton;
}

int BScene::posInWorld(glm::vec3 pos){
    //NUM_GRID_CELLS
    auto loc = posToOccupancyIndex(pos);
    
    if (get<0>(loc) <= 0 || get<0>(loc) >= NUM_GRID_CELLS) {
        return OUTSIDE_X_BOUNDS;
    }
    if (get<1>(loc) <= 0 || get<1>(loc) >= NUM_GRID_CELLS) {
        return OUTSIDE_Z_BOUNDS;
    }
    return INSIDE_GRID;
}


std::tuple<int, int> BScene::posToOccupancyIndex(glm::vec3 pos){
    int i = (pos.x + mapSize.x) / (2.0f * occupancyGridCellSize.x);
    int j = (pos.z + mapSize.y) / (2.0f * occupancyGridCellSize.y);
    return std::tuple<int, int>(i, j);
}

glm::vec3 BScene::occupancyIndexToPos(std::tuple<float, float> pos) {
	int i = get<0>(pos);
	int j = get<1>(pos);
	float x = ((2 * occupancyGridCellSize.x) * i) - mapSize.x;
	float z = ((2 * occupancyGridCellSize.y) * j) - mapSize.y;
	return vec3(x, 0, z);
}

void BScene::removeFromGrid(std::shared_ptr<BLZEntity> entity, c_collider_t collider) {
    auto lowerIndices = posToOccupancyIndex(collider.lowerBound);
    auto upperIndices = posToOccupancyIndex(collider.upperBound);
    for (int i = std::max(get<0>(lowerIndices), 0); i <= std::min(get<0>(upperIndices), get<0>(gridEntryCount) - 1); i++) {
        for (int j = std::max(get<1>(lowerIndices), 0); j <= std::min(get<1>(upperIndices), get<1>(gridEntryCount) - 1); j++) {
            for (size_t n = 0; n < occupancyGrid[i][j].size(); n++) {
                if (occupancyGrid[i][j][n] == entity) {
                    occupancyGrid[i][j].erase(occupancyGrid[i][j].begin() + n);
                    break;
                }
            }
        }   
    }
}

void BScene::addToGrid(std::shared_ptr<BLZEntity> entity, c_collider_t collider) {
    auto lowerIndices = posToOccupancyIndex(collider.lowerBound);
    auto upperIndices = posToOccupancyIndex(collider.upperBound);
    for (int i = std::max(get<0>(lowerIndices), 0); i <= std::min(get<0>(upperIndices), get<0>(gridEntryCount) - 1); i++) {
        for (int j = std::max(get<1>(lowerIndices), 0); j <= std::min(get<1>(upperIndices), get<1>(gridEntryCount) - 1); j++) {
            occupancyGrid[i][j].push_back(entity);
        }   
    }
}

#define EPSILON_V3 vec3(0.5f, 0.5f, 0.5f)
set<shared_ptr<BLZEntity>> BScene::getEntitiesCloseToOrWithinExtents(c_collider_t extents) {
    set<shared_ptr<BLZEntity>> result;
    auto lowerIndices = posToOccupancyIndex(extents.lowerBound - EPSILON_V3);
    auto upperIndices = posToOccupancyIndex(extents.upperBound + EPSILON_V3);
    for (int i = std::max(get<0>(lowerIndices) - 1  , 0); i <= std::min(get<0>(upperIndices) + 1, get<0>(gridEntryCount) - 1); i++) {
        for (int j = std::max(get<1>(lowerIndices) - 1, 0); j <= std::min(get<1>(upperIndices) + 1, get<1>(gridEntryCount) - 1); j++) {
            for (auto entity : occupancyGrid[i][j]) {
                result.insert(entity);
            }
        }   
    }
    return result;
}

template <class T, class... Args>
void removeComponent(BScene *scene);

void BScene::RemoveEntity(std::shared_ptr<BLZEntity> obj) {
    for(std::map<component_id_t,std::unique_ptr<component_t>>::iterator iter = obj->components.begin(); iter != obj->components.end(); ++iter)
    {
        component_id_t k =  iter->first;
        uint32_t idx = 0;
        for (auto o : componentEntityMap[k]) {
            if (o == obj) {
                componentEntityMap[k].erase(componentEntityMap[k].begin() + idx);
                
            }
            idx++;
        }
        
    }

}


std::shared_ptr<BLZEntity> BScene::maybeFindClosestEntityWithComponent(component_id_t type, glm::vec3 position) {
    auto entitiesWithType = this->getEntitiesWithComponent(type);
    position.y = 0;
    std::shared_ptr<BLZEntity> closest = nullptr;
    float closestDistance = INFINITY;

    for (auto entity : entitiesWithType) {
        if (entity->hasComponent<c_location_t>()) {
            auto locationComponent = entity->getComponent<c_location_t>();
            auto l = locationComponent->position - vec3(0, locationComponent->position.y, 0);
            if (glm::length(l - position) < closestDistance) {
                closest = entity;
                closestDistance = glm::length(l - position);
            }
        }
    }
    return closest;
}


// TODO: Test Me
void BScene::registerEntityComponent(shared_ptr<BLZEntity> obj, component_id_t typ) {
    if (obj->tag == 0){
        obj->tag = counter++;
    }

    componentEntityMap[typ].push_back(obj);

}

// TODO: Test Me
void BScene::deleteEntityComponent(std::shared_ptr<BLZEntity> obj, component_id_t typ) {
        uint32_t idx = 0;
        for (auto o : componentEntityMap[typ]) {
            if (o == obj) {
                componentEntityMap[typ].erase(componentEntityMap[typ].begin() + idx);
            }
            idx++;
        }
}

std::vector<std::shared_ptr<BLZEntity>> BScene::getEntitiesWithComponent(component_id_t type){
    return componentEntityMap[type];
}

std::vector<std::shared_ptr<BLZEntity>> BScene::getEntitiesWithComponents(std::vector<component_id_t> types){
    auto result = std::vector<std::shared_ptr<BLZEntity>>();
    if (types.size() == 0) {
        return result;
    }
    auto first = getEntitiesWithComponent(types[0]);
    for_each(first.begin(), first.end(), [&result, types](std::shared_ptr<BLZEntity> element) {
        bool hasAllTypes = true;
        for (size_t i = 1; i < types.size(); i++) {
            if (!element->hasComponent(types[i])) {
                hasAllTypes = false;
                break;
            }
        }
        if (hasAllTypes) {
            result.push_back(element);
        }
    });
    return result;
}

