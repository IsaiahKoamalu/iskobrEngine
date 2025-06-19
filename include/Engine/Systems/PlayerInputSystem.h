#ifndef PLAYERINPUTSYSTEM_H
#define PLAYERINPUTSYSTEM_H

#include "Engine/System.h"
#include "Engine/Components/Velocity.h"
#include "Engine/ComponentManager.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

#include "Engine/Components/AnimationComponent.h"
#include "Engine/Components/DirectionComponent.h"
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
    auto& dir = components.getComponent<DirectionComponent>(entity);
    auto& anim = components.getComponent<AnimationComponent>(entity);
    if (moveY < 0) {
     dir.current = Direction::Up;
     anim.currentState = "walkUp";
    }else if (moveY > 0) {
     dir.current = Direction::Down;
     anim.currentState = "walkDown";
    }else if(moveX > 0) {
     dir.current = Direction::Right;
     anim.currentState = "walkRight";
    }
    else if (moveX < 0) {
     dir.current = Direction::Left;
     anim.currentState = "walkLeft";
    }else {
     switch (dir.current) {
      case Direction::Up:
       anim.currentState = "idleUp";
       break;
      case Direction::Down:
       anim.currentState = "idleDown";
       break;
      case Direction::Left:
       anim.currentState = "idleLeft";
       break;
      case Direction::Right:
       anim.currentState = "idleRight";
       break;
     }
    }

   }
   auto& velocity = components.getComponent<Velocity>(entity);
   auto& spriteComp = components.getComponent<SpriteComponent>(entity);
   velocity.dx = moveX * speed;
   velocity.dy = moveY * speed;
  }
 }
};

#endif