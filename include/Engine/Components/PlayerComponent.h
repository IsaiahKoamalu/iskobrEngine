#ifndef PLAYERCOMPONENT_H
#define PLAYERCOMPONENT_H


struct PlayerComponent {
    // For jumping mechanic
    bool isGrounded = false;
    bool wasGrounded = false;
    bool landingJustHappened = false;
    float jumpForce = 600.f;
    // For rolling mechanic
    bool isRolling = false;
    float rollTimer = 0.0f;
    float rollDuration = 0.7f;
    float rollSpeed = 300.f;
    float speed = 300.f;
};

#endif