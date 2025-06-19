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
    auto idleRightTex = std::make_shared<sf::Texture>();
    idleRightTex->loadFromFile("assets/idle_right.png");

    auto idleLeftTex = std::make_shared<sf::Texture>();
    idleLeftTex->loadFromFile("assets/idle_left.png");

    auto idleDownTex = std::make_shared<sf::Texture>();
    idleDownTex->loadFromFile("assets/idle_down.png");

    auto idleUpTex = std::make_shared<sf::Texture>();
    idleUpTex->loadFromFile("assets/idle_up.png");

    auto walkRightTex = std::make_shared<sf::Texture>();
    walkRightTex->loadFromFile("assets/walk_right.png");

    auto walkLeftTex = std::make_shared<sf::Texture>();
    walkLeftTex->loadFromFile("assets/walk_left.png");

    auto walkUpTex = std::make_shared<sf::Texture>();
    walkUpTex->loadFromFile("assets/walk_up.png");

    auto walkDownTex = std::make_shared<sf::Texture>();
    walkDownTex->loadFromFile("assets/walk_down.png");

    AnimationComponent anim;
    anim.currentState = "idleRight";

    anim.animations["idleRight"] = {
        .texture = idleRightTex,
        .frameCount = 8,
        .frameWidth = 96,
        .frameHeight = 80,
        .frameTime = 0.2f
    };

    anim.animations["idleLeft"] = {
        .texture = idleLeftTex,
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

    anim.animations["walkRight"] = {
        .texture = walkRightTex,
        .frameCount = 8,
        .frameWidth = 96,
        .frameHeight = 80,
        .frameTime = 0.1f
    };

    anim.animations["walkLeft"] = {
        .texture = walkLeftTex,
        .frameCount = 8,
        .frameWidth = 96,
        .frameHeight = 80,
        .frameTime = 0.1f
    };

    anim.animations["walkUp"] = {
        .texture = walkUpTex,
        .frameCount = 8,
        .frameWidth = 96,
        .frameHeight = 80,
        .frameTime = 0.1f
    };

    anim.animations["walkDown"] = {
        .texture = walkDownTex,
        .frameCount = 8,
        .frameWidth = 96,
        .frameHeight = 80,
        .frameTime = 0.1f
    };

    sf::Sprite playerSprite;
    playerSprite.setTexture(*idleRightTex);
    playerSprite.setScale(3, 3);
    playerSprite.setTextureRect(sf::IntRect(0,0,96,80));
    playerSprite.setOrigin(96 / 2.f, 80 / 2.f);

    // Base collider
    ColliderComponent baseCollider;
    baseCollider.bounds = sf::FloatRect(-15,38.5,34,20);
    baseCollider.isStatic = false;

    // Creating Entities

    Entity npcActor = entityManager->createEntity();
    componentManager->addComponent<ActorComponent>(npcActor, {"npcActor"});
    componentManager->addComponent<Position>(npcActor, {500.f, 30.f});
    componentManager->addComponent<Velocity>(npcActor, {0.f, 0.f});
    componentManager->addComponent<DirectionComponent>(npcActor, {});
    componentManager->addComponent<SpriteComponent>(npcActor, {playerSprite});
    componentManager->addComponent<AnimationComponent>(npcActor, anim);
    componentManager->addComponent<ColliderComponent>(npcActor, baseCollider);

    //actorSystem->entities.insert(npcActor);
    renderSystem->entities.insert(npcActor);
    //animationSystem->entities.insert(npcActor);
    //collisionSystem->entities.insert(npcActor);
    //movementSystem->entities.insert(npcActor);

    // Create player entity.
    Entity player = entityManager->createEntity();
    componentManager->addComponent<Position>(player, {400.f, 300.f});
    componentManager->addComponent<Velocity>(player, {0.f, 0.f});
    componentManager->addComponent<SpriteComponent>(player, {playerSprite});
    componentManager->addComponent<AnimationComponent>(player,anim);
    componentManager->addComponent<DirectionComponent>(player, {});
    componentManager->addComponent<ColliderComponent>(player, baseCollider);

    // Register player entity with desired systems.
    movementSystem->entities.insert(player);
    renderSystem->entities.insert(player);
    inputSystem->entities.insert(player);
    animationSystem->entities.insert(player);
    collisionSystem->entities.insert(player);
    triggerSystem->entities.insert(player);
    cameraSystem->entities.insert(player);

    auto entityFile = std::make_shared<std::string>("assets/entities.json");
    if (!loadEntities(*entityFile)) {
        std::cerr << "PROBLEM LOADING ENTITIES" << std::endl;
    };

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

bool Engine::loadEntities(std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open entity file: " << filepath << std::endl;
        return false;
    }
    nlohmann::json jsonArray;
    file >> jsonArray;

    for (const auto& j : jsonArray) {
        Entity entity = entityManager->createEntity();

        std::string startAnim = j["anim"]["start"];
        std::shared_ptr<sf::Texture> entityTexture;
        if (j.contains("anim")) {
            AnimationComponent anim;
            anim.currentState = startAnim;
            for (auto it = j["spritePath"].begin(); it != j["spritePath"].end(); ++it) {
                auto tex = std::make_shared<sf::Texture>();
                if (!tex->loadFromFile(it.value()["filePath"])) {
                    std::cerr << "Failed to load: " << it.value()["filePath"] << std::endl;
                    continue;
                }

                anim.animations[it.key()] = {
                    .texture = tex,
                    .frameCount = j["anim"]["frameCount"],
                    .frameWidth = j["anim"]["frameWidth"],
                    .frameHeight = j["anim"]["frameHeight"],
                    .frameTime = j["anim"]["frameTime"]
                };

                if (it.key() == startAnim) {
                    entityTexture = tex;
                }
            }
            componentManager->addComponent<AnimationComponent>(entity, {anim});
            std::cout << "Added AnimationComponent for entity with states:\n";
            for (const auto& [k, _] : anim.animations)
                std::cout << " - " << k << '\n';
            animationSystem->entities.insert(entity);
        }
        if (entityTexture) {
            sf::Sprite entitySprite;
            entitySprite.setTexture(*entityTexture);
            entitySprite.setScale(3, 3);
            entitySprite.setTextureRect(sf::IntRect(0, 0, j["anim"]["frameWidth"], j["anim"]["frameHeight"]));
            float originX = j["anim"]["frameWidth"].get<float>() / 2.f;
            float originY = j["anim"]["frameHeight"].get<float>() / 2.f;
            entitySprite.setOrigin(originX, originY);

            componentManager->addComponent<SpriteComponent>(entity, {entitySprite});
        }

        if (j.contains("sprite") && j["sprite"] == true) {
            std::string startAnim = j["anim"]["start"];
            auto entityTexture = std::make_shared<sf::Texture>();
            entityTexture->loadFromFile(j["spritePath"][startAnim]["filePath"]);

            sf::Sprite entitySprite;
            entitySprite.setTexture(*entityTexture);
            entitySprite.setScale(3, 3);
            entitySprite.setTextureRect(sf::IntRect(0,0,48,64));
            entitySprite.setOrigin(48 / 2.f, 64 / 2.f);
        }
        if (j.contains("actor")) {
            componentManager->addComponent<ActorComponent>(entity, {j["actor"]["name"]});
            actorSystem->entities.insert(entity);
        }
        if (j.contains("Position")) {
            componentManager->addComponent<Position>(entity, {j["Position"]["x"], j["Position"]["y"]});
        };
        if (j.contains("Velocity")) {
            componentManager->addComponent<Velocity>(entity, {j["Velocity"]["dx"], j["Velocity"]["dy"]});
        }
        if (j.contains("Direction") && j["Direction"] == true) {
            componentManager->addComponent<DirectionComponent>(entity, {});
        }
        if (j.contains("Collision")) {
            ColliderComponent colCom;
            colCom.bounds = sf::FloatRect(j["Collision"]["rectLeft"], j["Collision"]["rectTop"], j["Collision"]["rectWidth"], j["Collision"]["rectHeight"]);
            colCom.isStatic = j["Collision"]["isStatic"];

            componentManager->addComponent<ColliderComponent>(entity, colCom);
            collisionSystem->entities.insert(entity);
        }

        if (j.contains("Movement")) {
            movementSystem->entities.insert(entity);
        }

        renderSystem->entities.insert(entity);
    }

    return true;
}



