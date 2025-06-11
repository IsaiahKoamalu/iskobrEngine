#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include "Engine/System.h"
#include "Engine/ComponentManager.h"
#include "Engine/Components/AnimationComponent.h"
#include "Engine/Components/SpriteComponent.h"

class AnimationSystem : public System {
public:
    void update(ComponentManager& components, float dt) {
        for (Entity entity : entities) {
            auto& anim = components.getComponent<AnimationComponent>(entity);
            auto& sprite = components.getComponent<SpriteComponent>(entity).sprite;

            anim.elapsedTime += dt;

            if (anim.elapsedTime >= anim.frameTime) {
                anim.elapsedTime = 0.0f;
                anim.currentFrame = (anim.currentFrame + 1) % anim.frameCount;

                // Adjust texture rect to show the current frame/
                sprite.setTextureRect(sf::IntRect(
                    anim.currentFrame * anim.frameWidth,
                    0,
                    anim.frameWidth,
                    anim.frameHeight
                ));
            }
        }
    }
};

#endif