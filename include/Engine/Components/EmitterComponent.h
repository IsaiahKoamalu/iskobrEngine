#ifndef EMITTERCOMPONENT_H
#define EMITTERCOMPONENT_H

#include <iostream>

struct EmitterComponent {
    std::string type;
    int amount;
    bool active = false;
};

#endif