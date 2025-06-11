#ifndef PLAYERINPUTSYSTEM_H
#define PLAYERINPUTSYSTEM_H

#include "Engine/System.h"
#include "Engine/Components/Velocity.h"
#include "Engine/ComponentManager.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

#include "Engine/Components/AnimationComponent.h"
#include "Engine/Components/SpriteComponent.h"

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

  float speed = 300.0f;

  // Apply velocity to all controlled entities.
  for (Entity entity : entities) {
   if (components.hasComponent<AnimationComponent>(entity)) {
    auto& anim = components.getComponent<AnimationComponent>(entity);
    if (moveY < 0){anim.currentState = "up";}
    else if (moveY > 0){anim.currentState = "down";}
    else if(moveX != 0){anim.currentState = "walk";}
    else {anim.currentState = "idle";}

   }
   auto& velocity = components.getComponent<Velocity>(entity);
   auto& spriteComp = components.getComponent<SpriteComponent>(entity);
   velocity.dx = moveX * speed;
   velocity.dy = moveY * speed;
   if (velocity.dx < 0) {
    spriteComp.flipX = true;
   }else if (velocity.dx > 0) {
    spriteComp.flipX = false;
   }
  }
 }
};

#endif