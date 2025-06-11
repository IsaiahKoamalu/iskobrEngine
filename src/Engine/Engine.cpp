#include "Engine/Engine.h"
#include <SFML/Graphics.hpp>
#include <iostream>

#include "Engine/EntityManager.h"
#include "Engine/ComponentManager.h"
#include "Engine/SystemManager.h"
#include "Engine/MovementSystem.h"
#include "Engine/RenderSystem.h"

void Engine::run() {
    window.create(sf::VideoMode(800, 600), "iskobr-Engine");

    sf::Clock clock;

    //======== ECS SETUP ============
    entityManager = std::make_unique<EntityManager>();
    componentManager = std::make_unique<ComponentManager>();
    syatemManager = std::make_unique<SystemManager>();

    // Register render system
    auto renderSystem = syatemManager->registerSystem<RenderSystem>();

    // Register and get a movement system.
    auto movementSystem = syatemManager->registerSystem<MovementSystem>();

    //Load texture and set up sprite
    playerTexture = std::make_unique<sf::Texture>();
    if (!playerTexture->loadFromFile("assets/player.png")) {
        std::cerr << "Failed to load texture\n";
        return;
    }

    sf::Sprite playerSprite;
    playerSprite.setTexture(*playerTexture);
    playerSprite.setScale(3, 3);

    // Create renderable entity
    Entity player = entityManager->createEntity();
    componentManager->addComponent<Position>(player, {100.f, 100.f});
    componentManager->addComponent<Velocity>(player, {30.f, 30.f});
    componentManager->addComponent<SpriteComponent>(player, {playerSprite});
    movementSystem->entities.insert(player);
    renderSystem->entities.insert(player);

    // Visual for player
    sf::CircleShape playerShape(20);
    playerShape.setFillColor(sf::Color::Red);


    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        processEvents();
        movementSystem->update(*componentManager, dt);
        window.clear(sf::Color::Black);
        renderSystem->update(window, *componentManager);

        // utilizing position from ECS to place player sprite.
        if (componentManager->hasComponent<Position>(player)) {
            auto& pos = componentManager->getComponent<Position>(player);
            playerShape.setPosition(pos.x, pos.y);
            //window.draw(playerShape);
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
