#ifndef PATHFOLLOWINGSYSTEM_H
#define PATHFOLLOWINGSYSTEM_H

#include "Engine/ComponentManager.h"
#include "Engine/System.h"
#include "Engine/Components/PathComponent.h"
#include "Engine/Components/PhysicsBody.h"
#include "Engine/Core/UpdateContext.h"

class PathFollowingSystem : public System {
public:
    void update(const UpdateContext& ctxt) override {
        auto& components = *ctxt.component;
        for (auto& entity : entities) {
            auto& path = components.getComponent<PathComponent>(entity);
            if (path.currentIndex >= path.steps.size()) continue;

            auto& body = components.getComponent<PhysicsBody>(entity);
            auto& step = path.steps[path.currentIndex];
            sf::Vector2f targetPos; // Tile->world
        }
    }
};

#endif