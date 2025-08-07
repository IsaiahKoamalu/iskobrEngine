#ifndef ACTORCOMPONENT_H
#define ACTORCOMPONENT_H

struct ActorComponent
{
    std::string name;

    bool isPlayerControlled = false;

    bool isHostile = true;
    bool canInteract = true;

    bool attacking = false;
    float attackTimer = 0.4f;
    float attackDuration = 0.4f;
};

#endif
