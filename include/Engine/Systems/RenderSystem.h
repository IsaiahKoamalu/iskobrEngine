#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "Engine/System.h"
#include "Engine/Core/UpdateContext.h"
#include "Engine/Components/Position.h"
#include "Engine/ComponentManager.h"
#include "Engine/Components/SpriteComponent.h"

#include "Engine/Components/AttackColliderComponent.h"
#include "Engine/Components/ColliderComponent.h"
#include "Engine/Components/HealthComponent.h"
#include "Engine/Components/TileComponent.h"
#include "ParticleSystem/HomingParticleSystem.h"

/**
 * System that draws entities with a given Position and SpriteComponent
 **/

// Forward declaration (globally defined?)

class RenderSystem : public System {
public:
    void update(const UpdateContext& ctxt) override{
        sf::RenderWindow& window = *ctxt.window;
        ComponentManager& components = *ctxt.component;
        std::vector<sf::Drawable*> drawables = ctxt.drawables;

        bool debugMode = false;

        // First draw all tile components
        for (auto entity: entities) {
            if (components.hasComponent<TileComponent>(entity)) {
                auto &pos = components.getComponent<Position>(entity);
                auto &tile = components.getComponent<TileComponent>(entity);
                tile.sprite.setPosition(pos.x, pos.y);
                window.draw(tile.sprite);
            }
        }

        if (debugMode) {
            for (auto entity: entities) {
                if (components.hasComponent<ColliderComponent>(entity)) {
                    const auto &collider = components.getComponent<ColliderComponent>(entity);
                    const auto &pos = components.getComponent<Position>(entity);

                    sf::RectangleShape debugRect;
                    debugRect.setSize({collider.bounds.width, collider.bounds.height});
                    debugRect.setPosition(pos.x + collider.bounds.left, pos.y + collider.bounds.top);
                    debugRect.setFillColor(sf::Color::Transparent);
                    debugRect.setOutlineColor(sf::Color::Red);
                    debugRect.setOutlineThickness(1.0f);
                    window.draw(debugRect);
                }
                if (components.hasComponent<AttackColliderComponent>(entity)) {
                    const auto& collider = components.getComponent<AttackColliderComponent>(entity);
                    if (collider.activeRight) {
                        const auto& pos = components.getComponent<Position>(entity);

                        sf::RectangleShape debugRect;
                        debugRect.setSize({collider.boundsRight.width, collider.boundsRight.height});
                        debugRect.setPosition(pos.x + collider.boundsRight.left, pos.y + collider.boundsRight.top);
                        debugRect.setFillColor(sf::Color::Green);
                        debugRect.setOutlineColor(sf::Color::Green);
                        debugRect.setOutlineThickness(1.0f);
                        window.draw(debugRect);
                    }
                    else if (collider.activeLeft) {
                        const auto& pos = components.getComponent<Position>(entity);

                        sf::RectangleShape debugRect;
                        debugRect.setSize({collider.boundsLeft.width, collider.boundsLeft.height});
                        debugRect.setPosition(pos.x + collider.boundsLeft.left, pos.y + collider.boundsLeft.top);
                        debugRect.setFillColor(sf::Color::Green);
                        debugRect.setOutlineColor(sf::Color::Green);
                        debugRect.setOutlineThickness(1.0f);
                        window.draw(debugRect);
                    }
                }
            }
        }

        //  drawing all sprite components (like player)
        for (auto entity: entities) {
            if (components.hasComponent<SpriteComponent>(entity) &&
                !components.hasComponent<TileComponent>(entity))
            {
                auto &spriteComp = components.getComponent<SpriteComponent>(entity);
                auto &pos = components.getComponent<Position>(entity);
                spriteComp.sprite.setPosition(pos.x, pos.y);
                window.draw(spriteComp.sprite);
            }
        }
        for (auto obj : drawables) {
            window.draw(*obj);
        }
    }

};

#endif
