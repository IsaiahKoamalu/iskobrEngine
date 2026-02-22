#ifndef PLAYERCOMPONENT_H
#define PLAYERCOMPONENT_H


struct PlayerComponent {
    // For jumping
    bool isGrounded = false;
    bool wasGrounded = false;
    bool landingJustHappened = false;
    float jumpForce = 600.f;

    // For rolling
    bool isRolling = false;
    float rollTimer = 0.0f;
    float rollDuration = 0.7f;
    float rollSpeed = 300.f;
    float speed = 300.f;

    // For crouching
    bool isCrouching = false;
    float crouchSpeed = 100;

    // For attacking
    bool isSlashing = false;
    bool isShooting = false;
    float slashTimer = 0.0f;
    float slashDuration = 0.4f;
    float shootTimer = 0.0f;
    float shootDuration = 1.0f;


    bool isSmoking;
    float smokingTimer = 0.0f;
    float smokingDuration = 0.4f;

    bool gravityToggle = true;
};

#endif
