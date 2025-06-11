#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include <unordered_map>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <cstdint>
#include <cassert>

/**
 * This system allows for the destruction of any number of component types in a safe manner.
 */


//Entity ID type alias/
using Entity = std::uint32_t;

class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void entityDestroyed(Entity entity) = 0;
};

/**
 * @brief Template class that stores components of the specific type T.
 *        Where each component is mapped to an entity.
 * @tparam T
 */
template<typename T>
class ComponentArray : public IComponentArray {
public:
    void insert(Entity entity, T component) {
        components[entity] = component;
    }

    void remove(Entity entity) {
        components.erase(entity);
    }

    T& get(Entity entity) {
        assert(components.find(entity) != components.end() && "Component not found.");
        return components[entity];
    }

    bool has(Entity entity) const {
        return components.find(entity) != components.end();
    }

    void entityDestroyed(Entity entity) override {
        components.erase(entity);
    }

private:
    std::unordered_map<Entity, T> components;
};

/**
 * @brief Responsible for managing all component arrays regardless of type.
 *        Also provides generic add/get/remove component functions for any struct type.
 */
class ComponentManager {
public:
    template<typename T>
    void addComponent(Entity entity, T component) {
        getComponentArray<T>()->insert(entity, component);
    }

    template<typename T>
    void removeComponent(Entity entity) {
        getComponentArray<T>()->remove(entity);
    }

    template<typename T>
    T& getComponent(Entity entity) {
        return getComponentArray<T>()->get(entity);
    }

    template<typename T>
    bool hasComponent(Entity entity) {
        return getComponentArray<T>()->has(entity);
    }

    void entityDestroyed(Entity entity) {
        for (auto const& pair : componentArrays) {
            pair.second->entityDestroyed(entity);
        }
    }

private:
    // Maps type_index (component type) -> ComponentArray base ptr.
    std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> componentArrays;

    // Internal helper for retrieving a typed component array.
    template<typename T>
    std::shared_ptr<ComponentArray<T>> getComponentArray(){
        std::type_index typeIndex = typeid(T);

        auto it = componentArrays.find(typeIndex);
        if (it == componentArrays.end()) {
            // if not found, create and store a new component array for this type.
            auto newArray = std::make_shared<ComponentArray<T>>();
            componentArrays[typeIndex] = newArray;
            return newArray;
        }
        return std::static_pointer_cast<ComponentArray<T>>(it->second);
    }
};

#endif