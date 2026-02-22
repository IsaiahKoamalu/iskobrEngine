//
// Created by ikbro on 2/21/2026.
//

#ifndef ISKOBRENGINE_ENTITYFACTORY_H
#define ISKOBRENGINE_ENTITYFACTORY_H

#include "Engine/ComponentManager.h"
#include "Engine/EntityManager.h"
#include "Engine/SystemManager.h"
#include "Engine/Core/UpdateContext.h"
#include "Engine/Components/Position.h"
#include "Engine/Components/Velocity.h"
#include "Engine/Components/ProjectileComponent.h"
#include "Engine/Components/EmitterComponent.h"
#include "Engine/Systems/EmitterSystem.h"
#include "Engine/Systems/ProjectileSystem.h"

namespace EntityFactory
{
    inline void spawnParticleProjectile(const UpdateContext& ctxt, float startX, float startY, float dirX, float dirY)
    {
        ComponentManager &components = *ctxt.component;
        EntityManager &entityManager = *ctxt.entity;
        SystemManager &systemManager = *ctxt.system;

        Entity projectile = entityManager.createEntity();

        components.addComponent<Position>(projectile, {startX, startY});
        float speed = 500.0f;
        components.addComponent<Velocity>(projectile, {speed, 0.0f});
        components.addComponent<ProjectileComponent>(projectile, {
            3.0f,
            "gas",
            false
        });

        components.addComponent<EmitterComponent>(projectile, {
            "gas",
            5
        });

        auto projSys = systemManager.getSystem<ProjectileSystem>();
        if (projSys) projSys->entities.insert(projectile);

        auto emitSys = systemManager.getSystem<EmitterSystem>();
        if (emitSys) emitSys->entities.insert(projectile);

        std::cout << "PROJECTILE ENTITY CREATED {" << projectile << "}" << std::endl;
    }
}

#endif //ISKOBRENGINE_ENTITYFACTORY_H