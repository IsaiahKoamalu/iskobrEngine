#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include "Engine/System.h"
#include "Engine/Core/UpdateContext.h"
#include "Engine/ComponentManager.h"
#include "Engine/Components/AnimationComponent.h"
#include "Engine/Components/SpriteComponent.h"
#include <iostream>

class AnimationSystem : public System {
public:
    void update(const UpdateContext& ctxt) override{
        ComponentManager& components = *ctxt.component;
        float dt = ctxt.dt;
        for (Entity entity: entities) {
            if (components.hasComponent<AnimationComponent>(entity) && components.hasComponent<SpriteComponent>(entity)) {
                auto &animComp = components.getComponent<AnimationComponent>(entity);
                auto &sprite = components.getComponent<SpriteComponent>(entity).sprite;

                if (components.hasComponent<KnockBackComponent>(entity) && !components.hasComponent<PlayerComponent>(entity)) {
                    auto& knock = components.getComponent<KnockBackComponent>(entity);
                    if(knock.isKnockback) {
                        animComp.currentState = "hurt";
                    }else {
                        animComp.currentState = "idleLeft";
                    }
                }

                // Grab wall cling anim first.
                if (components.hasComponent<WallClingComponent>(entity)) {
                    auto& cling = components.getComponent<WallClingComponent>(entity);
                    auto& dir = components.getComponent<DirectionComponent>(entity);
                    if (cling.active) {
                        if (animComp.currentState == "jumpLeft") {
                            animComp.currentState = "wallLeft";
                        }else if (animComp.currentState == "jumpRight") {
                            animComp.currentState = "wallRight";
                        }
                    }
                }

                // Get the current state's animation data
                auto it = animComp.animations.find(animComp.currentState);
                if (it == animComp.animations.end()) {
                    std::cerr << "Missing animation state: " << animComp.currentState << "\n";
                    continue;
                }

                AnimationData &animData = it->second;

                // If state changed, reset frame and update texture
                if (animComp.currentState != animComp.previousState) {
                    animComp.previousState = animComp.currentState;
                    animComp.currentFrame = 0;
                    animComp.elapsedTime = 0.0f;

                    sprite.setTexture(*animData.texture);
                }

                // Advance animation based on time
                animComp.elapsedTime += dt;
                if (animComp.elapsedTime >= animData.frameTime) {
                    animComp.elapsedTime = 0.0f;
                    animComp.currentFrame = (animComp.currentFrame + 1) % animData.frameCount;
                }

                sprite.setTexture(*animData.texture);
                // Update the sprite's texture rectangle
                sprite.setTextureRect(sf::IntRect(
                    animComp.currentFrame * animData.frameWidth,
                    0, // Assumes single-row sprite sheets
                    animData.frameWidth,
                    animData.frameHeight
                ));
            }
        }
    }
};

#endif
