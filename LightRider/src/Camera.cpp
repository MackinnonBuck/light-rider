#include "Camera.h"

#include <iostream>

#include "Game.h"
#include "AssetManager.h"

Camera::Camera(bool enabled, float layerDepth) :
    m_pSkyTexture(nullptr),
    m_pSkyShape(nullptr),
    m_pSkyShaderProgram(nullptr),
    m_isEnabled(false),
    m_layerDepth(0.0f),
    m_viewMatrix(1.0f),
    m_perspectiveMatrix(1.0f),
    m_fieldOfView(glm::pi<float>() / 4.0f),
    m_nearPlane(0.1f),
    m_farPlane(1000.0f)
{
    if (enabled)
        enable(layerDepth);
}

Camera::~Camera()
{
    if (m_isEnabled)
        Game::getInstance().getScene()->_unregisterCamera(this);
}

void Camera::setSky(const std::string& skyShaderProgramId, const std::string& skyTextureId, const std::string& skyShapeId)
{
    AssetManager* pAssets = Game::getInstance().getScene()->getAssetManager();
    Texture* pSkyTexture = pAssets->getTexture(skyTextureId);

    if (pSkyTexture->getTextureType() != TextureType::BACKGROUND)
    {
        std::cerr << "Cannot only set sky textures that have texture type \"BACKGROUND\"!" << std::endl;
        return;
    }

    glActiveTexture((GLenum)TextureType::BACKGROUND);
    glBindTexture(GL_TEXTURE_2D, pSkyTexture->getTextureId());

    m_pSkyTexture = pSkyTexture;

    m_pSkyShaderProgram = pAssets->getShaderProgram(skyShaderProgramId);
    m_pSkyShaderProgram->addAttribute("vertexPosition");
    m_pSkyShaderProgram->addAttribute("vertexNormal");
    m_pSkyShaderProgram->addAttribute("vertexTexture");

    m_pSkyShape = pAssets->getShape(skyShapeId);
}

void Camera::enable(float layerDepth)
{
    if (m_isEnabled)
        return;

    m_isEnabled = true;
    m_layerDepth = layerDepth;

    Game::getInstance().getScene()->_registerCamera(this);
}

void Camera::disable()
{
    if (!m_isEnabled)
        return;

    m_isEnabled = false;

    Game::getInstance().getScene()->_unregisterCamera(this);
}

void Camera::render()
{
    preRender();

    int x, y, width, height;
    getViewport(x, y, width, height);

    glViewport(x, y, width, height);
    glDisable(GL_DEPTH_TEST);

    float aspectRatio = (float)width / (float)height;

    glm::mat4& transformMatrix = getGameObject()->getTransform()->getTransformMatrix();

    m_viewMatrix = glm::inverse(transformMatrix);
    m_perspectiveMatrix = glm::perspective(m_fieldOfView, aspectRatio, m_nearPlane, m_farPlane);

    glDepthMask(GL_FALSE);

    if (m_pSkyShaderProgram && m_pSkyTexture && m_pSkyShape)
        renderSky(transformMatrix);

    glDepthMask(GL_TRUE);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    renderUnblendedRenderables();

    Scene* pScene = Game::getInstance().getScene();
    
    if (pScene->getDebugDrawEnabled())
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        pScene->getDynamicsWorld()->debugDrawWorld();
    }

    postRender();
}

void Camera::getViewport(int& x, int& y, int& width, int& height)
{
    x = y = 0;
    glfwGetFramebufferSize(Game::getInstance().getWindowHandle(), &width, &height);
}

void Camera::renderSky(const glm::mat4& transformMatrix)
{
    m_pSkyShaderProgram->bind();

    GLint pId = m_pSkyShaderProgram->getUniform("P");
    GLint vId = m_pSkyShaderProgram->getUniform("V");
    GLint mId = m_pSkyShaderProgram->getUniform("M");

    if (pId != -1 && vId != -1 && mId != -1)
    {
        glActiveTexture((GLenum)TextureType::BACKGROUND);
        glBindTexture(GL_TEXTURE_2D, m_pSkyTexture->getTextureId());

        glm::mat4 skyModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(transformMatrix[3])) *
            glm::rotate(glm::mat4(1.0f), glm::pi<float>() * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));

        glUniformMatrix4fv(pId, 1, GL_FALSE, &m_perspectiveMatrix[0][0]);
        glUniformMatrix4fv(vId, 1, GL_FALSE, &m_viewMatrix[0][0]);
        glUniformMatrix4fv(mId, 1, GL_FALSE, &skyModelMatrix[0][0]);

        m_pSkyShape->draw(m_pSkyShaderProgram);
    }
    else
    {
        std::cerr << "Sky shader is missing one of the following uniforms: P, V, M." << std::endl;
    }

    m_pSkyShaderProgram->unbind();
}

void Camera::renderUnblendedRenderables()
{
    for (auto shaderProgramNode : Game::getInstance().getScene()->getAssetManager()->getRenderTree())
    {
        Program* pShaderProgram = shaderProgramNode.first;

        if (pShaderProgram)
            setUpShaderProgram(pShaderProgram);

        for (auto textureNode : *shaderProgramNode.second)
        {
            Texture* pTexture = textureNode.first;

            if (pTexture)
                setUpImageTexture(pTexture);

            for (auto renderable : *textureNode.second)
                renderable->render();
        }

        if (pShaderProgram)
            pShaderProgram->unbind();
    }
}

void Camera::renderBlendedRenderables()
{
    AssetManager* pAssets = Game::getInstance().getScene()->getAssetManager();

    pAssets->sortBlendedRenderables(this);

    Program* pShaderProgram = nullptr;
    Texture* pTexture = nullptr;

    for (BlendedNode* pBlendedNode : pAssets->getSortedBlendedRenderables())
    {
        if (pShaderProgram != pBlendedNode->pShaderProgram)
        {
            if (pShaderProgram)
                pShaderProgram->unbind();

            pShaderProgram = pBlendedNode->pShaderProgram;

            if (pShaderProgram)
                setUpShaderProgram(pShaderProgram);
        }

        if (pTexture != pBlendedNode->pTexture)
        {
            pTexture = pBlendedNode->pTexture;

            if (pTexture)
                setUpImageTexture(pTexture);
        }

        pBlendedNode->pRenderable->render();
    }

    if (pShaderProgram)
        pShaderProgram->unbind();
}

void Camera::setUpShaderProgram(Program* pShaderProgram)
{
    pShaderProgram->bind();
    pShaderProgram->setVerbose(false);

    GLint pId = pShaderProgram->getUniform("P");
    GLint vId = pShaderProgram->getUniform("V");

    if (pId != -1)
        glUniformMatrix4fv(pId, 1, GL_FALSE, &m_perspectiveMatrix[0][0]);

    if (vId != -1)
        glUniformMatrix4fv(vId, 1, GL_FALSE, &m_viewMatrix[0][0]);

    pShaderProgram->setVerbose(true);
}

void Camera::setUpImageTexture(Texture* pTexture)
{
    glActiveTexture((GLenum)TextureType::IMAGE);
    glBindTexture(GL_TEXTURE_2D, pTexture->getTextureId());
}
