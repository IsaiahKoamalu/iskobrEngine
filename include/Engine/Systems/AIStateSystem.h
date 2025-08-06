#ifndef AISTATESYSTEM_H
#define AISTATESYSTEM_H

#include "Engine/ComponentManager.h"
#include "Engine/System.h"
#include "Engine/Components/AIComponent.h"
#include "Engine/Components/Position.h"
#include "Engine/Components/SeekComponent.h"
#include "Engine/Core/UpdateContext.h"


class AIStateSystem : public System {
public:
    void update(const UpdateContext& ctxt) override {}
};

#endif
