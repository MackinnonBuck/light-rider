#pragma once

#include "State.h"

class TitleScreenState : public State
{
public:

    virtual void resume();

    virtual void pause();

    virtual void update(float deltaTime);
};

