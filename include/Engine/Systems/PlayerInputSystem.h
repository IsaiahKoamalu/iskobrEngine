
#ifndef PLAYERINPUTSYSTEM_H
#define PLAYERINPUTSYSTEM_H

#include "Engine/System.h"
#include "Engine/Components/Velocity.h"
#include "Engine/ComponentManager.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

#include "Engine/Components/AnimationComponent.h"
#include "Engine/Components/AttackColliderComponent.h"
#include "Engine/Components/DirectionComponent.h"
#include "Engine/Components/PlayerComponent.h"
#include "Engine/Components/ColliderComponent.h"
#include "Engine/Components/SpriteComponent.h"
#include "Engine/Components/WallClingComponent.h"

/**
 * System that updates velocity of player-controlled entities
 * based on keyboard input.
 **/


class PlayerInputSystem : public System {
public:
    bool wasXPressedLastFrame = false;
    void update(ComponentManager &components, float dt) {
        // Determine the direction of input
        for (Entity entity : entities) {
            if (components.hasComponent<PlayerComponent>(entity)){
                auto& player = components.getComponent<PlayerComponent>(entity);
                auto& cling = components.getComponent<WallClingComponent>(entity);
                auto &velocity = components.getComponent<Velocity>(entity);
                auto& pos = components.getComponent<Position>(entity);
                auto& dir = components.getComponent<DirectionComponent>(entity);

                std::cout << pos.x << ", " << pos.y << std::endl;

                float moveX = 0.0f;

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !cling.active ||
                    sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X) < -20 && !cling.active) moveX -= 1.0f;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && !cling.active ||
                    sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X) > 20 && !cling.active) moveX += 1.0f;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                    pos.x = 100;
                    pos.y = 200;
                }

                bool isXPressed = (sf::Joystick::isButtonPressed(0, 9) || sf::Keyboard::isKeyPressed(sf::Keyboard::C));
                if (isXPressed && !wasXPressedLastFrame) {
                    player.isCrouching = !player.isCrouching;
                }
                wasXPressedLastFrame = isXPressed;
                bool jumpPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) ||
                                    sf::Joystick::isButtonPressed(0, 0));
                bool rollPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ||
                                    sf::Joystick::isButtonPressed(0, 1));
                bool slashPressed = (sf::Joystick::isButtonPressed(0, 5) ||
                                    sf::Mouse::isButtonPressed(sf::Mouse::Left));

                float speed = 300.0f;

                // Handle Jumping
                if (components.hasComponent<PlayerComponent>(entity)) {
                    auto &player = components.getComponent<PlayerComponent>(entity);
                    auto &cling = components.getComponent<WallClingComponent>(entity);
                    auto &attackCol = components.getComponent<AttackColliderComponent>(entity);
                    auto &dir = components.getComponent<DirectionComponent>(entity);

                    if (!player.isSlashing && slashPressed && player.isGrounded && !cling.active) {
                        player.isSlashing = true;
                        player.slashTimer = player.slashDuration;
                    }
                    if (player.isSlashing) {
                        player.slashTimer -= dt;
                        if (player.slashTimer <= 0.f || moveX > 0.f || moveX < 0.f) {
                            player.isSlashing = false;
                        }
                        if (player.slashTimer <= player.slashDuration / 3.f && dir.current == Direction::Right) {
                            attackCol.activeRight = true;
                        }
                        if (player.slashTimer <= player.slashDuration / 2 && dir.current == Direction::Left) {
                            attackCol.activeLeft = true;
                        }
                    }
                    else {
                        auto& attackCol = components.getComponent<AttackColliderComponent>(entity);
                        attackCol.activeRight = false;
                        attackCol.activeLeft = false;
                    }

                    if (!player.isRolling && rollPressed && player.isGrounded) {
                        player.isRolling = true;
                        player.rollTimer = player.rollDuration;
                    }
                    if (player.isRolling) {
                        // Adjusting collision box to account for rolling. (NOTE: be sure to offset the height and top by equal values)
                        auto& colCom = components.getComponent<ColliderComponent>(entity);
                        colCom.bounds.top = -5;
                        colCom.bounds.height = 54;
                        player.rollTimer -= dt;
                        if (player.rollTimer <= 0.0f) {
                            player.isRolling = false;
                            colCom.bounds.top = -36;
                            colCom.bounds.height = 85;
                        }
                        auto &dir = components.getComponent<DirectionComponent>(entity);
                        velocity.dx = (dir.current == Direction::Left ? -1.0f : 1.0f) * player.rollSpeed;
                        speed = player.rollSpeed;
                    }
                    else if (player.isCrouching) {
                        auto& colCom = components.getComponent<ColliderComponent>(entity);
                        colCom.bounds.top = -5;
                        colCom.bounds.height = 54;
                        auto &dir = components.getComponent<DirectionComponent>(entity);
                        velocity.dx = (dir.current == Direction::Left ? -1.0f : 1.0f) * player.crouchSpeed;
                        speed = player.crouchSpeed;
                    }else {
                        auto& colCom = components.getComponent<ColliderComponent>(entity);
                        colCom.bounds.top = -36;
                        colCom.bounds.height = 85;
                    }
                    if (jumpPressed && player.isGrounded || jumpPressed && cling.active) {
                        player.isCrouching = false;
                        cling.active = false;
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
                    else if (player.isCrouching) {
                        if (dir.current == Direction::Right) {
                            if (moveX > 0){anim.currentState = "crouchWalkRight";}
                            else if (moveX < 0){anim.currentState = "crouchWalkLeft";}
                            else{anim.currentState = "crouchIdleRight";}
                        }
                        else if (dir.current == Direction::Left) {
                            anim.currentState = "crouchIdleLeft";
                        }
                    }
                    else if (player.isSlashing) {
                        if (dir.current == Direction::Right) {
                            anim.currentState = "slashRight";
                        }
                        else if (dir.current == Direction::Left) {
                            anim.currentState = "slashLeft";
                        }
                    }
                    else if (!player.isGrounded) {
                        if (moveX > 0 || dir.current == Direction::Right) {
                            dir.current = Direction::Right;
                            anim.currentState = "jumpRight";
                        }
                        if (moveX < 0 || dir.current == Direction::Left) {
                            dir.current = Direction::Left;
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
    }
};

#endif
