#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "System.h"
#include "ComponentManager.h"

/**
 * A system that moves entities based on their velocity.
 * Requires: Position, Velocity
 */

struct Position {float x, y;};
struct Velocity {float dx, dy;};

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