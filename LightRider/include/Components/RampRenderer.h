#pragma once

#include <glm/gtc/type_ptr.hpp>
#include "Components/MeshRenderer.h"

// Renders the ramp.
// usage: addComponent()
class RampRenderer : public MeshRenderer
{
public:

    // Creates a new ramp renderer.
    RampRenderer() : MeshRenderer("rampShader", "", "rampShape"),
        m_totalTime(0.0f)
    {
    }

    // Updates the ramp.
    virtual void update(float deltaTime);

    // Renders the ramp.
    virtual void render();

private:

    // The total time since the ramp renderer was created.
    float m_totalTime;
};

