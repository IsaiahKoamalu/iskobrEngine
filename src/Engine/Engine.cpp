#include "Engine/Engine.h"
#include <SFML/Graphics.hpp>
#include <iostream>

#include "Engine/EntityManager.h"
#include "Engine/ComponentManager.h"
#include "Engine/SystemManager.h"
#include "Engine/Systems/MovementSystem.h"
#include "Engine/Systems/PlayerInputSystem.h"
#include "Engine/Systems/RenderSystem.h"

void Engine::run() {
    window.create(sf::VideoMode(800, 600), "iskobr-Engine");

    sf::Clock clock;

    //======== ECS SETUP ============
    entityManager = std::make_unique<EntityManager>();
    componentManager = std::make_unique<ComponentManager>();
    systemManager = std::make_unique<SystemManager>();

    // Register systems
     renderSystem = systemManager->registerSystem<RenderSystem>();
     movementSystem = systemManager->registerSystem<MovementSystem>();
     inputSystem = systemManager->registerSystem<PlayerInputSystem>();
     animationSystem = systemManager->registerSystem<AnimationSystem>();

    //Load texture and set up sprite
    playerTexture = std::make_unique<sf::Texture>();
    if (!playerTexture->loadFromFile("assets/player.png")) {
        std::cerr << "Failed to load texture\n";
        return;
    }

    sf::Sprite playerSprite;
    playerSprite.setTexture(*playerTexture);
    playerSprite.setScale(3, 3);
    playerSprite.setTextureRect(sf::IntRect(0,0,96,96));

    // Create player entity.
    Entity player = entityManager->createEntity();
    componentManager->addComponent<Position>(player, {100.f, 100.f});
    componentManager->addComponent<Velocity>(player, {0.f, 0.f});
    componentManager->addComponent<SpriteComponent>(player, {playerSprite});
    componentManager->addComponent<AnimationComponent>(player, {
        .frameCount = 10,
        .frameTime = 0.15f,
        .frameWidth = 96,
        .frameHeight = 96
    });

    // Register player entity with desired systems.
    movementSystem->entities.insert(player);
    renderSystem->entities.insert(player);
    inputSystem->entities.insert(player);
    animationSystem->entities.insert(player);

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        processEvents();
        update(dt);
        render();
    }
}

void Engine::update(float dt) {
    inputSystem->update(*componentManager, dt);
    movementSystem->update(*componentManager, dt);
    animationSystem->update(*componentManager, dt);
}

void Engine::processEvents() {
    sf::Event event;
    while(window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }
}

void Engine::render() {
    window.clear(sf::Color::Black);
    renderSystem->update(window, *componentManager);
    window.display();
}


