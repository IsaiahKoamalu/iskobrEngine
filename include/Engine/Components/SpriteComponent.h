#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include <SFML/Graphics.hpp>

/**
 * Component for storing a drawable sprite.
 * Allows for assigning textures before rendering.
 */

struct SpriteComponent {
    sf::Sprite sprite;
    bool flipX = false;
};

#endif
