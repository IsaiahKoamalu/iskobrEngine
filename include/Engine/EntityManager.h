#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <queue>
#include <bitset>
#include <cstdint>

// Entity ID type alias
using Entity = std::uint32_t;

// Maximum entities alllowed in engine
constexpr Entity MAX_ENTITIES = 5000;

/**
 * @brief Handles the creation and destruction of entity IDs.
 */
class EntityManager {
public:
    EntityManager();

    /**
     * @brief Creates a new entity ID, will reuse recycled IDs
     *        if available.
     * @return Entity ID
     */
    Entity createEntity();

    /**
     * @brief Destroys an existing entity.
     * @param entity
     */
    void destroyEntity(Entity entity);

private:
    std::queue<Entity> availableEntities; // Queue of unused IDs
    std::bitset<MAX_ENTITIES> aliveEntities; // Tracks which IDs are currently in use.
};

#endif