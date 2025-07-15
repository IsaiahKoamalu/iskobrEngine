#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H


#include "Engine/System.h"
#include "Engine/ComponentManager.h"
#include "Engine/Components/ColliderComponent.h"
#include "Engine/Components/AttackColliderComponent.h"
#include "Engine/Components/HealthComponent.h"
#include "Engine/Components/KnockBackComponent.h"
#include "Engine/Components/Position.h"
#include "Engine/Components/PlayerComponent.h"
#include "Engine/Components/TileComponent.h"
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
    bool isSolidAt(ComponentManager &component, float x, float y) const {
        for (Entity e: entities) {
            if (component.hasComponent<TileComponent>(e)
                && component.hasComponent<ColliderComponent>(e)
                && component.hasComponent<Position>(e)) {
                auto &col = component.getComponent<ColliderComponent>(e);
                auto &pos = component.getComponent<Position>(e);

                sf::FloatRect bounds{
                    pos.x + col.bounds.left,
                    pos.y + col.bounds.top,
                    col.bounds.width,
                    col.bounds.height
                };

                if (bounds.contains(x, y)) {
                    return true;
                }
            }
        }
        return false;
    }

    void handleWallContacts(Entity entity, ComponentManager &components, const Contact &contact) {
        const float verticalThreshold = 0.4f;
        glm::vec2 up{0.f, -1.f};

        float dotUp = glm::dot(contact.normal, up);
        if (std::abs(dotUp) < verticalThreshold) {
            auto &cling = components.getComponent<WallClingComponent>(entity);
            cling.active = true;
            cling.timer = 0.f;
            cling.wallNormal = contact.normal; //Points from the wall to player
            std::cout << "Wall contact" << std::endl;
        }
    }

    void affectHealth(int amount, Entity entity, ComponentManager &components) {
        if (components.hasComponent<HealthComponent>(entity)) {
            auto &health = components.getComponent<HealthComponent>(entity);
            health.health += amount;
            if (health.health <= 0) { health.isDead = true; }
        }
    }

    //void setDamageSystem(const DamageSystem* ds) {m_damageSystem = ds;}

    void update(ComponentManager &components,float dt) {
        // clear the *touched* flag ( not the active flag)
        for (Entity e: entities)
            if (components.hasComponent<WallClingComponent>(e))
                components.getComponent<WallClingComponent>(e).touchedThisFrame = false;


        // Attack collision loop
        for (Entity entity: entities) {
            if (components.hasComponent<AttackColliderComponent>(entity) && components.hasComponent<Position>(entity)) {
                auto &attCol = components.getComponent<AttackColliderComponent>(entity);
                auto &attPos = components.getComponent<Position>(entity);

                sf::FloatRect attBoundsRight{
                    attPos.x + attCol.boundsRight.left,
                    attPos.y + attCol.boundsRight.top,
                    attCol.boundsRight.width,
                    attCol.boundsRight.height
                };

                sf::FloatRect attBoundsLeft{
                    attPos.x + attCol.boundsLeft.left,
                    attPos.y + attCol.boundsLeft.top,
                    attCol.boundsLeft.width,
                    attCol.boundsRight.height
                };

                for (Entity other: entities) {
                    if (other == entity) continue;
                    if (!components.hasComponent<ColliderComponent>(other) || !components.hasComponent<
                            HealthComponent>(other)) {
                        continue;
                    }
                    auto &otherCol = components.getComponent<ColliderComponent>(other);
                    auto &otherPos = components.getComponent<Position>(other);

                    sf::FloatRect otherBounds{
                        otherPos.x + otherCol.bounds.left,
                        otherPos.y + otherCol.bounds.top,
                        otherCol.bounds.width,
                        otherCol.bounds.height
                    };

                    sf::FloatRect attackIntersection;
                    bool hit = false;

                    if (attCol.activeRight) {
                        hit = attBoundsRight.intersects(otherBounds, attackIntersection);
                        if (hit) {
                            std::cout << components.getComponent<HealthComponent>(other).health << std::endl;
                        }
                    }
                    if (!hit && attCol.activeLeft) {
                        hit = attBoundsLeft.intersects(otherBounds, attackIntersection);
                    }

                    float overlapX = attackIntersection.width;
                    float overlapY = attackIntersection.height;

                    if (overlapX < overlapY && attCol.activeRight) {
                        std::cout << "Hit Right" << std::endl;
                        auto& player = components.getComponent<PlayerComponent>(entity);
                        auto& pos = components.getComponent<Position>(entity);
                        auto& otherPos = components.getComponent<Position>(other);
                        affectHealth(-20, other, components);
                        player.isSlashing = false;

                        auto& knockOther = components.getComponent<KnockBackComponent>(other);
                        sf::Vector2f direction = normalize(sf::Vector2f{otherPos.x, otherPos.y} - sf::Vector2f{pos.x, pos.y});
                        knockOther.velocity = direction * knockOther.force;
                        knockOther.isKnockback = true;

                    } else if (overlapX < overlapY && attCol.activeLeft) {
                        auto& player = components.getComponent<PlayerComponent>(entity);
                        auto& pos = components.getComponent<Position>(entity);
                        auto& otherPos = components.getComponent<Position>(other);
                        affectHealth(-20, other, components);
                        player.isSlashing = false;

                        auto& knockOther = components.getComponent<KnockBackComponent>(other);
                        sf::Vector2f direction = normalize(sf::Vector2f{otherPos.x, otherPos.y} - sf::Vector2f{pos.x, pos.y}); //Make direction = (pos - otherPos) for a pull effect
                        float force = 300.f;
                        knockOther.velocity = direction * force;
                        knockOther.isKnockback = true;
                    }
                }
            }
        }

        // Double loop: broad-phase & narrow-phase collision tests (might change)
        for (Entity a: entities) {
            if (!components.hasComponent<ColliderComponent>(a) ||
                !components.hasComponent<Position>(a))
                continue;

            auto &aCol = components.getComponent<ColliderComponent>(a);
            auto &aPos = components.getComponent<Position>(a);

            sf::FloatRect aBounds{
                aPos.x + aCol.bounds.left,
                aPos.y + aCol.bounds.top,
                aCol.bounds.width,
                aCol.bounds.height
            };

            for (Entity b: entities) {
                if (a == b) continue;
                if (!components.hasComponent<ColliderComponent>(b) ||
                    !components.hasComponent<Position>(b))
                    continue;

                auto &bCol = components.getComponent<ColliderComponent>(b);
                auto &bPos = components.getComponent<Position>(b);

                sf::FloatRect bBounds{
                    bPos.x + bCol.bounds.left,
                    bPos.y + bCol.bounds.top,
                    bCol.bounds.width,
                    bCol.bounds.height
                };

                sf::FloatRect intersection;
                if (!aBounds.intersects(bBounds, intersection))
                    continue; // → no collision this pair

                // Build a contact normal so that “wall” vs “floor/ceiling” is discernible
                float overlapX = intersection.width;
                float overlapY = intersection.height;

                glm::vec2 normal{0.f, 0.f};
                if (overlapX < overlapY) // side hit
                    normal.x = (aBounds.left < bBounds.left) ? -1.f : 1.f;
                else // top/bottom hit
                    normal.y = (aBounds.top < bBounds.top) ? -1.f : 1.f;

                //Wall-cling
                if (components.hasComponent<WallClingComponent>(a)) {
                    Contact c;
                    c.other = b;
                    c.normal = normal;
                    c.penetration = (overlapX < overlapY) ? overlapX : overlapY;

                    if (components.hasComponent<PlayerComponent>(a)) {
                        auto &player = components.getComponent<PlayerComponent>(a);
                        if (!player.isGrounded) {
                            handleWallContacts(a, components, c); // sets active, timer, etc.
                            components.getComponent<WallClingComponent>(a).touchedThisFrame = true;
                        }
                    }
                }

                // base penetration–resolution logic
                if (!aCol.isTrigger && !bCol.isTrigger) {
                    if (overlapX < overlapY && !components.hasComponent<TileComponent>(b) && components.hasComponent<
                            PlayerComponent>(a)) // resolve along X
                    {
                        auto &player = components.getComponent<PlayerComponent>(a);
                        if (!player.isRolling) {
                            if (normal.x < 0) aPos.x -= overlapX;
                            else aPos.x += overlapX;
                        }
                    }

                    else if (overlapX < overlapY && components.hasComponent<TileComponent>(b)) // resolve along X
                    {
                        if (normal.x < 0) aPos.x -= overlapX;
                        else aPos.x += overlapX;
                    } else if (components.hasComponent<TileComponent>(b)) // resolve along Y
                    {
                        bool fromAbove = normal.y < 0; // a is above b
                        if (fromAbove) {
                            aPos.y -= overlapY;

                            if (components.hasComponent<Velocity>(a))
                                components.getComponent<Velocity>(a).dy = 0.f;

                            if (components.hasComponent<PlayerComponent>(a))
                                components.getComponent<PlayerComponent>(a).isGrounded = true;
                        } else // collision from below
                        {
                            aPos.y += overlapY;

                            if (components.hasComponent<Velocity>(a))
                                components.getComponent<Velocity>(a).dy = 0.f;
                        }
                    }
                }
            } // inner
        } // outer
        //Post-pass – turn cling off if *never* touched a wall this frame
        for (Entity e: entities)
            if (components.hasComponent<WallClingComponent>(e)) {
                auto &cling = components.getComponent<WallClingComponent>(e);
                //if (!cling.touchedThisFrame) cling.active = false;
            }
    }

private:
    //const DamageSystem* m_damageSystem = nullptr;
};

#endif
