#ifndef PARTICLESYSTEMBASE_H
#define PARTICLESYSTEMBASE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>

#include "Engine/Systems/CollisionSystem.h"
#include "Engine/System.h"


class ParticleSystem : public System, public sf::Drawable, public sf::Transformable {
public:
    explicit ParticleSystem(std::size_t maxParticles = 500)
        : m_capacity(maxParticles)
    {
        m_particles.reserve(m_capacity);
        m_vertices.setPrimitiveType(sf::Quads);
        m_vertices.resize(maxParticles * 4);
    }

    void setCollisionSystem(const CollisionSystem* cs) {m_collisionSystem = cs;}

    void setEmitter(const sf::Vector2f& position) {
        m_emitter = position;
    }

    void setParticleLifetime(sf::Time lifeTime) {
        m_lifetime = lifeTime;
    }

    void spawnParticles(std::size_t count) {
        // Clamping the pool size
        if (m_particles.size() + count > m_capacity) {
            count = m_capacity - m_particles.size();
        }
        for (std::size_t n = 0; n < count; ++n) {
           Particle p;
            resetParticle(p);
            m_particles.push_back(p);
        }
    }
    virtual ~ParticleSystem() = default;

    virtual void update(const UpdateContext& ctxt) override = 0;

protected:
    struct Particle {
        sf::Vector2f velocity;
        sf::Vector2f position;
        sf::Time lifeTime;
        float size = 0.f;
    };

    std::size_t m_capacity;
    std::vector<Particle> m_particles;
    sf::VertexArray m_vertices{sf::Points};
    sf::Time m_lifetime{sf::seconds(1.f)};
    sf::Vector2f m_emitter{};
    const sf::Vector2f m_gravity{ 0.f, 980.f }; // pixels / s^2 (appr. 100 px per 0.1 s)
    // Can be tuned later but: 980 = appr. 9.8 m/s^2 if world unit is 100 px = 1m

    const CollisionSystem* m_collisionSystem = nullptr;

    static std::random_device rd;
    static std::mt19937 rng;
    static std::uniform_real_distribution<float> angleDeg;
    static std::uniform_real_distribution<float> speedDist;
    static std::uniform_real_distribution<float> sizeDist;

    void resetParticle(Particle& p) {
        float theta = angleDeg(rng) * 3.14159265f / 180.f;
        float speed = speedDist(rng);

        p.velocity = {std::cos(theta) * speed, std::sin(theta) * speed };
        p.lifeTime = m_lifetime;

        p.size = sizeDist(rng);
        p.position = m_emitter;
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(m_vertices, states);
    }
};

// Static member definitions
inline std::random_device ParticleSystem::rd;
inline std::mt19937 ParticleSystem::rng;
inline std::uniform_real_distribution<float> ParticleSystem::angleDeg(0.f, 360.f);
inline std::uniform_real_distribution<float> ParticleSystem::speedDist(50.f, 150.f);
inline std::uniform_real_distribution<float> ParticleSystem::sizeDist(3.f, 6.f);

#endif
