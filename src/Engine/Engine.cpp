#include "Engine/Engine.h"
#include <SFML/Graphics.hpp>
#include <iostream>

#include "Engine/EntityManager.h"
#include "Engine/ComponentManager.h"
#include "Engine/SystemManager.h"
#include "Engine/MovementSystem.h"

void Engine::run() {
    window.create(sf::VideoMode(800, 600), "iskobr-Engine");

    if (!playerTexture.loadFromFile("assets/player.png")) {
        std::cerr << "Failed to load player.png\n";
        return;
    }

    sf::Clock clock;

    //======== ECS SETUP ============
    entityManager = std::make_unique<EntityManager>();
    componentManager = std::make_unique<ComponentManager>();
    syatemManager = std::make_unique<SystemManager>();

    // Register and get a movement system.
    auto movementSystem = syatemManager->registerSystem<MovementSystem>();

    // Test entity with position and velocity.
    Entity player = entityManager->createEntity();
    componentManager->addComponent<Position>(player, {100.f, 200.f});
    componentManager->addComponent<Velocity>(player, {50.f, 30.f});
    movementSystem->entities.insert(player);

    // Visual for player
    sf::CircleShape playerShape(20);
    playerShape.setFillColor(sf::Color::Red);


    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        processEvents();
        movementSystem->update(*componentManager, dt);
        window.clear(sf::Color::Black);

        // utilizing position from ECS to place player sprite.
        if (componentManager->hasComponent<Position>(player)) {
            auto& pos = componentManager->getComponent<Position>(player);
            playerShape.setPosition(pos.x, pos.y);
            window.draw(playerShape);
        }

        window.display();
    }
}

void Engine::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        input.handleEvent(event);
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }
}

void Engine::update(float dt) {
    if (input.isKeyPressed(sf::Keyboard::A)) {
        playerSprite.move(-200 * dt, 0);
    }
    if (input.isKeyPressed(sf::Keyboard::D)) {
        playerSprite.move(200 * dt, 0);
    }
    if (input.isKeyPressed(sf::Keyboard::W)) {
        playerSprite.move(0, -200 * dt);
    }
    if (input.isKeyPressed(sf::Keyboard::S)) {
        playerSprite.move(0, 200 * dt);
    }
}

void Engine::render() {
    window.clear(sf::Color::Black);
    window.draw(playerSprite);
    window.display();
}
