#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "Engine/System.h"
#include "Engine/Components/Position.h"
#include "Engine/ComponentManager.h"
#include "Engine/Components/SpriteComponent.h"
#include <SFML/Graphics.hpp>

/**
 * System that draws entities with a given Position and SpriteComponent
 **/

// Forward declaration (globally defined?)

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