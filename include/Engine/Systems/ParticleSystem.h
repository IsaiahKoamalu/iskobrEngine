#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>

#include "CollisionSystem.h"
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

    void update(sf::Time dt, ComponentManager& component)
    {
        const float dtSec = dt.asSeconds();
        std::size_t vertexIndex = 0;
        m_vertices.resize(m_particles.size() * 4);

        assert(m_collisionSystem && "ParticleSystem: collision pointer null");

        for (std::size_t i = 0; i < m_particles.size();) {
            Particle& p = m_particles[i];
            p.lifeTime -= dt;


            if (p.lifeTime <= sf::Time::Zero) {
                m_particles[i] = m_particles.back();
                m_particles.pop_back();
                continue;
            }

            p.velocity += m_gravity * dtSec;
            sf::Vector2f newPos = p.position + p.velocity * dtSec;


            constexpr float bounce = 0.4f; // 60% energy loss on bounce
            constexpr float tile = 16.f; // tile size

            auto solid = [&](float wx, float wy) {
                return m_collisionSystem && m_collisionSystem->isSolidAt(component, wx, wy);
            };

            // sample point one half-size below center
            if (solid(newPos.x, newPos.y + p.size * 0.5f)) {
                newPos.y = std::floor((newPos.y + p.size * 0.5f) / tile) * tile - p.size * 0.5f;
                p.velocity.y *= -bounce; // damp and invert vertical
            }

            // wall check
            if (solid(newPos.x + p.size * 0.5f, newPos.y) ||
                solid(newPos.x - p.size * 0.5f, newPos.y)) {
                p.velocity.x *= -bounce; // damp and invert horizontal
            }

            p.position = newPos;

            float half = p.size * 0.5f;
            float lifeRatio = p.lifeTime.asSeconds() / m_lifetime.asSeconds();
            sf::Uint8 alpha = static_cast<sf::Uint8>(lifeRatio * 255);

            sf::Color color(255, 0, 0, alpha);

            // 4 vertices per quad (TL, TR, BR, BL)
            m_vertices[vertexIndex + 0].position = { newPos.x - half, newPos.y - half };
            m_vertices[vertexIndex + 1].position = { newPos.x + half, newPos.y - half };
            m_vertices[vertexIndex + 2].position = { newPos.x + half, newPos.y + half };
            m_vertices[vertexIndex + 3].position = { newPos.x - half, newPos.y + half };

            m_vertices[vertexIndex + 0].color = color;
            m_vertices[vertexIndex + 1].color = color;
            m_vertices[vertexIndex + 2].color = color;
            m_vertices[vertexIndex + 3].color = color;

            vertexIndex += 4;

            ++i;
        }
    }

private:
    struct Particle {
        sf::Vector2f velocity;
        sf::Vector2f position;
        sf::Time lifeTime;
        float size;
    };

    std::size_t m_capacity;
    std::vector<Particle> m_particles;
    sf::VertexArray m_vertices{sf::Points};
    sf::Time m_lifetime{sf::seconds(3.f)};
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
