#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <SFML/Window/Event.hpp>
#include <unordered_map>

 /** @brief Class responsible for setting up and managing user input.*/
class InputManager {
public:
    void handleEvent(const sf::Event& event);
    bool isKeyPressed(sf::Keyboard::Key key) const;

private:
    std::unordered_map<sf::Keyboard::Key, bool> keyStates;
};

#endif