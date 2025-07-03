#ifndef ATTACKCOLLIDERCOMPONENT_H
#define ATTACKCOLLIDERCOMPONENT_H

#include <sfml/Graphics.hpp>

/**
 * A collision component for attacks.
 */
struct AttackColliderComponent {
    sf::FloatRect bounds;
    bool isStatic = false;
    bool isTrigger = false;
    std::string tag = "";

    bool active = false;
};

#endif
