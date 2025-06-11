#include "Engine/EntityManager.h"

EntityManager::EntityManager() {
    for (Entity e = 0; e < MAX_ENTITIES; ++e) {
        availableEntities.push(e);
    }
}

Entity EntityManager::createEntity() {
    // Retrieve an available ID
    Entity id = availableEntities.front();
    availableEntities.pop();

    // Mark entity active
    aliveEntities.set(id);

    return id;
}

void EntityManager::destroyEntity(Entity entity) {
    // Mark entity destroyed
    aliveEntities.reset(entity);

    // Recycle ID (push back into queue)
    availableEntities.push(entity);
}

