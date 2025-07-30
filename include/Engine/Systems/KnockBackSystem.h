#ifndef KNOCKBACKSYSTEM_H
#define KNOCKBACKSYSTEM_H

#include <cmath>

#include "Engine/ComponentManager.h"
#include "Engine/System.h"
#include "Engine/Core/UpdateContext.h"
#include "Engine/Components/KnockBackComponent.h"
#include "Engine/Components/Position.h"

class KnockBackSystem : public System {
public:
    void update(const UpdateContext &ctxt) override {
        ComponentManager &components = *ctxt.component;
        float dt = ctxt.dt;

        for (Entity entity: entities) {
            if (!components.hasComponent<KnockBackComponent>(entity)) continue;

            auto &knock = components.getComponent<KnockBackComponent>(entity);
            if (!knock.isKnockback) continue;

            auto &pos = components.getComponent<Position>(entity);

            pos.x += knock.velocity.x * dt;
            pos.y += knock.velocity.y * dt;

            sf::Vector2f direction = normalize(knock.velocity);
            float decayAmt = knock.decay * dt;

            if (length(knock.velocity) <= decayAmt) {
                knock.velocity = {0.f, 0.f};
                knock.isKnockback = false;
            } else {
                knock.velocity -= direction * decayAmt;
            }
        }
    }
};

#endif
