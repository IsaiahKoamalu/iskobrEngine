#include "Engine/Engine.h"
#include <SFML/Graphics.hpp>
#include <iostream>

#include "Engine/EntityManager.h"
#include "Engine/ComponentManager.h"
#include "Engine/SystemManager.h"
#include "Engine/Components/ColliderComponent.h"
#include "Engine/Systems/ActorSystem.h"
#include "Engine/Systems/CollisionSystem.h"
#include "Engine/Systems/MovementSystem.h"
#include "Engine/Systems/PlayerInputSystem.h"
#include "Engine/Systems/RenderSystem.h"

void Engine::run(bool debugMode) {
    int WINDOW_WIDTH = 800;
    int WINDOW_HEIGHT = 600;
    window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "iskobr-Engine");

    sf::Clock clock;

    //======== ECS SETUP ============
    entityManager = std::make_unique<EntityManager>();
    componentManager = std::make_unique<ComponentManager>();
    systemManager = std::make_unique<SystemManager>();
    tilesetManager = std::make_unique<TilesetManager>();

    // Register systems
    renderSystem = systemManager->registerSystem<RenderSystem>();
    movementSystem = systemManager->registerSystem<MovementSystem>();
    inputSystem = systemManager->registerSystem<PlayerInputSystem>();
    animationSystem = systemManager->registerSystem<AnimationSystem>();
    collisionSystem = systemManager->registerSystem<CollisionSystem>();
    triggerSystem = systemManager->registerSystem<TriggerSystem>();
    cameraSystem = systemManager->registerSystem<CameraSystem>(WINDOW_WIDTH, WINDOW_HEIGHT);
    tileMapSystem = systemManager->registerSystem<TileMapSystem>();
    actorSystem = systemManager->registerSystem<ActorSystem>();


    //Load texture and set up sprite
    auto idleTex = std::make_shared<sf::Texture>();
    idleTex->loadFromFile("assets/player.png");

    auto idleDownTex = std::make_shared<sf::Texture>();
    idleDownTex->loadFromFile("assets/idle_down.png");

    auto idleUpTex = std::make_shared<sf::Texture>();
    idleUpTex->loadFromFile("assets/idle_up.png");

    auto walkTex = std::make_shared<sf::Texture>();
    walkTex->loadFromFile("assets/walk.png");

    auto walkUpTex = std::make_shared<sf::Texture>();
    walkUpTex->loadFromFile("assets/walk_up.png");

    auto walkDownTex = std::make_shared<sf::Texture>();
    walkDownTex->loadFromFile("assets/walk_down.png");

    AnimationComponent anim;
    anim.currentState = "idle";

    anim.animations["idle"] = {
        .texture = idleTex,
        .frameCount = 8,
        .frameWidth = 96,
        .frameHeight = 80,
        .frameTime = 0.2f
    };

    anim.animations["idleDown"] = {
        .texture = idleDownTex,
        .frameCount = 8,
        .frameWidth = 96,
        .frameHeight = 80,
        .frameTime = 0.2f
    };

    anim.animations["idleUp"] = {
        .texture = idleUpTex,
        .frameCount = 8,
        .frameWidth = 96,
        .frameHeight = 80,
        .frameTime = 0.2f
    };

    anim.animations["walk"] = {
        .texture = walkTex,
        .frameCount = 8,
        .frameWidth = 96,
        .frameHeight = 80,
        .frameTime = 0.1f
    };

    anim.animations["up"] = {
        .texture = walkUpTex,
        .frameCount = 8,
        .frameWidth = 96,
        .frameHeight = 80,
        .frameTime = 0.1f
    };

    anim.animations["down"] = {
        .texture = walkDownTex,
        .frameCount = 8,
        .frameWidth = 96,
        .frameHeight = 80,
        .frameTime = 0.1f
    };

    sf::Sprite playerSprite;
    playerSprite.setTexture(*idleTex);
    playerSprite.setScale(3, 3);
    playerSprite.setTextureRect(sf::IntRect(0,0,96,80));
    playerSprite.setOrigin(96 / 2.f, 80 / 2.f);

    Entity npcActor = entityManager->createEntity();
    componentManager->addComponent<ActorComponent>(npcActor, {"npcActor"});
    componentManager->addComponent<Position>(npcActor, {500.f, 30.f});
    componentManager->addComponent<Velocity>(npcActor, {0.f, 100.f});
    componentManager->addComponent<DirectionComponent>(npcActor, {});
    componentManager->addComponent<SpriteComponent>(npcActor, {playerSprite});
    componentManager->addComponent<AnimationComponent>(npcActor, anim);

    actorSystem->entities.insert(npcActor);
    renderSystem->entities.insert(npcActor);
    animationSystem->entities.insert(npcActor);
    movementSystem->entities.insert(npcActor);

    // Create player entity.
    Entity player = entityManager->createEntity();
    componentManager->addComponent<Position>(player, {400.f, 300.f});
    componentManager->addComponent<Velocity>(player, {0.f, 0.f});
    componentManager->addComponent<SpriteComponent>(player, {playerSprite});
    componentManager->addComponent<AnimationComponent>(player,anim);
    componentManager->addComponent<DirectionComponent>(player, {});

    //Player collider
    ColliderComponent playerCollider;
    playerCollider.bounds = sf::FloatRect(-15,38.5,34,20);
    playerCollider.isStatic = false;
    componentManager->addComponent<ColliderComponent>(player, playerCollider);

    // Register player entity with desired systems.
    movementSystem->entities.insert(player);
    renderSystem->entities.insert(player);
    inputSystem->entities.insert(player);
    animationSystem->entities.insert(player);
    collisionSystem->entities.insert(player);
    triggerSystem->entities.insert(player);
    cameraSystem->entities.insert(player);

    tilesetManager->addTileset("grass", "assets/grassSheet.png", 16, 16);
    tilesetManager->addTileset("water", "assets/Water.png", 16, 16);
    tilesetManager->addTileset("dirt", "assets/dirtSheet.png", 16, 16);
    tilesetManager->addTileset("*water", "assets/NCWater.png", 16, 16);
    tileMapSystem->loadMap("assets/maps/level.json", *componentManager, *entityManager, *tilesetManager, *renderSystem, *collisionSystem);

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        processEvents();
        update(dt);
        render(debugMode);
    }
}

void Engine::update(float dt) {
    inputSystem->update(*componentManager, dt);
    movementSystem->update(*componentManager, dt);
    animationSystem->update(*componentManager, dt);
    actorSystem->update(*componentManager, dt);
    collisionSystem->update(*componentManager, dt);
    triggerSystem->update(*componentManager, dt);
    cameraSystem->update(*componentManager, dt);
    window.setView(cameraSystem->view);
}

void Engine::processEvents() {
    sf::Event event;
    while(window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }
}

void Engine::render(bool debugMode) {
    window.clear(sf::Color(155,212,195));
    renderSystem->update(window, *componentManager, debugMode);
    window.display();
}


