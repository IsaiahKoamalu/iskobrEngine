[Main](index.md)

**7-15-25 KnockBack System & Component**
-
Created KnockBackSystem & KnockBackComponent in order to add a visual effect upon attack collision. The system applies a force to the struck object resulting in a 'knockback' type of reaction. The boolean `isKnockback` is set to true within the collision system when a valid attack is landed on an entity which possess a knock back component. The direction is calculated by taking the normalized difference of the struck entitiy and the attacking entity, *ex:* `direction = normalize(sf::Vector2f{otherPos.x, otherPos.y} - sf::Vector2f{pos.x, pos.y})`, where `normalize()` is a separate helper function I have written and added to the System class. The direction is then multiplied by the struck entities knock back force variable and plugged into the struck entities knock back velocity variable.

```c++
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
