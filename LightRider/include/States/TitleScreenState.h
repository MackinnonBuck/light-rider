#pragma once

#include "State.h"

class TitleScreenState : public State
{
    virtual ~TitleScreenState();

    virtual void initialize();

    virtual void update(float deltaTime);
};

