#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include "Engine/System.h"
#include "Engine/ComponentManager.h"
#include "Engine/Components/AnimationComponent.h"
#include "Engine/Components/SpriteComponent.h"
#include <iostream>

class AnimationSystem : public System {
public:
    void update(ComponentManager &components, float dt) {
        for (Entity entity: entities) {
            auto &animComp = components.getComponent<AnimationComponent>(entity);
            auto &sprite = components.getComponent<SpriteComponent>(entity).sprite;

            // Grab wall cling anim first.
            if (components.hasComponent<WallClingComponent>(entity)) {
                auto& cling = components.getComponent<WallClingComponent>(entity);
                if (cling.active) {
                    animComp.currentState = "wallRight";
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
};

#endif
