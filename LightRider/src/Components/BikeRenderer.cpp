#include "Components/BikeRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Game.h"

void BikeRenderer::render()
{
    glUniform1i(getShaderProgram()->getUniform("playerId"), m_playerId);
    glUniform1f(getShaderProgram()->getUniform("transitionAmount"), m_transitionAmount);

    MeshRenderer::render();
}

void BikeRenderer::setTransitionAmount(float transitionAmount)
{
    m_transitionAmount = glm::clamp(transitionAmount, 0.0f, 1.0f);
}
