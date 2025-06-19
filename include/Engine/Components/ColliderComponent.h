#ifndef COLLIDERCOMPONENT_H
#define COLLIDERCOMPONENT_H

#include <SFML/Graphics.hpp>

/**
 * A collision component for player, enemies, objects, etc.
 */
struct ColliderComponent {
    sf::FloatRect bounds; // The local bounds
    bool isStatic = false; // Will be true for walls or obstacles.
    bool isTrigger = false; // For sensors (not meant for physical blocking)

    std::string tag = ""; // For debug logic

    bool active = true; // For animation control.
};

#endif
