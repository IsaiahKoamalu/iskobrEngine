#ifndef ENGINE_H
#define ENGINE_H

#include <memory>
#include <SFML/Graphics.hpp>

#include "Engine/InputManager.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "Systems/MovementSystem.h"
#include "Systems/PlayerInputSystem.h"
#include "Systems/RenderSystem.h"
#include "SystemManager.h"
#include "Systems/AnimationSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/TriggerSystem.h"

class  Engine {
public:
    void run();

private:
    sf::RenderWindow window;

    void processEvents();
    void update(float dt);
    void render();

    InputManager input;

    std::unique_ptr<EntityManager> entityManager;
    std::unique_ptr<ComponentManager> componentManager;
    std::unique_ptr<SystemManager> systemManager;
    std::unique_ptr<sf::Texture> playerTexture;

    std::shared_ptr<PlayerInputSystem> inputSystem;
    std::shared_ptr<MovementSystem> movementSystem;
    std::shared_ptr<RenderSystem> renderSystem;
    std::shared_ptr<AnimationSystem> animationSystem;
    std::shared_ptr<CollisionSystem> collisionSystem;
    std::shared_ptr<TriggerSystem> triggerSystem;


};

#endif