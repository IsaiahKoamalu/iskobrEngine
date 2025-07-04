#ifndef DAMAGESYSTEM_H
#define DAMAGESYSTEM_H

#include <iostream>

#include "ParticleSystem.h"
#include "Engine/ComponentManager.h"
#include "Engine/System.h"
#include "Engine/SystemManager.h"
#include "Engine/Components/Position.h"
#include "Engine/Components/HealthComponent.h"

class DamageSystem : public System {
public:
    void update(ComponentManager& components, EntityManager& entityManager, SystemManager& systemManager,  ParticleSystem& particleSystem, float dt) {
        for (Entity entity : entities) {
            if (components.hasComponent<PlayerComponent>(entity)) {
                auto& health = components.getComponent<HealthComponent>(entity);
                auto& player = components.getComponent<PlayerComponent>(entity);

                if (health.isLow) {
                    std::cout << "LOW HEALTH\n";
                }

                if (health.health < 80) {
                    health.isLow = true;
                }
            }
            // Deleting entities upon 'Death'
            if (components.hasComponent<HealthComponent>(entity) && !components.hasComponent<PlayerComponent>(entity)) {
                auto& health = components.getComponent<HealthComponent>(entity);
                if (health.isDead) {
                    auto& pos = components.getComponent<Position>(entity);
                    sf::Vector2f burstPos = {0.f, 0.f};
                    burstPos = {pos.x, pos.y};

                    particleSystem.setEmitter(burstPos);
                    std::cout << "Entity Destroyed\n";
                    particleSystem.spawnParticles(80);
                    entityManager.destroyEntity(entity);
                    systemManager.entityDestroyed(entity);
                }
            }
        }
    }
};

#endif