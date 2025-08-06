#ifndef AISystem_H
#define AISystem_H

#include "Engine/System.h"
#include "Engine/Core/UpdateContext.h"
#include "Engine/Components/AIComponent.h"
#include "Engine/Components/SeekComponent.h"
#include "Engine/Components/PatrolComponent.h"
#include "Engine/Components/PlayerComponent.h"
#include "Engine/Components/Position.h"
#include "Engine/Components/Velocity.h"
#include <cmath>

class AISystem : public System {
public:
    void update(const UpdateContext& ctxt) override {
        auto& components = *ctxt.component;
        auto& dt = ctxt.dt;

        sf::Vector2f playerPos{0,0};
        for (Entity entity : entities)
        {
            if (components.hasComponent<PlayerComponent>(entity))
            {
                auto& p = components.getComponent<Position>(entity);
                playerPos = {p.x, p.y};
                break;
            }
        }

        for (Entity entity : entities) {
            if (!components.hasComponent<AIComponent>(entity)) continue;

            auto& aiComp = components.getComponent<AIComponent>(entity);
            auto& pos = components.getComponent<Position>(entity);
            auto& vel = components.getComponent<Velocity>(entity);
            auto& dirCom = components.getComponent<DirectionComponent>(entity);

            if (aiComp.state == AIState::Idle)
            {
                vel.dx = 0.f;
            }
            else if (aiComp.state == AIState::Patrolling)
            {
                float dx = playerPos.x - pos.x;
                float dir = (dx > 0.f) ? 1.f : (dx < 0.f ? -1.f : 0.f);
                if (dir > 0.f)
                {
                    dirCom.current = Direction::Right;
                }
                else if (dir < 0.f)
                {
                    dirCom.current = Direction::Left;
                }
                vel.dx = dir * 50.f;
            }
            else
            {
                vel.dx = 0.f;
            }
        }
    }
private:
    sf::Vector2f targetPos;
};

#endif
