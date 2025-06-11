#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "System.h"
#include "ComponentManager.h"
#include "SpriteComponent.h"
#include <SFML/Graphics.hpp>

/**
 * System that draws entities with a given Position and SpriteComponent
 **/

struct Position; // Forward declaration (globally defined?)

class RenderSystem : public System {
public:
 void update(sf::RenderWindow& window, ComponentManager& components) {
  for (Entity entity : entities) {
   auto& pos = components.getComponent<Position>(entity);
   auto& spriteComp = components.getComponent<SpriteComponent>(entity);

   spriteComp.sprite.setPosition(pos.x, pos.y);
   window.draw(spriteComp.sprite);
  }
 }
};

#endif