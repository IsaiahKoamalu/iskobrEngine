#ifndef TILECOMPONENT_H
#define TILECOMPONENT_H

#include <SFML/Graphics.hpp>
#include "Engine/Core/TileType.h"

struct TileComponent {
    sf::Sprite sprite;
    TileType type = TileType::Empty;
    bool isSolid = false;
    bool isAnimated = false;
    int tileID = -1; // For raw ID tracking.
};

#endif
