#ifndef KNOCKBACKCOMPONENT_H
#define KNOCKBACKCOMPONENT_H
#include "SFML/System/Vector2.hpp"

struct KnockBackComponent {
    sf::Vector2f velocity;
    float force = 200.f;
    float decay = 1000.f;
    bool isKnockback = false;
};

#endif