#include "Components/RampRenderer.h"

#include "Game.h"

void RampRenderer::update(float deltaTime)
{
    m_totalTime += deltaTime;
}

void RampRenderer::render()
{
    glUniform1f(getShaderProgram()->getUniform("time"), m_totalTime);

    MeshRenderer::render();
}

