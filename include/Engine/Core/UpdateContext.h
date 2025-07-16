#ifndef UPDATECONTEXT_H
#define UPDATECONTEXT_H

#include <SFML/Graphics.hpp>
#include <vector>



class ComponentManager;
class ParticleSystem;
class SystemManager;


struct UpdateContext {
    sf::Time tDt;
    float dt;
    ComponentManager* component;
    EntityManager* entity;
    SystemManager* system;
    sf::RenderWindow* window;
    std::vector<std::shared_ptr<ParticleSystem>> particleSystems;
    std::vector<sf::Drawable*> drawables;
};

#endif
