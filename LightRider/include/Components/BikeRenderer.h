#pragma once

#include "Components/MeshRenderer.h"

// Renders the bike body.
// usage: addComponent(int playerId)
class BikeRenderer : public MeshRenderer
{
public:

    // Creates a new bike renderer, rendering using the specified bike color.
    BikeRenderer(int playerId) : MeshRenderer("bikeShader", "bikeTexture", "bikeShape", false, true),
        m_playerId(playerId),
        m_transitionAmount(0.0)
    {
    }

    // Gets the amount that the bike has transitioned into its final form.
    float getTransitionAmount() const { return m_transitionAmount; }

    // Sets the amount that the bike has transitioned into its final form.
    void setTransitionAmount(float transitionAmount);

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

