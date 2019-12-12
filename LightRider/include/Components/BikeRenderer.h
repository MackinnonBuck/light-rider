#pragma once

#include <glm/gtc/type_ptr.hpp>
#include "Components/MeshRenderer.h"

// Renders the bike body.
// usage: addComponent(const glm::vec3& bikeColor)
class BikeRenderer : public MeshRenderer
{
public:

    // Creates a new bike renderer, rendering using the specified bike color.
    BikeRenderer(const glm::vec3& bikeColor) : MeshRenderer("bikeShader", "bikeTexture", "bikeShape"),
        m_bikeColor(bikeColor) { }

    // Renders the bike body.
    virtual void render();

private:

    // The accent color of the bike.
    glm::vec3 m_bikeColor;
};

