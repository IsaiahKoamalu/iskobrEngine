#ifndef HOMINGPARTICLESYSTEM_H
#define HOMINGPARTICLESYSTEM_H

#include "Engine/Core/UpdateContext.h"
#include "ParticleSystemBase.h"


class HomingParticleSystem : public ParticleSystem {
public:

    void update(const UpdateContext& ctxt) override
    {
        ComponentManager& component = *ctxt.component;

        const float dtSec = ctxt.tDt.asSeconds();
        std::size_t vertexIndex = 0;
        m_vertices.resize(m_particles.size() * 4);

        assert(m_collisionSystem && "ParticleSystem: collision pointer null");

        auto it = entities.begin();
        Entity player = *it;

        auto& pos = component.getComponent<Position>(player);
        sf::Vector2f playerPos = {pos.x, pos.y};

        for (std::size_t i = 0; i < m_particles.size();) {
            Particle& p = m_particles[i];

            p.size = 10;

            sf::Vector2f direction = playerPos - p.position;

            float lengthVal = length(direction);

            if (lengthVal != 0.f)
                direction /= lengthVal;
            else
                direction = {0.f, 0.f};

            float speed = 200.f;
            sf::Vector2f desiredVelocity = direction * speed;
            p.velocity += (desiredVelocity - p.velocity) * 0.1f; // smoothing factor

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

            sf::Color color(0, 0, 255, alpha);

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
};

#endif
