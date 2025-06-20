#ifndef PLAYERINPUTSYSTEM_H
#define PLAYERINPUTSYSTEM_H

#include "Engine/System.h"
#include "Engine/Components/Velocity.h"
#include "Engine/ComponentManager.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

#include "Engine/Components/AnimationComponent.h"
#include "Engine/Components/DirectionComponent.h"
#include "Engine/Components/PlayerComponent.h"
#include "Engine/Components/SpriteComponent.h"

/**
 * System that updates velocity of player-controlled entities
 * based on keyboard input.
 **/


class PlayerInputSystem : public System {
public:
    void update(ComponentManager &components, float dt) {
        // Determine the direction of input.
        float moveX = 0.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) moveX -= 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) moveX += 1.0f;

        bool jumpPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
        bool rollPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);
        float speed = 300.0f;

        // Apply velocity to all controlled entities.
        for (Entity entity: entities) {
            // Reset grounded state. It will be set true by collision system if needed

            auto &velocity = components.getComponent<Velocity>(entity);


            // Handle Jumping
            if (components.hasComponent<PlayerComponent>(entity)) {
                auto &player = components.getComponent<PlayerComponent>(entity);
                if (!player.isRolling && rollPressed && player.isGrounded) {
                    player.isRolling = true;
                    player.rollTimer = player.rollDuration;
                    std::cout << "Started Roll\n";
                }
                if (player.isRolling) {
                    player.rollTimer -= dt;
                    if (player.rollTimer <= 0.0f) {
                        player.isRolling = false;
                        std::cout << "Ended Roll\n";
                    }
                    auto &dir = components.getComponent<DirectionComponent>(entity);
                    velocity.dx = (dir.current == Direction::Left ? -1.0f : 1.0f) * player.rollSpeed;
                }
                if (jumpPressed && player.isGrounded) {
                    velocity.dy = -player.jumpForce;
                    player.isGrounded = false;
                }
            }

            // Handle horizontal running movement (if not rolling)
            if (components.hasComponent<PlayerComponent>(entity)) {
                auto& player = components.getComponent<PlayerComponent>(entity);
                if (!player.isRolling) {
                    velocity.dx = moveX * speed;
                }
            }

            // Handle animation updates
            if (components.hasComponent<AnimationComponent>(entity)) {
                auto &dir = components.getComponent<DirectionComponent>(entity);
                auto &anim = components.getComponent<AnimationComponent>(entity);
                auto &player = components.getComponent<PlayerComponent>(entity);

                if (player.isRolling) {
                    if (dir.current == Direction::Right) {
                        anim.currentState = "rollRight";
                    }
                    else if (dir.current == Direction::Left) {
                        anim.currentState = "rollLeft";
                    }
                }
                else if (!player.isGrounded) {
                    if (dir.current == Direction::Right) {
                        anim.currentState = "jumpRight";
                    }
                    else if (dir.current == Direction::Left) {
                        anim.currentState = "jumpLeft";
                    }
                }else if (moveX > 0) {
                    dir.current = Direction::Right;
                    anim.currentState = "walkRight";
                } else if (moveX < 0) {
                    dir.current = Direction::Left;
                    anim.currentState = "walkLeft";
                }else {
                    anim.currentState = (dir.current == Direction::Left) ? "idleLeft" : "idleRight";
                }
                player.wasGrounded = player.isGrounded;
            }
        }
    }
};

#endif
