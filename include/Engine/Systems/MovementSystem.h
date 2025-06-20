#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "Engine/System.h"
#include "Engine/ComponentManager.h"
#include "Engine/Components/Position.h"
#include "Engine/Components/Velocity.h"
#include <iostream>


/**
 * System which moves entities by applying velocity to position at each frame.
 * Requires: Position, Velocity.
 */
class MovementSystem : public System {
public:
    void update(ComponentManager &components, float dt) {
        for (Entity entity: entities) {
            auto &pos = components.getComponent<Position>(entity);
            auto &vel = components.getComponent<Velocity>(entity);

            //std::cout << "{" << pos.x << ", " << pos.y << "}" << std::endl;

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
