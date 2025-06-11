#ifndef ANIMATIONCOMPONENT_H
#define ANIMATIONCOMPONENT_H

#include <SFML/Graphics.hpp>

/**
 * Store animation data for entity.
 * Use a sprite sheet and cycle through frames horizontallz.
 */
struct AnimationComponent {
 int frameCount;
 int currentFrame = 0;
 float frameTime = 0.1f;
 float elapsedTime = 0.0f;
 int frameWidth;
 int frameHeight;
};

#endif