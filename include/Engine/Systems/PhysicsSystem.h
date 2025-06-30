#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H
#include "Engine/ComponentManager.h"
#include "Engine/System.h"
#include "Engine/Components/Velocity.h"

class PhysicsSystem : public System {
public:
    void update(ComponentManager &components, float dt) {
        float gravity = 1500.0f;

        for (Entity entity : entities) {
            if (components.hasComponent<Velocity>(entity) && components.hasComponent<PlayerComponent>(entity)) {
                auto &velocity = components.getComponent<Velocity>(entity);
                auto &player = components.getComponent<PlayerComponent>(entity);

                if (player.gravityToggle) {
                    velocity.dy += gravity * dt;
                }
            }
            else if (!components.hasComponent<PlayerComponent>(entity) && components.hasComponent<Velocity>(entity)) {
                auto& velocity = components.getComponent<Velocity>(entity);
                velocity.dy += gravity * dt;
            }
        }
    }
};

#endif
