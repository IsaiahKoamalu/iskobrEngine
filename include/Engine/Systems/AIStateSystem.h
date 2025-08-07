#ifndef AISTATESYSTEM_H
#define AISTATESYSTEM_H

#include "Engine/System.h"
#include "Engine/Components/ActorComponent.h"

class AIStateSystem : public System {
public:
    void update(const UpdateContext& ctxt) override
    {
        auto& components = *ctxt.component;
        auto& dt = ctxt.dt;
        for (Entity entity: entities)
        {
            if (!components.hasComponent<PlayerComponent>(entity) && components.hasComponent<AnimationComponent>(entity))
            {
                auto& anim = components.getComponent<AnimationComponent>(entity);
                auto& dirCom = components.getComponent<DirectionComponent>(entity);
                auto& aiComp = components.getComponent<AIComponent>(entity);
                auto& attackCol = components.getComponent<AttackColliderComponent>(entity);
                auto& actor = components.getComponent<ActorComponent>(entity);

                if (actor.attacking)
                {
                    anim.currentState = "slashLeft";
                    actor.attackTimer -= dt;
                    if (actor.attackTimer <= 0.f)
                    {
                        actor.attacking = false;
                    }
                    if (actor.attackTimer <= 0.01f && dirCom.current == Direction::Right)
                    {
                        attackCol.activeRight = true;
                    }
                    if (actor.attackTimer <= 0.01 && dirCom.current == Direction::Left)
                    {
                        attackCol.activeLeft = true;
                    }
                }
                else
                {
                    //auto& attackCol = components.getComponent<AttackColliderComponent>(entity);
                    attackCol.activeRight = false;
                    attackCol.activeLeft = false;
                }
                if (dirCom.current == Direction::Right && aiComp.state == AIState::Chasing)
                {
                    anim.currentState = "walkRight";
                }
                else if (dirCom.current == Direction::Left && aiComp.state == AIState::Chasing)
                {
                    anim.currentState = "walkLeft";
                }
                else
                {
                    anim.currentState = "idleRight";
                }
            }
        }
    }
};

#endif
