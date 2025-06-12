#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "Engine/System.h"
#include "Engine/ComponentManager.h"
#include "Engine/Components/ColliderComponent.h"
#include "Engine/Components/Position.h"

/**
 * @brief A system responsible for identifying collisions and resolving them.
 */
class CollisionSystem : public System {
public:
    void update(ComponentManager& components, float dt) {
        for (Entity a : entities) {
            if (!components.hasComponent<ColliderComponent>(a) || !components.hasComponent<Position>(a)) continue;

            auto& aCollider = components.getComponent<ColliderComponent>(a);
            auto& aPos = components.getComponent<Position>(a);
            sf::FloatRect aBounds = aCollider.bounds;
            aBounds.left += aPos.x;
            aBounds.top += aPos.y;

            for (Entity b : entities) {
                if (a == b) continue;
                if (!components.hasComponent<ColliderComponent>(b) || !components.hasComponent<Position>(b)) continue;

                auto& bCollider = components.getComponent<ColliderComponent>(b);
                auto& bPos = components.getComponent<Position>(b);
                sf::FloatRect bBounds = bCollider.bounds;
                bBounds.left += bPos.x;
                bBounds.top += bPos.y;

                sf::FloatRect intersection;
                if (aBounds.intersects(bBounds, intersection)) {
                    if (!aCollider.isTrigger && !bCollider.isTrigger) {
                        float overlapX = intersection.width;
                        float overlapY = intersection.height;

                        if (overlapX < overlapY) {
                            // Push in X direction
                            if (aBounds.left < bBounds.left)
                                aPos.x -= overlapX;
                            else
                                aPos.x += overlapX;
                        } else {
                            // Push in Y direction
                            if (aBounds.top < bBounds.top)
                                aPos.y -= overlapY;
                            else
                                aPos.y += overlapY;
                        }
                    }
                }

            }
        }
    }

};

#endif