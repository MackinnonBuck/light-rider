#include "Components/BikeRenderer.h"

#include "Game.h"

void BikeRenderer::render()
{
    glUniform1i(getShaderProgram()->getUniform("playerId"), m_playerId);
    glUniform1f(getShaderProgram()->getUniform("transitionAmount"), m_transitionAmount);

    MeshRenderer::render();
}
