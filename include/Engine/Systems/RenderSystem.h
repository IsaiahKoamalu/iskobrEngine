#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "Engine/System.h"
#include "Engine/Components/Position.h"
#include "Engine/ComponentManager.h"
#include "Engine/Components/SpriteComponent.h"
#include <SFML/Graphics.hpp>

#include "Engine/Components/TileComponent.h"

/**
 * System that draws entities with a given Position and SpriteComponent
 **/

// Forward declaration (globally defined?)

class RenderSystem : public System {
public:
 void update(sf::RenderWindow& window, ComponentManager& components) {
  // First draw all tile components
  for (auto entity : entities) {
   if (components.hasComponent<TileComponent>(entity)) {
    auto& tile = components.getComponent<TileComponent>(entity);
    auto& pos = components.getComponent<Position>(entity);
    tile.sprite.setPosition(pos.x, pos.y);
    window.draw(tile.sprite);
   }
  }

  // Then draw all sprite components (like player)
  for (auto entity : entities) {
   if (components.hasComponent<SpriteComponent>(entity) &&
       !components.hasComponent<TileComponent>(entity))  // Don't draw tiles twice
   {
    auto& spriteComp = components.getComponent<SpriteComponent>(entity);
    auto& pos = components.getComponent<Position>(entity);
    spriteComp.sprite.setPosition(pos.x, pos.y);
    window.draw(spriteComp.sprite);
   }
  }

 }
};

#endif