#ifndef PATHCOMPONENT_H
#define PATHCOMPONENT_H

#include <vector>
#include "Engine/Core/TileCoord.h"

enum class StepAction { Walk, Jump, Fall };
struct PathStep {
    TileCoord tile;
    StepAction action;
};

struct PathComponent {
    std::vector<PathStep> steps;
    size_t currentIndex = 0;
};

#endif
