#ifndef ANIMATIONCOMPONENT_H
#define ANIMATIONCOMPONENT_H

#include <SFML/Graphics.hpp>

/**
 * Store animation data for entity.
 * Use a sprite sheet and cycle through frames horizontallz.
 */
struct AnimationData {
 std::shared_ptr<sf::Texture> texture;
 int frameCount;
 int currentFrame;
 int frameWidth;
 int frameHeight;
 float frameTime;
};

struct AnimationComponent {
 std::unordered_map<std::string, AnimationData> animations;
 std::string currentState = "idle";
 std::string previousState = "idle";

 int currentFrame = 0;
 float elapsedTime = 0.0f;
};

#endif