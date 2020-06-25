#include "BLZEntity.h"
#include "Shape.h"
#include <iostream>
#include <algorithm>
#include "components.h"


    std::shared_ptr<BLZEntity> BLZEntity::getptr() {
        return shared_from_this();
    }



    void BLZEntity::initialize(std::shared_ptr<std::vector<Shape>> s, BScene *scene, bool shouldCreateCollision) {
        auto renderComponent = c_render_t{};
        auto modelComponent = c_model_t{};
        modelComponent.model = s;
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
        auto modelOffset = minExt + ((maxExt - minExt) * 0.5f);
        glm::vec3 modelSize = maxExt - minExt;
        float maxDimension = std::max(modelSize.x, std::max(modelSize.y, modelSize.z));
        auto modelResize = glm::vec3(1 / maxDimension);

        auto locationComponent = c_location_t{};

        modelComponent.upperBound = (maxExt - modelOffset) * modelResize;
        modelComponent.lowerBound = (minExt - modelOffset) * modelResize;
        modelComponent.radius = calculateRadius(&modelComponent, &locationComponent);
        this->addComponent<c_model_t>(scene, modelComponent);
        this->addComponent<c_location_t>(scene, locationComponent);
        this->addComponent<c_render_t>(scene, renderComponent);

        if (shouldCreateCollision) {
            auto collisionComponent = c_collider_t{};   
            collisionComponent.upperBound = (maxExt - modelOffset) * modelResize;
            collisionComponent.lowerBound = (minExt - modelOffset) * modelResize;
            
            this->addComponent<c_collider_t>(scene, collisionComponent);
        }
    }

    BLZEntity::~BLZEntity()
    {
    }


