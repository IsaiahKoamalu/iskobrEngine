#ifndef TILECOMPONENT_H
#define TILECOMPONENT_H

#include <SFML/Graphics.hpp>

struct TileComponent {
    int tileID = 0; // Tile identifier
    sf::Sprite sprite;
    bool isSolid = false; // Will block player movement if true
};

#endif