#ifndef AICOMPONENT_H
#define AICOMPONENT_H

enum class AIState { Idle, Patrolling, Chasing };
struct AIComponent {
    AIState state = AIState::Patrolling;
};

#endif
