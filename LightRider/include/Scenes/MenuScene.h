#pragma once

#include "Scenes/LightRiderScene.h"

class MenuScene : public LightRiderScene
{
public:

    // Creates a new MenuScene instance.
    MenuScene() { }

    // Initializes the MenuScene.
    virtual void initialize();

    virtual void postUpdate(float deltaTime);

protected:

    virtual void loadAssets();
};

