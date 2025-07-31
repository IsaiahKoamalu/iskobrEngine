#ifndef PHYSICSBODY_H
#define PHYSICSBODY_H

#include "SFML/Graphics.hpp"

struct PhysicsBody {
    sf::Vector2f velocity;
    bool onGround;
};

#endif
