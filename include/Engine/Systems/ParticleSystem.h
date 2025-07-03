#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include "Engine/System.h"

class ParticleSystem : public System, public sf::Drawable, public sf::Transformable {
public:
    explicit ParticleSystem(std::size_t reserve = 0) {
        m_particles.reserve(reserve);
    }

    void setEmitter(const sf::Vector2f& position) {
        m_emitter = position;
    }

    void setParticleLifetime(sf::Time lifeTime) {
        m_lifetime = lifeTime;
    }

    void spawnParticles(std::size_t count) {
        for (std::size_t n = 0; n < count; ++n) {
            m_particles.emplace_back();
            m_vertices.append(sf::Vertex());

            resetParticle(m_particles.back(), m_vertices[m_vertices.getVertexCount() - 1]);
        }
    }

    void update(sf::Time dt) {
        constexpr std::size_t maxBurst = 100;

        for (std::size_t i = 0; i < m_particles.size(); ) {
            Particle& p = m_particles[i];
            sf::Vertex& v = m_vertices[i];

            p.lifeTime -= dt;
            v.position += p.Velocity * dt.asSeconds();

            if (p.lifeTime <= sf::Time::Zero) {
                std::size_t last = m_particles.size() - 1;
                m_particles[i] = m_particles[last];
                m_vertices[i] = m_vertices[last];
                m_particles.pop_back();
                m_vertices.resize(last);
                continue;
            }

            float ratio = p.lifeTime.asSeconds() / m_lifetime.asSeconds();
            ratio = std::clamp(ratio, 0.f, 1.f);
            v.color.a = static_cast<std::uint8_t>(ratio * 225);

            ++i;
        }
    }

private:
    struct Particle {
        sf::Vector2f Velocity;
        sf::Time lifeTime;
    };

    std::vector<Particle> m_particles;
    sf::VertexArray m_vertices{sf::Points};
    sf::Time m_lifetime{sf::seconds(3.f)};
    sf::Vector2f m_emitter{};

    static std::random_device rd;
    static std::mt19937 rng;
    static std::uniform_real_distribution<float> angleDeg;
    static std::uniform_real_distribution<float> speedDist;

    void resetParticle(Particle& p, sf::Vertex& v) {
        float theta = angleDeg(rng) * 3.14159265f / 180.f;
        float speed = speedDist(rng);

        p.Velocity = {std::cos(theta) * speed, std::sin(theta) * speed };
        p.lifeTime = m_lifetime;

        v.position = m_emitter;

        v.color = sf::Color::Red;
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(m_vertices, states);
    }
};

// Static member definitions
inline std::random_device ParticleSystem::rd;
inline std::mt19937 ParticleSystem::rng;
inline std::uniform_real_distribution<float> ParticleSystem::angleDeg(-360.f, 90.f);
inline std::uniform_real_distribution<float> ParticleSystem::speedDist(10.f, 50.f);

#endif
