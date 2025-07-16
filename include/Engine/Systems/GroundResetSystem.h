#ifndef GROUNDRESET_H
#define GROUNDRESET_H
#include "Engine/ComponentManager.h"
#include "Engine/System.h"
#include "Engine/Core/UpdateContext.h"
#include "Engine/Components/PlayerComponent.h"

class GroundResetSystem : public System {
public:
    void update(const UpdateContext& ctxt) override{
        ComponentManager& components = *ctxt.component;
        float dt = ctxt.dt;
        for (Entity entity : entities) {
            if (components.hasComponent<PlayerComponent>(entity)) {
                components.getComponent<PlayerComponent>(entity).isGrounded = false;
            }
        }
    }
};

#endif