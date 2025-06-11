#ifndef ENGINE_H
#define ENGINE_H

#include <SFML/Graphics.hpp>
#include "Engine/InputManager.h"

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
};

#endif