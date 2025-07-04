#ifndef ATTACKCOLLIDERCOMPONENT_H
#define ATTACKCOLLIDERCOMPONENT_H

#include <sfml/Graphics.hpp>

/**
 * A collision component for attacks.
 */
struct AttackColliderComponent {
    sf::FloatRect boundsRight;
    sf::FloatRect boundsLeft;
    bool isStatic = false;
    bool isTrigger = false;
    std::string tag = "";

    bool activeRight = false;
    bool activeLeft = false;
};

#endif
