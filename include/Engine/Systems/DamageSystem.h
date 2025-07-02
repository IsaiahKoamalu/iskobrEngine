#ifndef DAMAGESYSTEM_H
#define DAMAGESYSTEM_H

#include <iostream>
#include "Engine/ComponentManager.h"
#include "Engine/System.h"
#include "Engine/SystemManager.h"
#include "Engine/Components/HealthComponent.h"

class DamageSystem : public System {
public:
    void update(ComponentManager& components, EntityManager& entityManager, SystemManager& systemManager, float dt) {
        for (Entity entity : entities) {
            if (components.hasComponent<PlayerComponent>(entity)) {
                auto& health = components.getComponent<HealthComponent>(entity);
                auto& player = components.getComponent<PlayerComponent>(entity);

                if (health.isLow) {
                    std::cout << "LOW HEALTH\n";
                }

                std::cout << entity << ":" << health.health << std::endl;

                if (health.health < 80) {
                    health.isLow = true;
                }
            }
            // Deleting entities upon 'Death'
            if (components.hasComponent<HealthComponent>(entity) && !components.hasComponent<PlayerComponent>(entity)) {
                auto& health = components.getComponent<HealthComponent>(entity);
                if (health.isDead) {
                    std::cout << "Entity Destroyed\n";
                    entityManager.destroyEntity(entity);
                    systemManager.entityDestroyed(entity);
                }
            }
        }
    }
};

#endif