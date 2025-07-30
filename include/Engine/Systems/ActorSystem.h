#ifndef ACTORSYSTEM_H
#define ACTORSYSTEM_H

#include "Engine/System.h"
#include "Engine/Core/UpdateContext.h"
#include "Engine/ComponentManager.h"
#include "Engine/EntityManager.h"
#include "Engine/Components/ActorComponent.h"

class ActorSystem : public System {
public:
    void update(const UpdateContext &ctxt) override {
        ComponentManager &component = *ctxt.component;
        float dt = ctxt.dt;
        for (Entity entity: entities) {
            auto &actorComp = component.getComponent<ActorComponent>(entity);
            auto &dir = component.getComponent<DirectionComponent>(entity);
            auto &anim = component.getComponent<AnimationComponent>(entity);
            auto &vel = component.getComponent<Velocity>(entity);
            auto &pos = component.getComponent<Position>(entity);

            if (pos.y < 500) {
                vel.dy = 100.f;
                dir.current = Direction::Down;
                anim.currentState = "walkDown";
            } else if (pos.y > 505) {
                vel.dy = -100.f;
                dir.current = Direction::Up;
                anim.currentState = "walkUp";
            } else if (pos.x < 800) {
                vel.dx = 100.f;
                dir.current = Direction::Right;
                anim.currentState = "walkRight";
            } else if (pos.x > 805) {
                vel.dx = -100.f;
                dir.current = Direction::Left;
                anim.currentState = "walkLeft";
            } else {
                vel.dy = 0.f;
                vel.dx = 0.f;
                if (dir.current == Direction::Down) anim.currentState = "idleDown";
                if (dir.current == Direction::Up) anim.currentState = "idleUp";
                if (dir.current == Direction::Right) anim.currentState = "idleRight";
                if (dir.current == Direction::Left) anim.currentState = "idleLeft";
            }
        }
    }
};


#endif
