//
// Created by ikbro on 2/20/2026.
//

#ifndef PROJECTILESYSTEM_H
#define PROJECTILESYSTEM_H

#include "Engine/ComponentManager.h"
#include "Engine/Core/UpdateContext.h"
#include "Engine/System.h"
#include "Engine/Components/ProjectileComponent.h"
#include "Engine/Components/Transform.h"
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
          float dt = ctxt.dt;

          for (Entity entity: entities)
          {
               if (components.hasComponent<ProjectileComponent>(entity)
                    && components.hasComponent<Velocity>(entity)
                    && components.hasComponent<Transform>(entity))
               {
                    auto &projectile = components.getComponent<ProjectileComponent>(entity);
                    auto &velocity = components.getComponent<Velocity>(entity);
                    auto &transform = components.getComponent<Transform>(entity);
               }
          }
     }
};
#endif //PROJECTILESYSTEM_H
