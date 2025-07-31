#ifndef NAVIGATIONCOMMANDSYSTEM_H
#define NAVIGATIONCOMMANDSYSTEM_H

#include "Engine/ComponentManager.h"
#include "Engine/System.h"
#include "Engine/Components/NavigationComponent.h"
#include "Engine/Core/UpdateContext.h"

class NavigationCommandSystem : public System {
public:
    void goTo(Entity e, const TileCoord& goal, UpdateContext& ctxt) {
        auto& nav = ctxt.component->getComponent<NavigationComponent>(e);
        nav.targetTile = goal;
        nav.needsRepath = true;
        entities.insert(e);
    }

    void update(const UpdateContext& ctxt) override {}
};

#endif
