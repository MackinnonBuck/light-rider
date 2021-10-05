#pragma once

#include <glm/gtc/type_ptr.hpp>
#include "Components/MeshRenderer.h"

// Renders the chunk.
// usage: addComponent()
class ChunkRenderer : public MeshRenderer
{
public:
    // Creates a new chunk renderer.
    ChunkRenderer(int playerId) : MeshRenderer("chunkShader", "chunkTexture", "chunkShape"),
        m_playerId(playerId)
    {
        setCullingEnabled(false);
    }

    // Renders the chunk.
    virtual void render();

private:
    // The ID of the player that this chunk was spawned from.
    int m_playerId;
};


