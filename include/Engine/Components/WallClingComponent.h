#ifndef WALLCLINGCOMPONENT_H
#define WALLCLINGCOMPONENT_H

#include "glm/vec2.hpp"
#include "glm/geometric.hpp"

struct WallClingComponent {
    bool active{false};
    bool touchedThisFrame{false};
    float timer{0.0f}; //Times the seconds spent on wall
    glm::vec2 wallNormal{0.0f, 0.0f}; //A unit vector that points out of the wall

    float maxTime{1.5f};
    float slideSpeed{0.0f};
    float jumpImpulse{350.0f};
};

#endif
