#ifndef DAMAGESYSTEM_H
#define DAMAGESYSTEM_H

#include <iostream>

#include "ParticleSystem/ParticleSystemBase.h"
#include "ParticleSystem/HomingParticleSystem.h"
#include "Engine/ComponentManager.h"
#include "Engine/System.h"
#include "Engine/Core/UpdateContext.h"
#include "Engine/SystemManager.h"
#include "Engine/Components/EmitterComponent.h"
#include "Engine/Components/Position.h"
#include "Engine/Components/HealthComponent.h"
#include "ParticleSystem/FluidParticleSystem.h"
#include "ParticleSystem/GaseousParticleSystem.h"

class DamageSystem : public System {
public:
    void update(const UpdateContext &ctxt) override {
        ComponentManager &components = *ctxt.component;
        EntityManager &entityManager = *ctxt.entity;
        SystemManager &systemManager = *ctxt.system;
        std::vector<sf::Drawable *> drawables = ctxt.drawables;
        auto &basePtrH = ctxt.particleSystems[0];
        auto &basePtrF = ctxt.particleSystems[1];
        auto &basePtrG = ctxt.particleSystems[2];
        auto &basePtrSF = ctxt.particleSystems[3];
        auto hps = std::dynamic_pointer_cast<HomingParticleSystem>(basePtrH);
        auto fps = std::dynamic_pointer_cast<FluidParticleSystem>(basePtrF);
        auto gps = std::dynamic_pointer_cast<GaseousParticleSystem>(basePtrG);
        auto sfps = std::dynamic_pointer_cast<ParticleSystem>(basePtrSF);
        float dt = ctxt.dt;

        for (Entity entity: entities) {
            if (components.hasComponent<PlayerComponent>(entity)) {
                auto &health = components.getComponent<HealthComponent>(entity);
                auto &player = components.getComponent<PlayerComponent>(entity);

                if (health.isLow) {
                    std::cout << "LOW HEALTH\n";
                }

                if (health.health < 80) {
                    health.isLow = true;
                }
            }
            // Deleting entities upon 'Death'
            if (components.hasComponent<HealthComponent>(entity) && !components.hasComponent<PlayerComponent>(entity)) {
                auto &health = components.getComponent<HealthComponent>(entity);
                if (health.health <= 0) {
                    health.isDead = true;
                }
                if (health.isDead) {
                    auto &pos = components.getComponent<Position>(entity);
                    sf::Vector2f burstPos = {0.f, 0.f};
                    burstPos = {pos.x, pos.y};

                    hps->setEmitter(burstPos);
                    sfps->setEmitter(burstPos);
                    hps->spawnParticles(5);
                    sfps->spawnParticles(20);

                    std::cout << "Entity Destroyed\n";
                    entityManager.destroyEntity(entity);
                    systemManager.entityDestroyed(entity);
                }
            }
        }
    }

    void affectHealth(int amount, Entity entity, ComponentManager &components)
    {
        if (components.hasComponent<HealthComponent>(entity))
            {
            auto &health = components.getComponent<HealthComponent>(entity);
            health.health += amount;
            if (amount < 0) {
                auto &pos = components.getComponent<Position>(entity);
            }
            if (health.health <= 0) { health.isDead = true; }
        }
    }
};

#endif
