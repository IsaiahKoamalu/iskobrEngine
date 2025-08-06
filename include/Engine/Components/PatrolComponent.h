#ifndef PATROLCOMPONENT_H
#define PATROLCOMPONENT_H

#include <vector>
#include "Engine/Core/TileCoord.h"

struct PatrolComponent {
    std::vector<sf::Vector2f> waypoints;
    std::size_t currentIndex = 0;
    bool needsNewWaypoint = true;
    float speed = 50.f;
    float dwellTime = 1.0f;
    float dwellTimer = 0.0f;
};

#endif
