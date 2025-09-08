//
// Created by ikbro on 9/8/2025.
//

#ifndef PARTICLEGENSYSTEM_H
#define PARTICLEGENSYSTEM_H

#include <iostream>

#include "ParticleSystem/ParticleSystemBase.h"
#include "ParticleSystem/HomingParticleSystem.h"
#include "Engine/ComponentManager.h"
#include "Engine/EntityManager.h"
#include "Engine/System.h"
#include "Engine/Core/UpdateContext.h"
#include "Engine/SystemManager.h"
#include "Engine/Components/EmitterComponent.h"
#include "Engine/Components/Position.h"
#include "Engine/Components/HealthComponent.h"
#include "ParticleSystem/FluidParticleSystem.h"
#include "ParticleSystem/GaseousParticleSystem.h"


class ParticleGenSystem : public System
{
public:
    void update(const UpdateContext &ctxt) override
    {
        ComponentManager &components = *ctxt.component;
        EntityManager &entityManager = *ctxt.entity;
        SystemManager &systemManager = *ctxt.system;
        std::vector<sf::Drawable *> drawables = ctxt.drawables;
        auto &basePtrG = ctxt.particleSystems[2];
        auto gps = std::dynamic_pointer_cast<GaseousParticleSystem>(basePtrG);

        for (Entity entity : entities)
        {
            /*if (components.hasComponent<PlayerComponent>(entity))
            {
                auto &player = components.getComponent<PlayerComponent>(entity);
                if (player.isSmoking && player.smokingTimer < 0.2)
                {
                    auto &pos = components.getComponent<Position>(entity);
                    sf::Vector2f burstPos = {0.f, 0.f};
                    burstPos = {pos.x + 10.f, pos.y - 20.f};

                    gps->setEmitter(burstPos);
                    gps->spawnParticles(20);
                }
            }*/
        }
    }
};

#endif //PARTICLEGENSYSTEM_H
