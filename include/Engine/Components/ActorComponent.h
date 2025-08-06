#ifndef ACTORCOMPONENT_H
#define ACTORCOMPONENT_H

struct ActorComponent {
    std::string name;

    bool isPlayerControlled = false;

    bool isHostile = false;
    bool canInteract = true;

    bool attacking = false;
};

#endif
