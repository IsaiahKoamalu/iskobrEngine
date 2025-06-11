#ifndef ENGINE_H
#define ENGINE_H

#include <memory>
#include <SFML/Graphics.hpp>

#include "Engine/InputManager.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

class  Engine {
public:
    void run();

private:
    sf::RenderWindow window;
    sf::Texture playerTexture;
    sf::Sprite playerSprite;

    void processEvents();
    void update(float dt);
    void render();

    InputManager input;

    std::unique_ptr<EntityManager> entityManager;
    std::unique_ptr<ComponentManager> componentManager;
    std::unique_ptr<SystemManager> syatemManager;
};

#endif