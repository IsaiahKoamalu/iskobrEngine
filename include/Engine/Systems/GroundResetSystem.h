#ifndef GROUNDRESET_H
#define GROUNDRESET_H
#include "Engine/ComponentManager.h"
#include "Engine/System.h"
#include "Engine/Components/PlayerComponent.h"

class GroundResetSystem : public System {
public:
    void update(ComponentManager &components, float dt) {
        for (Entity entity : entities) {
            if (components.hasComponent<PlayerComponent>(entity)) {
                components.getComponent<PlayerComponent>(entity).isGrounded = false;
            }
        }
    }
};

#endif