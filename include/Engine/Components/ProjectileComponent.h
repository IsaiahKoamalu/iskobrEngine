//
// Created by ikbro on 2/20/2026.
//

#include <iostream>

#ifndef PROJECTILECOMPONENT_H
#define PROJECTILECOMPONENT_H

struct ProjectileComponent
{
    float lifeTime;
    std::string type;
    bool isDestroyed = false;
};

#endif //PROJECTILECOMPONENT_H
