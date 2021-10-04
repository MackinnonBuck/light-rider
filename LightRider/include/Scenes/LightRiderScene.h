#pragma once

#include "Scene.h"

// Contains common scene functionality to scenes in LightRider.
class LightRiderScene : public Scene
{
protected:

    // Loads common LightRider game assets.
    virtual void loadAssets();
};

