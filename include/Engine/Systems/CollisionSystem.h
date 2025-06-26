#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "Engine/System.h"
#include "Engine/ComponentManager.h"
#include "Engine/Components/ColliderComponent.h"
#include "Engine/Components/Position.h"
#include "Engine/Components/PlayerComponent.h"
#include "Engine/Components/WallClingComponent.h"


// TESTING OUT COMMENTING AND DOCUMENTATION STYLE

struct Contact {
    Entity other;
    glm::vec2 point;
    glm::vec2 normal;
    float penetration{};
};

/**
 * @brief A system responsible for identifying collisions and resolving them.
 */
class CollisionSystem : public System {
public:

    void handleWallContacts(Entity entity, ComponentManager &components, const Contact& contact) {
        const float verticalThreshold = 0.4f;
        glm::vec2 up {0.f, -1.f};

        float dotUp = glm::dot(contact.normal, up);
        if (std::abs(dotUp) < verticalThreshold) {
            auto& cling = components.getComponent<WallClingComponent>(entity);
            cling.active = true;
            cling.timer = 0.f;
            cling.wallNormal = contact.normal; //Points from the wall to player
            std::cout << "Wall contact" << std::endl;
        }
    }

    void update(ComponentManager &components, float dt)
{

    // clear the *touched* flag (but NOT the active flag)
    for (Entity e : entities)
        if (components.hasComponent<WallClingComponent>(e))
            components.getComponent<WallClingComponent>(e).touchedThisFrame = false;


    // Double loop: broad-phase & narrow-phase collision tests
    for (Entity a : entities)
    {
        if (!components.hasComponent<ColliderComponent>(a) ||
            !components.hasComponent<Position>(a))
            continue;

        auto &aCol = components.getComponent<ColliderComponent>(a);
        auto &aPos = components.getComponent<Position>(a);

        sf::FloatRect aBounds{ aPos.x + aCol.bounds.left,
                               aPos.y + aCol.bounds.top,
                               aCol.bounds.width,
                               aCol.bounds.height };

        for (Entity b : entities)
        {
            if (a == b) continue;
            if (!components.hasComponent<ColliderComponent>(b) ||
                !components.hasComponent<Position>(b))
                continue;

            auto &bCol = components.getComponent<ColliderComponent>(b);
            auto &bPos = components.getComponent<Position>(b);

            sf::FloatRect bBounds{ bPos.x + bCol.bounds.left,
                                   bPos.y + bCol.bounds.top,
                                   bCol.bounds.width,
                                   bCol.bounds.height };

            sf::FloatRect intersection;
            if (!aBounds.intersects(bBounds, intersection))
                continue;                        // → no collision this pair

            // Build a contact normal so we “wall” vs “floor/ceiling” is discernible
            float overlapX = intersection.width;
            float overlapY = intersection.height;

            glm::vec2 normal{0.f, 0.f};
            if (overlapX < overlapY)                 // side hit
                normal.x = (aBounds.left < bBounds.left) ? -1.f : 1.f;
            else                                     // top/bottom hit
                normal.y = (aBounds.top < bBounds.top) ? -1.f : 1.f;

            //Wall-cling hook
            if (components.hasComponent<WallClingComponent>(a))
            {
                Contact c;
                c.other       = b;
                c.normal      = normal;
                c.penetration = (overlapX < overlapY) ? overlapX : overlapY;

                if (components.hasComponent<PlayerComponent>(a)) {
                    auto& player = components.getComponent<PlayerComponent>(a);
                    if (!player.isGrounded) {
                        handleWallContacts(a, components, c);          // sets active, timer, etc.
                        components.getComponent<WallClingComponent>(a).touchedThisFrame = true;
                    }
                }
            }


            //Your existing penetration–resolution logic (unchanged)
            if (!aCol.isTrigger && !bCol.isTrigger)
            {
                if (overlapX < overlapY)                        // resolve along X
                {
                    if (normal.x < 0) aPos.x -= overlapX;
                    else              aPos.x += overlapX;
                }
                else                                            // resolve along Y
                {
                    bool fromAbove = normal.y < 0;              // a is above b
                    if (fromAbove)
                    {
                        aPos.y -= overlapY;

                        if (components.hasComponent<Velocity>(a))
                            components.getComponent<Velocity>(a).dy = 0.f;

                        if (components.hasComponent<PlayerComponent>(a))
                            components.getComponent<PlayerComponent>(a).isGrounded = true;
                    }
                    else  // collision from below
                    {
                        aPos.y += overlapY;

                        if (components.hasComponent<Velocity>(a))
                            components.getComponent<Velocity>(a).dy = 0.f;
                    }
                }
            }
        } // end inner loop
    }     // end outer loop

    //Post-pass – turn cling off if *never* touched a wall this frame
    for (Entity e : entities)
        if (components.hasComponent<WallClingComponent>(e))
        {
            auto &cling = components.getComponent<WallClingComponent>(e);
            //if (!cling.touchedThisFrame) cling.active = false; // (This was causing problems)
        }
    }

};

#endif