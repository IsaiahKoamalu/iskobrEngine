#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "Engine/System.h"
#include "Engine/Core/UpdateContext.h"
#include "Engine/Core/UpdateContext.h"
#include "Engine/ComponentManager.h"
#include "Engine/Components/Position.h"
#include "Engine/Components/Velocity.h"
#include <iostream>

#include "Engine/Components/PlayerComponent.h"
#include "Engine/Components/WallClingComponent.h"


/**
 * System which moves entities by applying velocity to position at each frame.
 * Requires: Position, Velocity.
 */
class MovementSystem : public System {
public:
    void update(const UpdateContext& ctxt) override{
        ComponentManager& components = *ctxt.component;
        float dt = ctxt.dt;

        for (Entity entity: entities) {
            auto &pos = components.getComponent<Position>(entity);
            auto &vel = components.getComponent<Velocity>(entity);

            //std::cout << "{" << pos.x << ", " << pos.y << "}" << std::endl;

            // Wall cling behavior checked first
            if (components.hasComponent<WallClingComponent>(entity)) {
                auto& cling = components.getComponent<WallClingComponent>(entity);
                auto& player = components.getComponent<PlayerComponent>(entity);
                if (cling.active) {

                    //player.gravityToggle = false;

                    // Limiting downward speed
                    if (vel.dy < cling.slideSpeed) {
                        vel.dy = cling.slideSpeed;
                    }

                    float intoWall = glm::dot(glm::vec2{vel.dx, vel.dy}, -cling.wallNormal);
                    if (intoWall > 0.f) {
                        glm::vec2 v = glm::vec2{vel.dx, vel.dy} - intoWall * (-cling.wallNormal);
                        vel.dx = v.x, vel.dy = v.y;
                    }

                    cling.timer += dt;

                    if (cling.maxTime > 0 && cling.timer > cling.maxTime || player.isGrounded) {
                        cling.active = false; // Release timer after timeout
                    }
                }else {
                    cling.timer = 0.f;
                    //player.gravityToggle = true;
                }
            }

            pos.x += vel.dx * dt;
            pos.y += vel.dy * dt;

            // Level bounds
            float levelWidth = 2000.f;
            float levelHeight = 2000.f;

            // Player size (if needed to offset half the sprite)
            float halfW = 96.f / 2.f;
            float halfH = 80.f / 2.f;

            // Clamp X
            if (pos.x < halfW) pos.x = halfW;
            if (pos.x > levelWidth - halfW) pos.x = levelWidth - halfW;

            // Clamp Y
            if (pos.y < halfH) pos.y = halfH;
            if (pos.y > levelHeight - halfH) pos.y = levelHeight - halfH;
        }
    }
};

#endif
