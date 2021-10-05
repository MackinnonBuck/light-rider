#include "Components/ChunkRenderer.h"

#include "Game.h"

void ChunkRenderer::render()
{
    glUniform1i(getShaderProgram()->getUniform("playerId"), m_playerId);

    MeshRenderer::render();
}

