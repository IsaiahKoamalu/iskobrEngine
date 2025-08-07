#ifndef FLUIDPARTICLESYSTEM_H
#define FLUIDPARTICLESYSTEM_H

#include <algorithm>
#include <cmath>
#include <cassert>
#include <random>

#include "Engine/Core/UpdateContext.h"
#include "ParticleSystemBase.h"
#include "Engine/Components/PlayerComponent.h"
#include "Engine/Components/ColliderComponent.h"
#include "Engine/Components/Position.h"

class FluidParticleSystem : public ParticleSystem
{
public:
    sf::Vector2f m_lastPlayerPos{0.f, 0.f};
    bool m_firstFrame{true};

    void update(const UpdateContext& ctxt) override
    {
        ComponentManager& component = *ctxt.component;
        const float dt = ctxt.tDt.asSeconds();
        std::size_t vertexIndex = 0;
        m_vertices.resize(m_particles.size() * 4);

        assert(m_collisionSystem && "ParticleSystem: collision pointer null");

        bool hasPlayer = false;
        sf::FloatRect playerBounds;
        sf::Vector2f playerCenter{0.f, 0.f};
        sf::Vector2f playerVel{0.f, 0.f};
        for (Entity entity : entities)
        {
            if (component.hasComponent<PlayerComponent>(entity))
            {
                hasPlayer = true;
                auto& pos  = component.getComponent<Position>(entity);
                auto& col  = component.getComponent<ColliderComponent>(entity);
                auto& vel = component.getComponent<Velocity>(entity);
                playerBounds = sf::FloatRect
                (
                    pos.x + col.bounds.left,
                    pos.y + col.bounds.top,
                    col.bounds.width,
                    col.bounds.height
                );

                playerCenter = sf::Vector2f
                (
                    playerBounds.left + playerBounds.width  * 0.5f,
                    playerBounds.top  + playerBounds.height * 0.5f
                );
                playerVel = {vel.dx, vel.dy};
                break;
            }
        }

        // Compute player velocity for directional push
        if (!m_firstFrame && hasPlayer)
        {
            playerVel = (playerCenter - m_lastPlayerPos) / dt;
        }
        if (hasPlayer)
        {
            m_lastPlayerPos = playerCenter;
        }
        m_firstFrame = false;

        constexpr float influenceRadius = 60.f;
        constexpr float dirStrength = 0.3f;
        constexpr float radialStrength = 40.f;
        constexpr float playerBounce = 0.2f;
        static std::normal_distribution<float> noisDist(0.f, 1.f);

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


            constexpr float bounce = 0.4f; // 60% energy loss on bounce
            constexpr float tile = 16.f; // tile size

            auto solid = [&](float wx, float wy)
            {
                return m_collisionSystem && m_collisionSystem->isSolidAt(component, wx, wy);
            };

            // sample point one half-size below center
            if (solid(newPos.x, newPos.y + p.size * 0.5f) && p.velocity.y > 10)
            {
                newPos.y = std::floor((newPos.y + p.size * 0.5f) / tile) * tile - p.size * 0.5f;
                p.velocity.y *= -bounce; // damp and invert vertical
            }

            // wall check
            if (solid(newPos.x + p.size * 0.5f, newPos.y) ||
                solid(newPos.x - p.size * 0.5f, newPos.y))
            {
                p.velocity.x *= -bounce; // damp and invert horizontal
            }

            if (hasPlayer)
            {
                {
                    sf::Vector2f diffCenter = newPos - playerCenter;
                    float d2center = diffCenter.x*diffCenter.x + diffCenter.y*diffCenter.y;
                    if (d2center < influenceRadius * influenceRadius)
                    {
                        float d = std::sqrt(d2center);
                        if (d > 0.f)
                        {
                            sf::Vector2f pushDir = diffCenter / d;
                            float factor = 1.f - (d / influenceRadius);
                            // apply a gentle fling outward
                            p.velocity += pushDir * (radialStrength * factor * dt);
                        }
                    }
                }
                float radius  = p.size * 0.5f;
                float cx      = newPos.x;
                float cy      = newPos.y;
                float nearestX = std::clamp(cx, playerBounds.left,
                                            playerBounds.left + playerBounds.width);
                float nearestY = std::clamp(cy, playerBounds.top,
                                            playerBounds.top  + playerBounds.height);

                float dx   = cx - nearestX;
                float dy   = cy - nearestY;
                float dist2 = dx*dx + dy*dy;

                if (dist2 < radius*radius)
                {
                    float dist = std::sqrt(dist2);
                    if (dist > 0.f)
                    {
                        sf::Vector2f normal{ dx/dist, dy/dist };

                        // Stronger interpenetration resolution (3x)
                        float penetration = radius - dist + 0.001f;
                        newPos += normal * (penetration * 3.f);

                        // Enhanced restitution: amplify bounce effect
                        float vRelN = p.velocity.x*normal.x + p.velocity.y*normal.y;
                        float e = playerBounce * 2.f; // stronger bounce
                        p.velocity -= (1.f + e) * vRelN * normal;

                        // Directional push: inherit a larger fraction of player velocity
                        p.velocity += playerVel * (dirStrength * 2.f);

                        // Tangential friction to simulate drag along surface
                        sf::Vector2f tangent{ -normal.y, normal.x };
                        float vRelT = p.velocity.x*tangent.x + p.velocity.y*tangent.y;
                        p.velocity -= COLLISION_FRICTION * vRelT * tangent;
                    }
                }
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

    void resetParticle(Particle &p) override
    {
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

inline std::uniform_real_distribution<float> FluidParticleSystem::angleDeg(180.f, 360.f);
inline std::uniform_real_distribution<float> FluidParticleSystem::speedDist(50.f, 150.f);
inline std::uniform_real_distribution<float> FluidParticleSystem::sizeDist(3.f, 6.f);

#endif