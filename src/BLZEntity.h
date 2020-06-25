#pragma once
#ifndef _BLZOBJECT_H_
#define _BLZOBJECT_H_

#include "Shape.h"
#include "Texture.h"
#include "Material.h"
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <stdint.h>
#include "_components.h"
#include <typeindex>
#include <typeinfo>
#include <iostream>
#include "BScene.h"
#include "Systems/SystemRender.h"

class BLZEntity : public std::enable_shared_from_this<BLZEntity> {
    public:
        virtual ~BLZEntity();
        static std::shared_ptr<BLZEntity> newEntity(std::shared_ptr<std::vector<Shape>> m, BScene *scene) {
            auto result = std::make_shared<BLZEntity>();
            result->initialize(m, scene, true);
            return result;
        }

        static std::shared_ptr<BLZEntity> newSimpleEntity(std::shared_ptr<std::vector<Shape>> m, BScene *scene) {
            auto result = std::make_shared<BLZEntity>();
            result->initialize(m, scene, false);
            return result;
        }

        static std::shared_ptr<BLZEntity> newSimpleEntity() {
            auto result = std::make_shared<BLZEntity>();
            return result;
        }

        static std::shared_ptr<BLZEntity> newEntity() {
            auto result = std::make_shared<BLZEntity>();
            return result;
        }

        uint64_t tag = 0;
        std::map<component_id_t, std::unique_ptr<component_t>> components;
        std::shared_ptr<BLZEntity> getptr();
        
        template <class T, class... Args>
        void addComponent(BScene *scene, Args &&... args) {
            if(!components.emplace(
                std::type_index(typeid(T)), std::make_unique<T>(std::forward<Args>(args)...)
            ).second)
                throw std::runtime_error("Component already present.");
            scene->registerEntityComponent(getptr(), std::type_index(typeid(T)));
        }

        template <class T, class... Args>
        void removeComponent(BScene *scene) {
            if (0 == components.erase(std::type_index(typeid(T)))) {
                throw std::runtime_error("Component was not present.");
            }
            scene->deleteEntityComponent(getptr(), std::type_index(typeid(T)));
        }

        template <class T>
        T *getComponent() {
            auto found = components.find(std::type_index(typeid(T)));
            return found == end(components)
                ? nullptr
                : static_cast<T*>(found->second.get());
        }
        template <class T>
        bool hasComponent() {
            auto found = components.find(std::type_index(typeid(T)));
            return found != end(components);
        }

        bool hasComponent(component_id_t type) {
            return components.count(type) != 0;
        }

    private:
        void initialize(std::shared_ptr<std::vector<Shape>> m, BScene *scene, bool shouldCreateCollision);

};


#endif