#include "Components/GroundRenderer.h"

#include "Game.h"

bool GroundRenderer::initialize()
{
    m_pSkyTexture = Game::getInstance().getScene()->getAssetManager()->getTexture("skyTexture");

    if (m_pSkyTexture == nullptr)
    {
        return false;
    }

    return true;
}

void GroundRenderer::render()
{
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_pSkyTexture->getTextureId());

    glUniform3fv(getShaderProgram()->getUniform("campos"), 1, &Game::getInstance().getScene()->getActiveCamera()->getTransform()->getPosition()[0]);

    MeshRenderer::render();
}

