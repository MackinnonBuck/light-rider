#pragma once

#include <glm/gtc/type_ptr.hpp>
#include "Components/MeshRenderer.h"

// Renders the ramp.
// usage: addComponent()
class GroundRenderer : public MeshRenderer
{
public:
    // Creates a new ramp renderer.
    GroundRenderer() :
        MeshRenderer("groundShader", "groundTexture", "planeShape", true),
        m_pSkyTexture(nullptr)
    {
    }

    // Initializes the ramp renderer.
    virtual bool initialize();

    // Renders the ground.
    virtual void render();

private:
    Texture* m_pSkyTexture;
};

