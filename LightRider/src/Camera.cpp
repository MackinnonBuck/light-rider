#include "Camera.h"

#include <iostream>

#include <glm/gtx/euler_angles.hpp>

#include "AssetManager.h"
#include "Game.h"
#include "ProgramMetadata.h"
#include "ProgramOutputMode.h"

Camera::Camera(bool enabled, float layerDepth) :
    m_pSkyTexture(nullptr),
    m_pSkyShape(nullptr),
    m_pSkyShaderProgram(nullptr),
    m_isEnabled(false),
    m_layerDepth(0.0f),
    m_viewMatrix(1.0f),
    m_perspectiveMatrix(1.0f),
    m_sunPosition(glm::vec3(0.0f, 0.0f, 0.0f)),
    m_sunDirection(glm::normalize(glm::vec3(1.0f, -0.5f, -1.0f))),
    m_sunDistance(300.0f),
    m_sunVMatrix(1.0f),
    m_sunPvMatrix(1.0f),
    m_focalLength(0.0f, 0.0f),
    m_fieldOfView(glm::pi<float>() / 4.0f),
    m_nearPlane(0.1f),
    m_farPlane(1000.0f)
{
    if (enabled)
        enable(layerDepth);

    const float fov = 40.0f;
    const float left = -fov;
    const float right = fov;
    const float bottom = -fov;
    const float top = fov;
    const float zNear = 0.1f;
    const float zFar = 500.0f;

    m_sunPMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);

    m_primaryRenderConfiguration =
    {
        // canUseShader
        m_primaryRenderConfiguration.canUseShader = [](Program* pShaderProgram) -> bool
        {
            return true;
        },

        // configureShader
        [this](Program* pShaderProgram) -> void
        {
            pShaderProgram->setVerbose(false);

            GLint pId = pShaderProgram->getUniform("P");
            GLint vId = pShaderProgram->getUniform("V");

            if (pId != -1)
                glUniformMatrix4fv(pId, 1, GL_FALSE, &m_perspectiveMatrix[0][0]);

            if (vId != -1)
                glUniformMatrix4fv(vId, 1, GL_FALSE, &m_viewMatrix[0][0]);

            pShaderProgram->setVerbose(true);

            if (doesProgramHaveDynamicOutput(pShaderProgram))
            {
                glUniform1i(pShaderProgram->getUniform("_outputMode"), (GLint)ProgramOutputMode::STATIC);
            }
        },
    };
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
    Transform* pTransform = getGameObject()->getTransform();
    glm::mat4& transformMatrix = pTransform->getTransformMatrix();

    m_viewMatrix = glm::inverse(transformMatrix);

    computeSunPvMatrix();

    preRender();

    int x, y, width, height;
    getViewport(x, y, width, height);

    glViewport(x, y, width, height);
    glDisable(GL_DEPTH_TEST);

    float aspectRatio = (float)width / (float)height;
    m_perspectiveMatrix = glm::perspective(m_fieldOfView, aspectRatio, m_nearPlane, m_farPlane);
    m_focalLength.x = (1.0f / glm::tan(m_fieldOfView * 0.5f)) * (1.0f / aspectRatio);
    m_focalLength.y = 1.0f / glm::tan(m_fieldOfView * 0.5f);

    glDepthMask(GL_FALSE);

    if (m_pSkyShaderProgram && m_pSkyTexture && m_pSkyShape)
        renderSky(transformMatrix);

    glDepthMask(GL_TRUE);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    renderUnblendedRenderables(m_primaryRenderConfiguration);

    Scene* pScene = Game::getInstance().getScene();
    
    if (pScene->getDebugDrawEnabled())
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        pScene->getDynamicsWorld()->debugDrawWorld();
    }

    postRender();
}

void Camera::computeSunPvMatrix()
{
    Transform* pTransform = getGameObject()->getTransform();
    m_sunPosition = pTransform->getPosition() - m_sunDirection * m_sunDistance;
    m_sunVMatrix = glm::lookAt(m_sunPosition, m_sunPosition + m_sunDirection, glm::vec3(0.0f, 1.0f, 0.0f));
    m_sunPvMatrix = m_sunPMatrix * m_sunVMatrix;
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

void Camera::renderUnblendedRenderables(const RenderConfiguration& configuration)
{
    AssetManager* pAssets = Game::getInstance().getScene()->getAssetManager();

    for (auto shaderProgramNode : pAssets->getRenderTree())
    {
        Program* pShaderProgram = shaderProgramNode.first;

        if (pShaderProgram)
        {
            if (!configuration.canUseShader(pShaderProgram))
            {
                continue;
            }

            pShaderProgram->bind();

            configuration.configureShader(pShaderProgram);
        }

        for (auto textureNode : *shaderProgramNode.second)
        {
            Texture* pTexture = textureNode.first;

            if (pTexture)
            {
                setUpTexture(TextureType::IMAGE, pTexture);
            }

            for (auto pRenderable : *textureNode.second)
            {
                findAndSetUpTexture(pAssets, TextureType::NORMAL, pRenderable->getNormalTextureId());
                pRenderable->render();
            }
        }

        if (pShaderProgram)
            pShaderProgram->unbind();
    }
}

void Camera::renderBlendedRenderables(const RenderConfiguration& configuration)
{
    AssetManager* pAssets = Game::getInstance().getScene()->getAssetManager();

    pAssets->sortBlendedRenderables(this);

    Program* pShaderProgram = nullptr;
    Texture* pColorTexture = nullptr;

    for (BlendedNode* pBlendedNode : pAssets->getSortedBlendedRenderables())
    {
        if (pShaderProgram != pBlendedNode->pShaderProgram)
        {
            if (pShaderProgram)
            {
                pShaderProgram->unbind();
            }

            pShaderProgram = pBlendedNode->pShaderProgram;

            if (pShaderProgram)
            {
                if (!configuration.canUseShader(pShaderProgram))
                {
                    pShaderProgram = nullptr;
                    continue;
                }

                pShaderProgram->bind();

                configuration.configureShader(pShaderProgram);
            }
        }

        if (pColorTexture != pBlendedNode->pTexture)
        {
            pColorTexture = pBlendedNode->pTexture;

            if (pColorTexture)
            {
                setUpTexture(TextureType::IMAGE, pColorTexture);
            }
        }

        Renderable* pRenderable = pBlendedNode->pRenderable;

        findAndSetUpTexture(pAssets, TextureType::NORMAL, pRenderable->getNormalTextureId());

        pRenderable->render();
    }

    if (pShaderProgram)
        pShaderProgram->unbind();
}

void Camera::findAndSetUpTexture(AssetManager* pAssets, TextureType type, const std::string& textureId)
{
    if (textureId.empty())
    {
        return;
    }

    Texture* pTexture = pAssets->getTexture(textureId);

    if (pTexture != nullptr)
    {
        setUpTexture(TextureType::NORMAL, pTexture);
    }
}

void Camera::setUpTexture(TextureType type, Texture* pTexture)
{
    glActiveTexture((GLenum)type);
    glBindTexture(GL_TEXTURE_2D, pTexture->getTextureId());
}

bool Camera::doesProgramHaveDynamicOutput(Program* pShaderProgram)
{
    auto programMetadata = (ProgramMetadata)(uintptr_t)pShaderProgram->getUserPointer();
    return programMetadata == ProgramMetadata::USES_DYNAMIC_OUTPUT;
}
