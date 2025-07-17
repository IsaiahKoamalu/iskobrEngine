#ifndef GASEOUSPARTICLESYSTEM_H
#define GASEOUSPARTICLESYSTEM_H

#include "Engine/Core/UpdateContext.h"
#include "ParticleSystemBase.h"

class GaseousParticleSystem : public ParticleSystem {
public:
    void update(const UpdateContext& ctxt) override {
        ComponentManager& component = *ctxt.component;
        const float dt   = ctxt.tDt.asSeconds();
        size_t  vIndex   = 0;

        // Resize the vertex array to match the live particles
        m_vertices.clear();
        m_vertices.resize(m_particles.size() * 4);

        assert(m_collisionSystem && "ParticleSystem: collision pointer null");

        // Iterate in “remove‑while‑iterating” style
        for (size_t i = 0; i < m_particles.size(); ) {
            auto& p = m_particles[i];

            // 1) age
            p.lifeTime -= ctxt.tDt;
            if (p.lifeTime <= sf::Time::Zero) {
                // swap‑pop dead particle
                p = m_particles.back();
                m_particles.pop_back();
                continue;
            }

            // 2) forces (e.g. gravity)
            p.velocity -= sf::Vector2f{0.f, 120.f} * dt;

            // 3) integrate
            sf::Vector2f newPos = p.position + p.velocity * dt;

            constexpr float bounce = 0.8;
            constexpr float tile = 16.f;

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
            // 5) build your quad
            float half = p.size * 0.5f;
            float lifeRatio = p.lifeTime.asSeconds() / m_lifetime.asSeconds();
            sf::Uint8 alpha = static_cast<sf::Uint8>(lifeRatio * 255);

            // set the texture coordinates if you’re using a sprite‑sheet
            // e.g. sf::FloatRect uv = pickFrameFor(p);
            // then assign uv.left,uv.top,uv.width,uv.height to each vertex.texCoords

            // positions (TL,TR,BR,BL)
            m_vertices[vIndex+0].position = { newPos.x - half, newPos.y - half };
            m_vertices[vIndex+1].position = { newPos.x + half, newPos.y - half };
            m_vertices[vIndex+2].position = { newPos.x + half, newPos.y + half };
            m_vertices[vIndex+3].position = { newPos.x - half, newPos.y + half };

            auto ts = m_texture.getSize();
            float tw = float(ts.x), th = float(ts.y);

            // texture coords (make them match your texture)
            m_vertices[vIndex+0].texCoords = {0.f,0.f};
            m_vertices[vIndex+1].texCoords = {tw,0.f};
            m_vertices[vIndex+2].texCoords = {tw,th};
            m_vertices[vIndex+3].texCoords = {0.f,th};

            // tint/fade color
            sf::Color c(255,255,255, alpha);
            m_vertices[vIndex+0].color = c;
            m_vertices[vIndex+1].color = c;
            m_vertices[vIndex+2].color = c;
            m_vertices[vIndex+3].color = c;

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
inline std::uniform_real_distribution<float> GaseousParticleSystem::angleDeg(90.f, 0.f);
inline std::uniform_real_distribution<float> GaseousParticleSystem::speedDist(10.f, 60.f);
inline std::uniform_real_distribution<float> GaseousParticleSystem::sizeDist(3.f, 6.f);

#endif