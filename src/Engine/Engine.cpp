#include "Engine/Engine.h"
#include <SFML/Graphics.hpp>
#include <iostream>

#include "Engine/Core/UpdateContext.h"
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

    ctxt = std::make_shared<UpdateContext>();



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
    physicsSystem = systemManager->registerSystem<PhysicsSystem>();
    groundResetSystem = systemManager->registerSystem<GroundResetSystem>();
    damageSystem = systemManager->registerSystem<DamageSystem>();
    knockBackSystem = systemManager->registerSystem<KnockBackSystem>();
    homingParticleSystem = systemManager->registerSystem<HomingParticleSystem>();
    fluidParticleSystem = systemManager->registerSystem<FluidParticleSystem>();
    gaseousParticleSystem = systemManager->registerSystem<GaseousParticleSystem>();
    //Passing the collision system to the particle system so that it can handle
    //its own collision. Using .get() since the collision system is a shared_ptr.
    homingParticleSystem->setCollisionSystem(collisionSystem.get());
    fluidParticleSystem->setCollisionSystem(collisionSystem.get());
    gaseousParticleSystem->setCollisionSystem(collisionSystem.get());


    // Vector of drawables(pretty much just the different particle systems) for the update context.
    //calling them with .get() so that the actual raw pointer is retrieved.
    drawables.push_back(homingParticleSystem.get());
    drawables.push_back(fluidParticleSystem.get());
    drawables.push_back(gaseousParticleSystem.get());

    // Vector of the particle systems as objects of ParticleSystem.
    particleSystems.push_back(homingParticleSystem);
    particleSystems.push_back(fluidParticleSystem);
    particleSystems.push_back(gaseousParticleSystem);

    auto entityFile = std::make_shared<std::string>("assets/entities.json");
    if (!loadEntities(*entityFile)) {
        std::cerr << "PROBLEM LOADING ENTITIES" << std::endl;
    };

    tilesetManager->addTileset("grass", "assets/grassSheet.png", 16, 16);
    tilesetManager->addTileset("water", "assets/Water.png", 16, 16);
    tilesetManager->addTileset("dirt", "assets/dirtSheet.png", 16, 16);
    tilesetManager->addTileset("*water", "assets/NCWater.png", 16, 16);
    tileMapSystem->loadMap("assets/maps/level.json", *componentManager, *entityManager, *tilesetManager, *renderSystem,
                           *collisionSystem);

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        sf::Time tDt = sf::seconds(dt);

        // Wiring update context
        ctxt->tDt = tDt;
        ctxt->dt = dt;
        ctxt->component = componentManager.get();
        ctxt->entity = entityManager.get();
        ctxt->system = systemManager.get();
        ctxt->window = &window;
        ctxt->drawables = drawables;
        ctxt->particleSystems = particleSystems;

        processEvents();
        update(*ctxt);
        render(*ctxt, false);
    }
}

void Engine::update(UpdateContext& ctxt) {
    inputSystem->update(ctxt);
    physicsSystem->update(ctxt);
    movementSystem->update(ctxt);
    knockBackSystem->update(ctxt);
    collisionSystem->update(ctxt);
    animationSystem->update(ctxt);
    damageSystem->update(ctxt);
    homingParticleSystem->update(ctxt);
    fluidParticleSystem->update(ctxt);
    gaseousParticleSystem->update(ctxt);
    actorSystem->update(ctxt);
    triggerSystem->update(ctxt);
    cameraSystem->update(ctxt);
    window.setView(cameraSystem->view);
}

void Engine::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }
}

void Engine::render(UpdateContext& ctxt, bool debugMode) {
    window.clear(sf::Color::Black); // 155, 212, 195
    renderSystem->update(ctxt);
    window.display();
}

bool Engine::loadEntities(std::string &filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open entity file: " << filepath << std::endl;
        return false;
    }
    nlohmann::json jsonArray;
    file >> jsonArray;

    for (const auto &j: jsonArray) {
        Entity entity = entityManager->createEntity();

        std::cout << "|------WORKING ON ENTITY: " << j["name"] << "------|\n";

        if (j.contains("player") && j["player"] == true) {
            cameraSystem->entities.insert(entity);
            inputSystem->entities.insert(entity);
            groundResetSystem->entities.insert(entity);
            homingParticleSystem->entities.insert(entity);
            componentManager->addComponent<PlayerComponent>(entity,{});
            componentManager->addComponent<WallClingComponent>(entity, {});
            std::cout << "...Registered To Systems: cameraSystem, inputSystem, groundResetSystem\n";
            std::cout << "...Added Component: PlayerComponent\n";
        }

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
                    .frameCount = it.value()["frameCount"],
                    .frameWidth = j["anim"]["frameWidth"],
                    .frameHeight = j["anim"]["frameHeight"],
                    .frameTime = it.value()["frameTime"]
                };

                if (it.key() == startAnim) {
                    entityTexture = tex;
                }
            }
            componentManager->addComponent<AnimationComponent>(entity, {anim});
            std::cout << "...Added Component: AnimationComponent->with the following states:\n";
            for (const auto &[k, _]: anim.animations)
                std::cout << " - " << k << '\n';
            animationSystem->entities.insert(entity);
            std::cout << "...Registered To System: animationSystem\n";
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
            std::cout << "...Added Component: SpriteComponent\n";
        }

        if (j.contains("sprite") && j["sprite"] == true) {
            std::string startAnim = j["anim"]["start"];
            auto entityTexture = std::make_shared<sf::Texture>();
            entityTexture->loadFromFile(j["spritePath"][startAnim]["filePath"]);

            sf::Sprite entitySprite;
            entitySprite.setTexture(*entityTexture);
            entitySprite.setScale(3, 3);
            entitySprite.setTextureRect(sf::IntRect(0, 0, 48, 64));
            entitySprite.setOrigin(48 / 2.f, 64 / 2.f);
        }
        if (j.contains("actor")) {
            componentManager->addComponent<ActorComponent>(entity, {j["actor"]["name"]});
            actorSystem->entities.insert(entity);
            std::cout << "...Added Component: Actor Name\n";
            std::cout << "...Registered To System: actorSystem\n";
        }
        if (j.contains("Position")) {
            componentManager->addComponent<Position>(entity, {j["Position"]["x"], j["Position"]["y"]});
            //float x = j["Positon"]["x"].get<float>();
            //float y = j["Position"]["y"].get<float>();
            //std::cout << "...Added Component: PositionComponent-> with coordinates {" << x << "," << y << "}\n";
        };
        if (j.contains("Velocity")) {
            componentManager->addComponent<Velocity>(entity, {j["Velocity"]["dx"], j["Velocity"]["dy"]});
            componentManager->addComponent<KnockBackComponent>(entity, {});
            physicsSystem->entities.insert(entity);
            knockBackSystem->entities.insert(entity);
            float dx = j["Velocity"]["dx"].get<float>();
            float dy = j["Velocity"]["dy"].get<float>();
            std::cout << "...Component Added: Velocity-> with vectors {" << dx << "," << dy << "}\n";
            std::cout << "...Registered To System: physicsSystem\n";
        }
        if (j.contains("Direction") && j["Direction"] == true) {
            componentManager->addComponent<DirectionComponent>(entity, {});
            std::cout << "...Added Component: DirectionComponent\n";
        }
        if (j.contains("Collision")) {
            ColliderComponent colCom;
            colCom.bounds = sf::FloatRect(j["Collision"]["rectLeft"], j["Collision"]["rectTop"],
                                          j["Collision"]["rectWidth"], j["Collision"]["rectHeight"]);
            colCom.isStatic = j["Collision"]["isStatic"];

            componentManager->addComponent<ColliderComponent>(entity, colCom);
            collisionSystem->entities.insert(entity);

            std::cout << "...Added Component: ColliderComponent-> with the following parameters...\n "
                         "        rectLeft:" << j["Collision"]["rectLeft"] << "\n "
                         "        rectTop:" << j["Collision"]["rectTop"] << "\n"
                         "        rectWidth:" << j["Collision"]["rectWidth"] << "\n"
                         "        rectHeight:" << j["Collision"]["rectHeight"] << "\n"
                         "        isStatic:" << j["Collision"]["isStatic"] << "\n";
            std::cout << "...Registered To System: collisionSystem\n";
        }
        if (j.contains("AttackCollisionRight")) {
            AttackColliderComponent colCom;
            // Right directional attack collision
            colCom.boundsRight = sf::FloatRect(j["AttackCollisionRight"]["rectLeft"], j["AttackCollisionRight"]["rectTop"],
                                          j["AttackCollisionRight"]["rectWidth"], j["AttackCollisionRight"]["rectHeight"]);
            // Left directional attack collision
            colCom.boundsLeft = sf::FloatRect(j["AttackCollisionLeft"]["rectLeft"], j["AttackCollisionLeft"]["rectTop"],
                                          j["AttackCollisionLeft"]["rectWidth"], j["AttackCollisionLeft"]["rectHeight"]);

            componentManager->addComponent<AttackColliderComponent>(entity, colCom);

            std::cout << "...Added Component: AttackColliderComponent-> with the following parameters...\n ";
        }

        if (j.contains("Movement")) {
            movementSystem->entities.insert(entity);
            std::cout << "...Registered To System: movementSystem\n";
        }

        if (j.contains("health") && j["health"] == true) {
            componentManager->addComponent<HealthComponent>(entity, {});
            damageSystem->entities.insert(entity);
            std::cout << "...Added Component: HealthComponent\n";
            std::cout << "...Registered To System: damageSystem\n";
        }

        renderSystem->entities.insert(entity);
        std::cout << "...Registered To System: renderSystem\n";
    }

    return true;
}
