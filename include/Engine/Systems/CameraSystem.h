#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include "Engine/System.h"
#include "Engine/Components/Position.h"
#include <SFML/Graphics/View.hpp>

#include "Engine/ComponentManager.h"

class CameraSystem : public System {
public:
    sf::View view;

    CameraSystem(float width, float height) {
        view.setSize(width, height);
        view.setCenter(0.f, 0.f);
    }

    void update(ComponentManager &components, float dt) {
        for (Entity entity: entities) {
            if (!components.hasComponent<Position>(entity)) continue;
            auto &pos = components.getComponent<Position>(entity);

            float targetX = pos.x;
            float targetY = pos.y;

            float halfWidth = view.getSize().x / 2.f;
            float halfHeight = view.getSize().y / 2.f;

            // Level bounds (example: 2000x2000 pixels)
            float levelWidth = 2000.f;
            float levelHeight = 2000.f;

            // Clamp X
            if (targetX < halfWidth) targetX = halfWidth;
            if (targetX > levelWidth - halfWidth) targetX = levelWidth - halfWidth;

            // Clamp Y
            if (targetY < halfHeight) targetY = halfHeight;
            if (targetY > levelHeight - halfHeight) targetY = levelHeight - halfHeight;

            view.setCenter(targetX, targetY);
            break; // Only first entity should be tracked (player)
        }
    }
};

#endif
