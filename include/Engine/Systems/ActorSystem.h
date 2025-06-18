#ifndef ACTORSYSTEM_H
#define ACTORSYSTEM_H

#include "Engine/System.h"
#include "Engine/ComponentManager.h"
#include "Engine/EntityManager.h"
#include "Engine/Components/ActorComponent.h"

class ActorSystem : public System {
public:
    void update(ComponentManager& componentManager, float dt) {
        for (Entity entity : entities) {
            auto& actorComp = componentManager.getComponent<ActorComponent>(entity);
            auto& dir = componentManager.getComponent<DirectionComponent>(entity);
            auto& anim = componentManager.getComponent<AnimationComponent>(entity);
            auto& vel = componentManager.getComponent<Velocity>(entity);

            if (vel.dy > 50) {
                dir.current = Direction::Down;
                anim.currentState = "down";
            }
        }
    }
};


#endif
