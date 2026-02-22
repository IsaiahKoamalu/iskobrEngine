//
// Created by ikbro on 2/20/2026.
//

#ifndef PROJECTILESYSTEM_H
#define PROJECTILESYSTEM_H

#include "Engine/ComponentManager.h"
#include "Engine/EntityManager.h"
#include "Engine/Core/UpdateContext.h"
#include "Engine/System.h"
#include "Engine/SystemManager.h"
#include "Engine/Components/ProjectileComponent.h"
#include "Engine/Components/Position.h"
#include "Engine/Components/Velocity.h"

/**
 * @brief In order for Projectile System to grab entities, they MUST have:
 *        - Projectile Component
 *        - Velocity Component
 *        - Transform Component
 */
class ProjectileSystem : public System
{
public:
     void update(const UpdateContext&ctxt)  override
     {
          ComponentManager &components = *ctxt.component;
          EntityManager &entityManager = *ctxt.entity;
          SystemManager &systemManager = *ctxt.system;
          float dt = ctxt.dt;

          for (Entity entity: entities)
          {
               std::cout << "PROJECTILE SYSTEM UPDATE CALL" << std::endl;
               if (components.hasComponent<ProjectileComponent>(entity) && components.hasComponent<Position>(entity))
               {
                    auto &projectile = components.getComponent<ProjectileComponent>(entity);
                    auto &velocity = components.getComponent<Velocity>(entity);
                    auto &pos = components.getComponent<Position>(entity);

                    // May need to look into a better way to handle this
                    // since projectiles should be able along x and y
                    pos.x += velocity.dx * dt;
                    pos.y += velocity.dy * dt;

                    projectile.lifeTime -= dt;

                    if (projectile.lifeTime <= 0.0f)
                    {
                         projectile.isDestroyed = true;
                    }

                    if (projectile.isDestroyed)
                    {
                         std::cout << "DESTROYING PROJECTILE{" << entity << "}";
                         entityManager.destroyEntity(entity);
                         systemManager.entityDestroyed(entity);
                         std::cout << "SUCCESSFUL" << std::endl;
                    }
               }

          }
     }
};
#endif //PROJECTILESYSTEM_H
