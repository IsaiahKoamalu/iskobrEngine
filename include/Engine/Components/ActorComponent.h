#ifndef ACTORCOMPONENT_H
#define ACTORCOMPONENT_H

#include <iostream>

struct ActorComponent {
    std::string name;
    enum class Behavior {Idle, Follow};
    Behavior behavior = Behavior::Idle;

    float moveSpeed = 1.0f;
    bool isPlayerControlled = false;

    bool isHostile = false;
    bool canInteract = true;
};

#endif