#include "Engine/Engine.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>  // Ensure included for std::ifstream
#include <nlohmann/json.hpp>  // Assuming nlohmann::json is used, include it explicitly if not in headers

#include "Engine/EntityManager.h"
#include "Engine/ComponentManager.h"
#include "Engine/SystemManager.h"
#include "Engine/Components/ColliderComponent.h"
#include "Engine/Components/AttackColliderComponent.h"
#include "Engine/Components/HealthComponent.h"
#include "Engine/Components/PlayerComponent.h"
#include "Engine/Systems/ActorSystem.h"
#include "Engine/Systems/CollisionSystem.h"
#include "Engine/Systems/MovementSystem.h"
#include "Engine/Systems/PlayerInputSystem.h"
#include "Engine/Systems/RenderSystem.h"
// Include other necessary headers for systems and components mentioned in the code

void Engine::run(bool debugMode) {
    const int WINDOW_WIDTH = 800;  // Consider making configurable via config file
    const int WINDOW_HEIGHT = 600;
    window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "iskobr-Engine");

    sf::Clock clock;

    //======== ECS SETUP ============
    entityManager = std::make_unique<EntityManager>();
    componentManager = std::make_unique<ComponentManager>();
    systemManager = std::make_unique<SystemManager>();
    tilesetManager = std::make_unique<TilesetManager>();

    // Register systems (ensure all used systems are registered)
    renderSystem = systemManager->registerSystem<RenderSystem>();
    movementSystem = systemManager->registerSystem<MovementSystem>();
    inputSystem = systemManager->registerSystem<PlayerInputSystem>();
    animationSystem = systemManager->registerSystem<AnimationSystem>();
    collisionSystem = systemManager->registerSystem<CollisionSystem>();
    triggerSystem = systemManager->registerSystem<TriggerSystem>();
    cameraSystem = systemManager->registerSystem<CameraSystem>(WINDOW_WIDTH, WINDOW_HEIGHT);
    tileMapSystem = systemManager->registerSystem<TileMapSystem>();
    actorSystem = systemManager->registerSystem<ActorSystem>();
    physicsSystem = systemManager->registerSystem<PhysicsSystem>();
    groundResetSystem = systemManager->registerSystem<GroundResetSystem>();
    damageSystem = systemManager->registerSystem<DamageSystem>();
    particleSystem = systemManager->registerSystem<ParticleSystem>();

    // Pass collision system to particle system
    particleSystem->setCollisionSystem(collisionSystem.get());

    std::string entityFilePath = "assets/entities.json";
    if (!loadEntities(entityFilePath)) {
        std::cerr << "Failed to load entities. Exiting." << std::endl;
        return;  // Exit early on failure
    }

    // Load tilesets (consider loading from a config JSON for flexibility)
    tilesetManager->addTileset("grass", "assets/grassSheet.png", 16, 16);
    tilesetManager->addTileset("water", "assets/Water.png", 16, 16);
    tilesetManager->addTileset("dirt", "assets/dirtSheet.png", 16, 16);
    tilesetManager->addTileset("*water", "assets/NCWater.png", 16, 16);

    // Load map
    tileMapSystem->loadMap("assets/maps/level.json", *componentManager, *entityManager, *tilesetManager, *renderSystem, *collisionSystem);

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        sf::Time tDt = sf::seconds(dt);
        processEvents();
        update(dt, tDt);
        render(debugMode);
    }
}

void Engine::update(float dt, sf::Time tDt) {
    // System update order is critical; ensure dependencies are respected (e.g., input before movement)
    inputSystem->update(*componentManager, dt);
    physicsSystem->update(*componentManager, dt);
    movementSystem->update(*componentManager, dt);
    collisionSystem->update(*componentManager, dt);
    animationSystem->update(*componentManager, dt);
    damageSystem->update(*componentManager, *entityManager, *systemManager, *particleSystem, dt);
    particleSystem->update(tDt, *componentManager);
    actorSystem->update(*componentManager, dt);
    triggerSystem->update(*componentManager, dt);
    cameraSystem->update(*componentManager, dt);
    window.setView(cameraSystem->view);
}

void Engine::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        // Consider adding more event handling, e.g., resize, key presses if not handled in systems
    }
}

void Engine::render(bool debugMode) {
    window.clear(sf::Color(155, 212, 195));
    renderSystem->update(window, *componentManager, *particleSystem, debugMode);
    window.display();
}

bool Engine::loadEntities(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open entity file: " << filepath << std::endl;
        return false;
    }

    nlohmann::json jsonArray;
    try {
        file >> jsonArray;
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return false;
    }

    for (const auto& j : jsonArray) {
        if (!j.contains("name")) {
            std::cerr << "Entity missing 'name' field. Skipping." << std::endl;
            continue;
        }

        std::cout << "|------WORKING ON ENTITY: " << j["name"] << "------|\n";

        Entity entity = entityManager->createEntity();

        // Player-specific setup
        if (j.value("player", false)) {
            cameraSystem->entities.insert(entity);
            inputSystem->entities.insert(entity);
            groundResetSystem->entities.insert(entity);
            componentManager->addComponent<PlayerComponent>(entity, {});
            componentManager->addComponent<WallClingComponent>(entity, {});
            std::cout << "...Registered To Systems: cameraSystem, inputSystem, groundResetSystem\n";
            std::cout << "...Added Component: PlayerComponent, WallClingComponent\n";
        }

        // Animation handling
        std::shared_ptr<sf::Texture> entityTexture;
        if (j.contains("anim")) {
            const auto& animJson = j["anim"];
            std::string startAnim = animJson.value("start", "");
            if (startAnim.empty()) {
                std::cerr << "Animation missing 'start' state for entity: " << j["name"] << std::endl;
                continue;
            }

            AnimationComponent anim;
            anim.currentState = startAnim;

            if (!j.contains("spritePath")) {
                std::cerr << "Animation defined but missing 'spritePath' for entity: " << j["name"] << std::endl;
                continue;
            }

            for (auto it = j["spritePath"].begin(); it != j["spritePath"].end(); ++it) {
                const std::string& state = it.key();
                const auto& pathJson = it.value();

                auto tex = std::make_shared<sf::Texture>();
                std::string filePath = pathJson.value("filePath", "");
                if (filePath.empty() || !tex->loadFromFile(filePath)) {
                    std::cerr << "Failed to load texture for state '" << state << "': " << filePath << std::endl;
                    continue;
                }

                anim.animations[state] = {
                    .texture = tex,
                    .frameCount = pathJson.value("frameCount", 1),
                    .frameWidth = animJson.value("frameWidth", 0),
                    .frameHeight = animJson.value("frameHeight", 0),
                    .frameTime = pathJson.value("frameTime", 0.0f)
                };

                if (state == startAnim) {
                    entityTexture = tex;
                }
            }

            componentManager->addComponent<AnimationComponent>(entity, anim);
            animationSystem->entities.insert(entity);
            std::cout << "...Added Component: AnimationComponent->with the following states:\n";
            for (const auto& [k, _] : anim.animations) {
                std::cout << " - " << k << '\n';
            }
            std::cout << "...Registered To System: animationSystem\n";
        }

        // Sprite component (requires texture from animation or separate sprite handling)
        if (entityTexture) {
            sf::Sprite entitySprite;
            entitySprite.setTexture(*entityTexture);
            entitySprite.setScale(3.f, 3.f);  // Consider making scale configurable
            int frameWidth = j.value("anim", nlohmann::json{}).value("frameWidth", 0);
            int frameHeight = j.value("anim", nlohmann::json{}).value("frameHeight", 0);
            entitySprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
            entitySprite.setOrigin(static_cast<float>(frameWidth) / 2.f, static_cast<float>(frameHeight) / 2.f);

            componentManager->addComponent<SpriteComponent>(entity, {entitySprite});
            std::cout << "...Added Component: SpriteComponent\n";
        } else if (j.value("sprite", false)) {
            // Handle static sprites separately if no animation
            std::string filePath = j.value("spritePath", "");
            if (filePath.empty()) {
                std::cerr << "Static sprite missing 'spritePath' for entity: " << j["name"] << std::endl;
                continue;
            }
            auto tex = std::make_shared<sf::Texture>();
            if (!tex->loadFromFile(filePath)) {
                std::cerr << "Failed to load static sprite: " << filePath << std::endl;
                continue;
            }
            sf::Sprite entitySprite;
            entitySprite.setTexture(*tex);
            entitySprite.setScale(3.f, 3.f);
            // Assume default dimensions for static sprites; make configurable
            entitySprite.setTextureRect(sf::IntRect(0, 0, 48, 64));
            entitySprite.setOrigin(24.f, 32.f);
            componentManager->addComponent<SpriteComponent>(entity, {entitySprite});
            std::cout << "...Added Component: SpriteComponent (static)\n";
        }

        // Actor component
        if (j.contains("actor")) {
            std::string actorName = j["actor"].value("name", "");
            if (!actorName.empty()) {
                componentManager->addComponent<ActorComponent>(entity, {actorName});
                actorSystem->entities.insert(entity);
                std::cout << "...Added Component: ActorComponent (" << actorName << ")\n";
                std::cout << "...Registered To System: actorSystem\n";
            }
        }

        // Position component
        if (j.contains("Position")) {
            const auto& pos = j["Position"];
            float x = pos.value("x", 0.f);
            float y = pos.value("y", 0.f);
            componentManager->addComponent<Position>(entity, {x, y});
            std::cout << "...Added Component: PositionComponent-> with coordinates {" << x << "," << y << "}\n";
        }

        // Velocity component
        if (j.contains("Velocity")) {
            const auto& vel = j["Velocity"];
            float dx = vel.value("dx", 0.f);
            float dy = vel.value("dy", 0.f);
            componentManager->addComponent<Velocity>(entity, {dx, dy});
            physicsSystem->entities.insert(entity);
            std::cout << "...Added Component: Velocity-> with vectors {" << dx << "," << dy << "}\n";
            std::cout << "...Registered To System: physicsSystem\n";
        }

        // Direction component
        if (j.value("Direction", false)) {
            componentManager->addComponent<DirectionComponent>(entity, {});
            std::cout << "...Added Component: DirectionComponent\n";
        }

        // Collider component
        if (j.contains("Collision")) {
            const auto& col = j["Collision"];
            ColliderComponent colCom;
            colCom.bounds = sf::FloatRect(
                col.value("rectLeft", 0.f),
                col.value("rectTop", 0.f),
                col.value("rectWidth", 0.f),
                col.value("rectHeight", 0.f)
            );
            colCom.isStatic = col.value("isStatic", false);

            componentManager->addComponent<ColliderComponent>(entity, colCom);
            collisionSystem->entities.insert(entity);

            std::cout << "...Added Component: ColliderComponent-> with parameters:\n"
                      << "  rectLeft: " << colCom.bounds.left << "\n"
                      << "  rectTop: " << colCom.bounds.top << "\n"
                      << "  rectWidth: " << colCom.bounds.width << "\n"
                      << "  rectHeight: " << colCom.bounds.height << "\n"
                      << "  isStatic: " << colCom.isStatic << "\n";
            std::cout << "...Registered To System: collisionSystem\n";
        }

        // Attack collider component
        if (j.contains("AttackCollisionRight") && j.contains("AttackCollisionLeft")) {
            AttackColliderComponent colCom;
            const auto& right = j["AttackCollisionRight"];
            colCom.boundsRight = sf::FloatRect(
                right.value("rectLeft", 0.f),
                right.value("rectTop", 0.f),
                right.value("rectWidth", 0.f),
                right.value("rectHeight", 0.f)
            );
            const auto& left = j["AttackCollisionLeft"];
            colCom.boundsLeft = sf::FloatRect(
                left.value("rectLeft", 0.f),
                left.value("rectTop", 0.f),
                left.value("rectWidth", 0.f),
                left.value("rectHeight", 0.f)
            );

            componentManager->addComponent<AttackColliderComponent>(entity, colCom);
            std::cout << "...Added Component: AttackColliderComponent\n";
        }

        // Movement system registration (assuming no specific component, just registration)
        if (j.value("Movement", false)) {
            movementSystem->entities.insert(entity);
            std::cout << "...Registered To System: movementSystem\n";
        }

        // Health component
        if (j.value("health", false)) {
            componentManager->addComponent<HealthComponent>(entity, {});  // Consider adding initial health value from JSON
            damageSystem->entities.insert(entity);
            std::cout << "...Added Component: HealthComponent\n";
            std::cout << "...Registered To System: damageSystem\n";
        }

        // Always register to render system if it has visual components
        renderSystem->entities.insert(entity);
        std::cout << "...Registered To System: renderSystem\n";
    }

    return true;
}
