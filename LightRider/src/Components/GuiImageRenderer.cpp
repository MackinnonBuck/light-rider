#include "Components/GuiImageRenderer.h"

#include "Game.h"

GuiImageRenderer::GuiImageRenderer(const std::string& shaderId, const std::string& textureId, float depth) : 
    Renderable(shaderId, textureId, true),
    m_depth(depth)
{
    AssetManager* pAssets = Game::getInstance().getScene()->getAssetManager();

    m_pShaderProgram = pAssets->getShaderProgram(shaderId);
    m_pShaderProgram->setVerbose(false);

    if (m_pShaderProgram->getAttribute("vertexPosition") == -1)
        m_pShaderProgram->addAttribute("vertexPosition");

    if (m_pShaderProgram->getAttribute("vertexNormal") == -1)
        m_pShaderProgram->addAttribute("vertexNormal");

    if (m_pShaderProgram->getAttribute("vertexTexture") == -1)
        m_pShaderProgram->addAttribute("vertexTexture");

    m_pShaderProgram->setVerbose(true);

    m_pShape = pAssets->getShape("planeShape");
}

void GuiImageRenderer::render()
{
    glm::mat4& transformMatrix = getTransform()->getTransformMatrix();
    int viewX, viewY, viewWidth, viewHeight;

    Game::getInstance().getScene()->getActiveCamera()->getViewport(viewX, viewY, viewWidth, viewHeight);

    glUniformMatrix4fv(m_pShaderProgram->getUniform("M"), 1, GL_FALSE, &transformMatrix[0][0]);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pShape->draw(m_pShaderProgram);
}

float GuiImageRenderer::getDepth(Camera* pCamera) const
{
    return m_depth;
}
