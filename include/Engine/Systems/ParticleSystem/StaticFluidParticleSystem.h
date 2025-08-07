#ifndef STATICFLUIDPARTICLESYSTEM_H
#define STATICFLUIDPARTICLESYSTEM_H

#include "Engine/Core/UpdateContext.h"
#include "ParticleSystemBase.h"

class StaticFluidParticleSystem : public ParticleSystem
{
public:
    void update(const UpdateContext& ctxt) override
    {
        auto& components = *ctxt.component;
        const float dt = ctxt.tDt.asSeconds();
        std::size_t vertexIndex = 0;
        m_vertices.resize(m_particles.size() * 4);

        assert(m_collisionSystem && "ParticleSystem: collision pointer null");

        for (std::size_t i = 0; i < m_particles.size();)
        {
            Particle& p = m_particles[i];
            p.lifeTime -= ctxt.tDt;

            if (p.lifeTime <= sf::Time::Zero)
            {
                m_particles[i] = m_particles.back();
                m_particles.pop_back();
                continue;
            }

            p.velocity += m_gravity * dt;
            sf::Vector2f newPos = p.position + p.velocity * dt;

            constexpr float bounce = 0.4f;
            constexpr float tile = 16.f;

            auto solid = [&](float wx, float wy)
            {
                return m_collisionSystem && m_collisionSystem->isSolidAt(components, wx, wy);
            };

            if (solid(newPos.x, newPos.y + p.size * 0.0f) && p.velocity.y > 10)
            {
                newPos.y = std::floor((newPos.y + p.size * 0.5f) / tile) * tile - p.size * 0.5f;
                p.velocity.y *= -bounce;
            }

            if (solid(newPos.x + p.size * 0.f, newPos.y) ||
                solid(newPos.x -p.size * 0.5f, newPos.y))
            {
                p.velocity.x *= -bounce;
            }

            p.position = newPos;

            float half = p.size * 0.5f;
            float lifeRatio = p.lifeTime.asSeconds() / m_lifetime.asSeconds();
            auto alpha = static_cast<sf::Uint8>(lifeRatio * 255);

            sf::Color color(255, 0, 0, alpha);

            m_vertices[vertexIndex + 0].position = { newPos.x - half, newPos.y - half };
            m_vertices[vertexIndex + 1].position = { newPos.x + half, newPos.y - half };
            m_vertices[vertexIndex + 2].position = { newPos.x + half, newPos.y + half };
            m_vertices[vertexIndex + 3].position = { newPos.x - half, newPos.y + half };

            m_vertices[vertexIndex + 0].color = color;
            m_vertices[vertexIndex + 1].color = color;
            m_vertices[vertexIndex + 2].color = color;
            m_vertices[vertexIndex + 3].color = color;

            auto ts = m_texture.getSize();
            auto tw = float(ts.x), th = float(ts.y);

            m_vertices[vertexIndex + 0].texCoords = {0.f, 0.f};
            m_vertices[vertexIndex + 1].texCoords = {tw, 0.f};
            m_vertices[vertexIndex + 2].texCoords = {tw, th};
            m_vertices[vertexIndex + 3].texCoords = {0.f, th};

            vertexIndex += 4;

            ++i;
        }
    }

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        states.texture = &m_texture;
        target.draw(m_vertices, states);
    }

    void resetParticle(Particle& p) override
    {
        setParticleLifetime(m_lifetime);
        float theta = angleDeg(rng) * 3.14159265f / 180.f;
        float speed = speedDist(rng);

        p.velocity = { std::cos(theta) * speed, std::sin(theta) * speed };
        p.lifeTime = m_lifetime;
        p.size = sizeDist(rng);
        p.position = m_emitter;
    }
    sf::Time m_lifetime{sf::seconds(1.f)};
    static std::uniform_real_distribution<float> angleDeg;
    static std::uniform_real_distribution<float> speedDist;
    static std::uniform_real_distribution<float> sizeDist;
};

inline std::uniform_real_distribution<float> StaticFluidParticleSystem::angleDeg(180.f, 360.f);
inline std::uniform_real_distribution<float> StaticFluidParticleSystem::speedDist(200.f, 400.f);
inline std::uniform_real_distribution<float> StaticFluidParticleSystem::sizeDist(4.f, 7.f);

#endif