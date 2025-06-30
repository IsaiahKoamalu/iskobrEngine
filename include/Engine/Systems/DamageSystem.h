#ifndef DAMAGESYSTEM_H
#define DAMAGESYSTEM_H

#include <iostream>
#include "Engine/ComponentManager.h"
#include "Engine/System.h"
#include "Engine/Components/HealthComponent.h"

class DamageSystem : public System {
public:
    void update(ComponentManager& components, float dt) {
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
        }
    }
};

#endif