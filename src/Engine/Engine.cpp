#include "Engine/Engine.h"
#include <SFML/Graphics.hpp>
#include <iostream>

void Engine::run() {
    window.create(sf::VideoMode(800, 600), "iskobr-Engine");

    if (!playerTexture.loadFromFile("assets/player.png")) {
        std::cerr << "Failed to load player.png\n";
        return;
    }

    playerSprite.setTexture(playerTexture);
    playerSprite.setScale(3, 3);
    playerSprite.setPosition(400, 300);

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        render();
        processEvents();
        update(dt);
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
