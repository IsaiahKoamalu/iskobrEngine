#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "Engine/System.h"
#include "Engine/ComponentManager.h"
#include "Engine/Components/Position.h"
#include "Engine/Components/Velocity.h"



/**
 * System which moves entities by applying velocity to position at each frame.
 * Requires: Position, Velocity.
 */
class MovementSystem : public System {
public:
 void update(ComponentManager& components, float dt) {
  for (Entity entity : entities) {
   auto& pos = components.getComponent<Position>(entity);
   auto& vel = components.getComponent<Velocity>(entity);

   pos.x += vel.dx * dt;
   pos.y += vel.dy * dt;
  }
 }
};

#endif