#ifndef GASEOUSPARTICLESYSTEM_H
#define GASEOUSPARTICLESYSTEM_H

#include <algorithm>
#include <cmath>
#include <cassert>
#include <random>

#include "Engine/Core/UpdateContext.h"
#include "ParticleSystemBase.h"
#include "Engine/Components/Position.h"
#include "Engine/Components/ColliderComponent.h"
#include "Engine/Components/PlayerComponent.h"

// Friction coefficient applied at the collision tangent
constexpr float COLLISION_FRICTION = 0.3f;

class GaseousParticleSystem : public ParticleSystem {
public:
    sf::Vector2f m_lastPlayerPos{0.f, 0.f};
    bool m_firstFrame{true};

    void update(const UpdateContext& ctxt) override {
        ComponentManager& component = *ctxt.component;
        const float dt   = ctxt.tDt.asSeconds();
        size_t  vIndex   = 0;

        // Resize vertex buffer
        m_vertices.clear();
        m_vertices.resize(m_particles.size() * 4);
        assert(m_collisionSystem && "ParticleSystem: collision pointer null");

        // Find player bounds and center
        bool hasPlayer = false;
        sf::FloatRect playerBounds;
        sf::Vector2f playerCenter{0.f, 0.f};
        sf::Vector2f playerVel{0.f, 0.f};
        for (Entity entity : entities) {
            if (component.hasComponent<PlayerComponent>(entity)) {
                hasPlayer = true;
                auto& pos  = component.getComponent<Position>(entity);
                auto& col  = component.getComponent<ColliderComponent>(entity);
                auto& vel = component.getComponent<Velocity>(entity);
                playerBounds = sf::FloatRect(
                    pos.x + col.bounds.left,
                    pos.y + col.bounds.top,
                    col.bounds.width,
                    col.bounds.height
                );

                playerCenter = sf::Vector2f(
                    playerBounds.left + playerBounds.width  * 0.5f,
                    playerBounds.top  + playerBounds.height * 0.5f
                );
                playerVel = {vel.dx, vel.dy};
                break;
            }
        }

        // Compute player velocity for directional push
        if (!m_firstFrame && hasPlayer) {
            playerVel = (playerCenter - m_lastPlayerPos) / dt;
        }
        if (hasPlayer) {
            m_lastPlayerPos = playerCenter;
        }
        m_firstFrame = false;

        // Gas motion parameters
        constexpr float influenceRadius = 50.f;
        constexpr float dirStrength     = 0.f;
        constexpr float radialStrength  = 60.f;
        constexpr float wallBounce      = 0.2f;
        constexpr float playerBounce    = 0.1f;
        constexpr float drag            = 0.5f;
        constexpr float jitterAmt       = 30.f;
        constexpr float swirlAmt        = 20.f;
        constexpr float tileSize        = 16.f;
        static std::normal_distribution<float> noiseDist(0.f, 1.f);

        for (size_t i = 0; i < m_particles.size();) {
            auto& p = m_particles[i];

            // Age & remove dead
            p.lifeTime -= ctxt.tDt;
            if (p.lifeTime <= sf::Time::Zero) {
                p = m_particles.back();
                m_particles.pop_back();
                continue;
            }

            // Gravity & integrate
            p.velocity -= sf::Vector2f{0.f, 120.f} * dt;
            sf::Vector2f newPos = p.position + p.velocity * dt;

            // Tile collisions (existing)
            auto solid = [&](float x, float y) {
                return m_collisionSystem->isSolidAt(component, x, y);
            };
            if (solid(newPos.x, newPos.y + p.size*0.5f) && p.velocity.y > 10.f) {
                newPos.y = std::floor((newPos.y + p.size*0.5f)/tileSize)*tileSize - p.size*0.5f;
                p.velocity.y *= -wallBounce;
            }
            if (solid(newPos.x + p.size*0.5f, newPos.y) ||
                solid(newPos.x - p.size*0.5f, newPos.y)) {
                p.velocity.x *= -wallBounce;
            }

            // --- Improved Player Collision (stronger push) ---
            if (hasPlayer) {

                {
                    sf::Vector2f diffCenter = newPos - playerCenter;
                    float d2center = diffCenter.x*diffCenter.x + diffCenter.y*diffCenter.y;
                    if (d2center < influenceRadius * influenceRadius) {
                        float d = std::sqrt(d2center);
                        if (d > 0.f) {
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

                if (dist2 < radius*radius) {
                    float dist = std::sqrt(dist2);
                    if (dist > 0.f) {
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

            // Drag
            p.velocity *= std::max(0.f, 1.f - drag * dt);

            // Jitter
            p.velocity += sf::Vector2f(noiseDist(rng), noiseDist(rng)) * jitterAmt * dt;

            // Swirl around emitter
            {
                sf::Vector2f dir = newPos - m_emitter;
                float d2 = dir.x*dir.x + dir.y*dir.y;
                if (d2 > 0.f) {
                    float inv = 1.f / std::sqrt(d2);
                    sf::Vector2f perp{ -dir.y*inv, dir.x*inv };
                    p.velocity += perp * swirlAmt * dt;
                }
            }

            // Commit new position
            p.position = newPos;

            // Build vertex quad (existing)
            float half      = p.size * 0.5f;
            float lifeRatio = p.lifeTime.asSeconds() / m_lifetime.asSeconds();
            sf::Uint8 alpha = static_cast<sf::Uint8>(lifeRatio * 255);
            auto ts        = m_texture.getSize();
            float tw       = static_cast<float>(ts.x);
            float th       = static_cast<float>(ts.y);

            m_vertices[vIndex+0].position = { newPos.x - half, newPos.y - half };
            m_vertices[vIndex+1].position = { newPos.x + half, newPos.y - half };
            m_vertices[vIndex+2].position = { newPos.x + half, newPos.y + half };
            m_vertices[vIndex+3].position = { newPos.x - half, newPos.y + half };

            m_vertices[vIndex+0].texCoords = { 0.f,  0.f };
            m_vertices[vIndex+1].texCoords = { tw,   0.f };
            m_vertices[vIndex+2].texCoords = { tw,   th };
            m_vertices[vIndex+3].texCoords = { 0.f,  th };

            for (int j = 0; j < 4; ++j)
                m_vertices[vIndex+j].color = sf::Color(255,255,255, alpha);

            vIndex += 4;
            ++i;
        }
    }

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        states.texture = &m_texture;
        target.draw(m_vertices, states);
    }

    void resetParticle(Particle& p) override {
        float theta = angleDeg(rng) * 3.14159265f / 180.f;
        float speed = speedDist(rng);
        p.velocity = { std::cos(theta)*speed, std::sin(theta)*speed };
        p.lifeTime = m_lifetime;
        p.size     = sizeDist(rng);
        p.position = m_emitter;
    }

private:
    static std::uniform_real_distribution<float> angleDeg;
    static std::uniform_real_distribution<float> speedDist;
    static std::uniform_real_distribution<float> sizeDist;
};

inline std::uniform_real_distribution<float> GaseousParticleSystem::angleDeg(90.f, 0.f);
inline std::uniform_real_distribution<float> GaseousParticleSystem::speedDist(10.f, 60.f);
inline std::uniform_real_distribution<float> GaseousParticleSystem::sizeDist(3.f, 6.f);

#endif // GASEOUSPARTICLESYSTEM_H
