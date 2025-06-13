#ifndef TRIGGERSYSTEM_H
#define TRIGGERSYSTEM_H

#include "Engine/System.h"
#include "Engine/ComponentManager.h"
#include "Engine/Components/Position.h"
#include "Engine/Components/ColliderComponent.h"
#include <iostream>


/**
 * System that uitilizes collider component in order to trigger events.
 */
class TriggerSystem : public System {
public:
    void update(ComponentManager& components, float dt){
        for (Entity a : entities) {
            if (!components.hasComponent<ColliderComponent>(a)) continue;
            if (!components.hasComponent<Position>(a)) continue;

            auto& aCol = components.getComponent<ColliderComponent>(a);
            auto& aPos = components.getComponent<Position>(a);
            if (!aCol.isTrigger) continue;

            sf::FloatRect aBounds = aCol.bounds;
            aBounds.left += aPos.x;
            aBounds.top += aPos.y;

            for (auto& b : entities) {
                if (a == b) continue;

                if (!components.hasComponent<ColliderComponent>(b)) continue;
                if (!components.hasComponent<Position>(b)) continue;

                auto& bCol = components.getComponent<ColliderComponent>(b);
                auto& bPos = components.getComponent<Position>(b);
                if (bCol.isTrigger) continue;

                sf::FloatRect bBounds = bCol.bounds;
                bBounds.left += bPos.x;
                bBounds.top += bPos.y;

               if (aBounds.intersects(bBounds)) {

                   aCol.active = false; // Mark trigger as used (if applicable)
               }
            }
        }
    }
};

#endif