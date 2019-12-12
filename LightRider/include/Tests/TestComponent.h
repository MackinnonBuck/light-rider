#pragma once

#include "Component.h"

class TestComponent : public Component
{
public:

    virtual void update(float deltaTime);

    virtual void postUpdate(float deltaTime);

    virtual void prePhysicsTick(float physicsTimeStep);

    virtual void physicsTick(float physicsTimeStep);
};

