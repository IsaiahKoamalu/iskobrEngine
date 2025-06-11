#ifndef PLAYERINPUTSYSTEM_H
#define PLAYERINPUTSYSTEM_H

#include "Engine/System.h"
#include "Engine/Components/Velocity.h"
#include "Engine/ComponentManager.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

/**
 * System that updates velocity of player-controlled entities
 * based on keyboard input.
 **/



class PlayerInputSystem : public System {
public:
 void update(ComponentManager& components, float dt) {
  // Determine the direction of input.
  float moveX = 0.0f;
  float moveY = 0.0f;

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) moveX -= 1.0f;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) moveX += 1.0f;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) moveY -= 1.0f;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) moveY += 1.0f;

  float speed = 150.0f;

  // Apply velocity to all controlled entities.
  for (Entity entity : entities) {
   auto& velocity = components.getComponent<Velocity>(entity);
   velocity.dx = moveX * speed;
   velocity.dy = moveY * speed;
  }
 }
};

#endif