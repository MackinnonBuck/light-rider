#pragma once

#include <glm/gtc/type_ptr.hpp>
#include "Components/MeshRenderer.h"

// Renders the bike body.
// usage: addComponent(int playerId)
class BikeRenderer : public MeshRenderer
{
public:

    // Creates a new bike renderer, rendering using the specified bike color.
    BikeRenderer(int playerId) : MeshRenderer("bikeShader", "bikeTexture", "bikeShape"),
        m_playerId(playerId),
        m_transitionAmount(0.0)
    {
    }

    // Sets the amount that the bike has transitioned into its final form.
    void setTransitionAmount(float transitionAmount) { m_transitionAmount = transitionAmount; }

    // The ID of the player being rendered.
    int getPlayerId() const { return m_playerId; }

    // Renders the bike body.
    virtual void render();

private:

    // The ID of the player controlling the bike.
    int m_playerId;

    // The amount that the bike has transitioned into its final form.
    float m_transitionAmount;
};

