#include "Engine/InputManager.h"

void InputManager::handleEvent(const sf::Event &event) {
    if (event.type == sf::Event::KeyPressed) {
        keyStates[event.key.code] = true;
    } else if (event.type == sf::Event::KeyReleased) {
        keyStates[event.key.code] = false;
    }
}

bool InputManager::isKeyPressed(sf::Keyboard::Key key) const {
    auto it = keyStates.find(key);
    return it != keyStates.end() && it->second;
}

