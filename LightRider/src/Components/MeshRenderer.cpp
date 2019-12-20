#include "Components/MeshRenderer.h"

#include "Game.h"

MeshRenderer::MeshRenderer(const std::string& shaderProgramId, const std::string& primaryTextureId,
    const std::string& shapeId, bool useBlending)
    : Renderable(shaderProgramId, primaryTextureId, useBlending),
    m_shapeId(shapeId),
    m_localTransform(1.0f),
    m_pDepthFunc(nullptr)
{
    AssetManager* pAssets = Game::getInstance().getScene()->getAssetManager();

    m_pShaderProgram = pAssets->getShaderProgram(shaderProgramId);
    m_pShaderProgram->setVerbose(false);

    if (m_pShaderProgram->getAttribute("vertexPosition") == -1)
        m_pShaderProgram->addAttribute("vertexPosition");

    if (m_pShaderProgram->getAttribute("vertexNormal") == -1)
        m_pShaderProgram->addAttribute("vertexNormal");

    if (m_pShaderProgram->getAttribute("vertexTexture") == -1)
        m_pShaderProgram->addAttribute("vertexTexture");

    m_pShaderProgram->setVerbose(true);

    m_pShape = pAssets->getShape(shapeId);
}

void MeshRenderer::render()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glm::mat4 globalTransform = getGameObject()->getTransform()->getTransformMatrix() * m_localTransform;

    glUniformMatrix4fv(m_pShaderProgram->getUniform("M"), 1, GL_FALSE, &globalTransform[0][0]);

    if (usesBlending())
        glBlendFunci(0, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pShape->draw(m_pShaderProgram);

    glDisable(GL_CULL_FACE);
}

float MeshRenderer::getDepth(Camera* pCamera) const
{
    return m_pDepthFunc ? m_pDepthFunc(pCamera) : Renderable::getDepth(pCamera);
}
