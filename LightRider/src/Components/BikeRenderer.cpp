#include "Components/BikeRenderer.h"

#include "Game.h"

void BikeRenderer::render()
{
    glUniform1i(getShaderProgram()->getUniform("playerId"), m_playerId);
    glUniform3fv(getShaderProgram()->getUniform("campos"), 1, &Game::getInstance().getScene()->getActiveCamera()->getTransform()->getPosition()[0]);

    MeshRenderer::render();
}
