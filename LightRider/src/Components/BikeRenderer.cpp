#include "Components/BikeRenderer.h"

#include "Game.h"

void BikeRenderer::render()
{
    glUniform3fv(getShaderProgram()->getUniform("bikeColor"), 1, &m_bikeColor[0]);
    glUniform3fv(getShaderProgram()->getUniform("campos"), 1, &Game::getInstance().getScene()->getActiveCamera()->getTransform()->getPosition()[0]);

    MeshRenderer::render();
}
