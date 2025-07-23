#ifndef EMITTERSYSTEM_H
#define EMITTERSYSTEM_H

#include "Engine/ComponentManager.h"
#include "Engine/Core/UpdateContext.h"
#include "Engine/System.h"
#include "Engine/Components/EmitterComponent.h"
#include "Engine/Components/Position.h"
#include "ParticleSystem/GaseousParticleSystem.h"
#include "ParticleSystem/FluidParticleSystem.h"

class EmitterSystem : public System {
public:
    void update(const UpdateContext &ctxt) override {
        ComponentManager& components = *ctxt.component;
        float dt = ctxt.dt; // Might use to slow down particle spawn rate
        auto& basePtrH = ctxt.particleSystems[0];
        auto& basePtrF = ctxt.particleSystems[1];
        auto& basePtrG = ctxt.particleSystems[2];

        auto hps = std::dynamic_pointer_cast<HomingParticleSystem>(basePtrH);
        auto fps = std::dynamic_pointer_cast<FluidParticleSystem>(basePtrF);
        auto gps = std::dynamic_pointer_cast<GaseousParticleSystem>(basePtrG);

        for (Entity entity : entities) {
            if (components.hasComponent<EmitterComponent>(entity)) {
                auto& emitter = components.getComponent<EmitterComponent>(entity);
                auto& pos = components.getComponent<Position>(entity);

                sf::Vector2f emitterPos = {pos.x, pos.y};

                if (emitter.type == "gas") {
                    gps->setEmitter(emitterPos);
                    gps->spawnParticles(emitter.amount);
                }
                if (emitter.type == "fluid") {
                    fps->setEmitter(emitterPos);
                    fps->spawnParticles(emitter.amount);
                }
            }
        }
    }
};

#endif