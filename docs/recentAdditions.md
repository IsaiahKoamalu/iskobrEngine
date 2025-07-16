[Main](index.md)

**7-16-25 UpdateContext.h and New Particle System(s) Structure**
-
All system update methods are now called with a single parameter, an object `const UpdateContext& ctxt`. UpdateContext is a struct which is defined in the new UpdateContext.h file located in the include/Engine/Core directory. This new change allows for a much needed increase of freedom in regards to the implementation of the update method of System subclasses. I have also changed the dynamic of the particle system(s), there is now a base particle system class from which other varying particle system subclasses are derived (only 2 as of right now). The improved flexablility introduced by the update context makes the creation and application of new systems, such as the derived particle systems, much smoother and cleaner.

```C++
// ----New Update Context----
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
//------------------------------------------------------------------

//----The Base Particle System----
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
//--------------------------------------------------------------------------------------

//----Example of the New Update Method Being Applied in the RenderSystem(just a short excerpt of the beginning)----
class RenderSystem : public System {
public:
    void update(const UpdateContext& ctxt) override{
        sf::RenderWindow& window = *ctxt.window;
        ComponentManager& components = *ctxt.component;
        std::vector<sf::Drawable*> drawables = ctxt.drawables;
//.....
for (auto obj : drawables){
  window.draw(*obj);
}
};
```

**7-15-25 KnockBack System & Component**
-
Created KnockBackSystem & KnockBackComponent in order to add a visual effect upon attack collision. The system applies a force to the struck object resulting in a 'knockback' type of reaction. The boolean `isKnockback` is set to true within the collision system when a valid attack is landed on an entity which possess a knock back component. The direction is calculated by taking the normalized difference of the struck entitiy and the attacking entity, *ex:* `direction = normalize(sf::Vector2f{otherPos.x, otherPos.y} - sf::Vector2f{pos.x, pos.y})`, where `normalize()` is a separate helper function I have written and added to the System class. The direction is then multiplied by the struck entities knock back force variable and plugged into the struck entities knock back velocity variable.

```C++
//---------Two additional helper functions--------

float length(const sf::Vector2f v) {
  return std::sqrt(v.x * v.x + v.y * v.y);
}

sf::Vector2f normalize(const sf::Vector2f v){
  float length = std::sqrt(v.x * v.x + v.y * v.y);
  return length != 0 ? v / length : sf::Vector2f(0.f, 0.f);
}
//----------------------------------------------------------

struct KnockBackComponent{
  sf::Vector2f velocity;
  float force;
  float decay;
  bool isKnockback = false;
};

class KnockBackSystem : public System {
public:
   void update(ComponentManager& components, float dt) {
        for (Entity entity : entities) {
            if (!components.hasComponent<KnockBackComponent>(entity)) continue;

            auto& knock = components.getComponent<KnockBackComponent>(entity);
            if(!knock.isKnockback) continue;

            auto& pos = components.getComponent<Position>(entity);

            pos.x += knock.velocity.x * dt;
            pos.y += knock.velocity.y * dt;

            sf::Vector2f direction = normalize(knock.velocity);
            float decayAmt = knock.decay * dt;

            if (length(knock.velocity) <= decayAmt) {
                knock.velocity = {0.f, 0.f};
                knock.isKnockback = false;
            }else {
                knock.velocity -= direction * decayAmt;
            }
        }
    }
};
```
