#ifndef SYSTEM_H
#define SYSTEM_H

#include <set>
#include <cstdint>

/**
 *@brief Represents a system that runs logic on entities.
 *       Wherein a derived system defines its own componet filter and behavior.
 */

using Entity = std::uint32_t;

class System {
public:
 std::set<Entity> entities;

 virtual ~System() = default;
};

#endif