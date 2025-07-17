#ifndef FLUIDPARTICLESYSTEM_H
#define FLUIDPARTICLESYSTEM_H

#include "Engine/Core/UpdateContext.h"
#include "ParticleSystemBase.h"

class FluidParticleSystem : public ParticleSystem {
public:
    void update(const UpdateContext& ctxt) override
    {
        ComponentManager& component = *ctxt.component;
        const float dtSec = ctxt.tDt.asSeconds();
        std::size_t vertexIndex = 0;
        m_vertices.resize(m_particles.size() * 4);

        assert(m_collisionSystem && "ParticleSystem: collision pointer null");

        for (std::size_t i = 0; i < m_particles.size();) {
            Particle& p = m_particles[i];
            p.lifeTime -= ctxt.tDt;


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
            if (solid(newPos.x, newPos.y + p.size * 0.5f) && p.velocity.y > 10) {
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
            auto alpha = static_cast<sf::Uint8>(lifeRatio * 255);

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

            auto ts = m_texture.getSize();
            float tw = float(ts.x), th = float(ts.y);

            m_vertices[vertexIndex + 0].texCoords = {0.f, 0.f};
            m_vertices[vertexIndex + 1].texCoords = {tw, 0.f};
            m_vertices[vertexIndex + 2].texCoords = {tw, th};
            m_vertices[vertexIndex + 3].texCoords = {0.f, th};

            vertexIndex += 4;

            ++i;
        }
    }
protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        states.texture = &m_texture;
        target.draw(m_vertices, states);
    }

    void resetParticle(Particle &p) override {
        float theta = angleDeg(rng) * 3.14159265f / 180.f;
        float speed = speedDist(rng);

        p.velocity = {std::cos(theta) * speed, std::sin(theta) * speed };
        p.lifeTime = m_lifetime;

        p.size = sizeDist(rng);
        p.position = m_emitter;
    }

    static std::uniform_real_distribution<float> angleDeg;
    static std::uniform_real_distribution<float> speedDist;
    static std::uniform_real_distribution<float> sizeDist;
};

inline std::uniform_real_distribution<float> FluidParticleSystem::angleDeg(0.f, 360.f);
inline std::uniform_real_distribution<float> FluidParticleSystem::speedDist(50.f, 150.f);
inline std::uniform_real_distribution<float> FluidParticleSystem::sizeDist(3.f, 6.f);

#endif