#ifndef NAVIGATIONCOMPONENT_H
#define NAVIGATIONCOMPONENT_H

#include <Engine/Core/TileCoord.h>

struct NavigationComponent {
    TileCoord targetTile;
    bool needsRepath = true;
};

#endif