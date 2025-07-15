#ifndef SYSTEM_H
#define SYSTEM_H

#include <cmath>
#include <set>
#include <cstdint>

/**
 *@brief Represents a system that runs logic on entities.
 *       Wherein a derived system defines its own componet filter and behavior.
 */

using Entity = std::uint32_t;

class System {
public:

    // Helper function
    float length(const sf::Vector2f v) {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }
    //Helper function
    sf::Vector2f normalize(const sf::Vector2f v) {
        float length = std::sqrt(v.x * v.x + v.y * v.y);
        return length != 0 ? v / length : sf::Vector2f(0.f, 0.f);
    }

    std::set<Entity> entities;

    virtual ~System() = default;
};

#endif
