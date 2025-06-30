#ifndef HEALTHCOMPONENT_H
#define HEALTHCOMPONENT_H

struct HealthComponent {
    float health = 100.f;
    bool isFull = true;
    bool isLow = false;
    bool isDead = false;
};

#endif