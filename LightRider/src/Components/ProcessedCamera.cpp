#include "Components/ProcessedCamera.h"

#include "Game.h"

constexpr int SHADOW_MAP_WIDTH = 4096;
constexpr int SHADOW_MAP_HEIGHT = 4096;

ProcessedCamera::ProcessedCamera(bool enabled, float layerDepth) :
    Camera(enabled, layerDepth),
    m_offsetRatio(glm::zero<glm::vec2>()),
    m_sizeRatio(glm::one<glm::vec2>()),
    m_areFrameBuffersDirty(true),
    m_defaultBufferWidth(-1),
    m_defaultBufferHeight(-1),
    m_bufferWidth(-1),
    m_bufferHeight(-1),
    m_depthRenderBuffer(0),
    m_blendedDepthRenderBuffer(0),
    m_deferredFrameBuffer(0),
    m_deferredColorBuffer(0),
    m_primaryFrameBuffer(0),
    m_primaryColorBuffer(0),
    m_primaryPositionBuffer(0),
    m_primaryNormalBuffer(0),
    m_primaryMaterialBuffer(0),
    m_shadowMapFrameBuffer(0),
    m_shadowMapDepthBuffer(0),
    m_fxaaFrameBuffer(0),
    m_fxaaColorBuffer(0),
    m_hdrFrameBuffer(0),
    m_hdrColorBuffer(0),
    m_pingPongFrameBuffers{0, 0},
    m_pingPongColorBuffers{0, 0}
{
    AssetManager* pAssets = Game::getInstance().getScene()->getAssetManager();

    m_pShadowShader = pAssets->getShaderProgram("shadowShader");
    m_pDeferredShader = pAssets->getShaderProgram("deferredShader");
    m_pPostShader = pAssets->getShaderProgram("postShader");
    m_pFxaaShader = pAssets->getShaderProgram("fxaaShader");
    m_pBlurShader = pAssets->getShaderProgram("blurShader");
    m_pBloomShader = pAssets->getShaderProgram("bloomShader");

    m_pDeferredShader->bind();
    glUniform1i(m_pDeferredShader->getUniform("gColor"), 0);
    glUniform1i(m_pDeferredShader->getUniform("gPosition"), 1);
    glUniform1i(m_pDeferredShader->getUniform("gNormal"), 2);
    glUniform1i(m_pDeferredShader->getUniform("gMaterial"), 3);
    glUniform1i(m_pDeferredShader->getUniform("shadowMap"), 4);
    m_pDeferredShader->unbind();

    m_pPostShader->bind();
    glUniform1i(m_pPostShader->getUniform("screenTexture"), 0);
    m_pPostShader->unbind();

    m_pFxaaShader->bind();
    glUniform1i(m_pFxaaShader->getUniform("screenTexture"), 0);
    m_pFxaaShader->unbind();

    m_pBlurShader->bind();
    glUniform1i(m_pBlurShader->getUniform("screenTexture"), 0);
    m_pBlurShader->unbind();

    m_pBloomShader->bind();
    glUniform1i(m_pBloomShader->getUniform("screenTexture"), 0);
    glUniform1i(m_pBloomShader->getUniform("blurTexture"), 1);
    glUniform1i(m_pBloomShader->getUniform("bloom"), 1);
    glUniform1f(m_pBloomShader->getUniform("exposure"), 1.0f);
    m_pBloomShader->unbind();

    GLfloat quadVertices[] =
    {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f
    };

    GLfloat quadTextureCoordinates[] =
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    glGenVertexArrays(1, &m_quadVertexArrayObject);
    glBindVertexArray(m_quadVertexArrayObject);

    glGenBuffers(1, &m_quadVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVertexBufferObject);

    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glGenBuffers(1, &m_quadTextureBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadTextureBufferObject);

    glBufferData(GL_ARRAY_BUFFER, sizeof(quadTextureCoordinates), quadTextureCoordinates, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);
}

ProcessedCamera::~ProcessedCamera()
{
    if (m_bufferWidth != -1 || m_bufferHeight != -1)
        deleteBuffers();

    glDeleteBuffers(1, &m_quadVertexBufferObject);
    glDeleteBuffers(1, &m_quadTextureBufferObject);
    glDeleteVertexArrays(1, &m_quadVertexArrayObject);
}

void ProcessedCamera::setOffsetRatio(const glm::vec2& offsetRatio)
{
    m_offsetRatio = offsetRatio;
    m_areFrameBuffersDirty = true;
}

void ProcessedCamera::setSizeRatio(const glm::vec2& sizeRatio)
{
    m_sizeRatio = sizeRatio;
    m_areFrameBuffersDirty = true;
}

void ProcessedCamera::getViewport(int& x, int& y, int& width, int& height)
{
    x = y = 0;
    width = m_bufferWidth;
    height = m_bufferHeight;
}

void ProcessedCamera::preRender()
{
    int width;
    int height;

    glfwGetFramebufferSize(Game::getInstance().getWindowHandle(), &width, &height);

    if (width != m_defaultBufferWidth || height != m_defaultBufferHeight)
    {
        m_defaultBufferWidth = width;
        m_defaultBufferHeight = height;
        m_areFrameBuffersDirty = true;
    }

    if (m_areFrameBuffersDirty)
    {
        m_areFrameBuffersDirty = false;
        createFrameBuffers();
    }

    // Shadow map pass
    renderShadowMap();

    // First render pass
    glBindFramebuffer(GL_FRAMEBUFFER, m_primaryFrameBuffer);
    
    unsigned int attachments[4] =
    {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3,
    };
    glDrawBuffers(4, attachments);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ProcessedCamera::renderShadowMap()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFrameBuffer);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);

    glDisable(GL_BLEND);

    m_pShadowShader->bind();
    glUniformMatrix4fv(m_pShadowShader->getUniform("P"), 1, GL_FALSE, &getSunPMatrix()[0][0]);
    glUniformMatrix4fv(m_pShadowShader->getUniform("V"), 1, GL_FALSE, &getSunVMatrix()[0][0]);

    for (auto shaderProgramNode : Game::getInstance().getScene()->getAssetManager()->getRenderTree())
    {
        for (auto textureNode : *shaderProgramNode.second)
        {
            for (auto renderable : *textureNode.second)
                renderable->renderDepth(m_pShadowShader);
        }
    }

    m_pShadowShader->unbind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ProcessedCamera::postRender()
{
    unsigned int attachments[1] =
    {
        GL_COLOR_ATTACHMENT0,
    };
    glDrawBuffers(1, attachments);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glBindVertexArray(m_quadVertexArrayObject);

    glBindFramebuffer(GL_FRAMEBUFFER, m_deferredFrameBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_pDeferredShader->bind();

    glUniform3fv(m_pDeferredShader->getUniform("campos"), 1, &getTransform()->getPosition()[0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_primaryColorBuffer);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_primaryPositionBuffer);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_primaryNormalBuffer);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_primaryMaterialBuffer);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, m_shadowMapDepthBuffer);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    m_pDeferredShader->unbind();

    glBindFramebuffer(GL_FRAMEBUFFER, m_fxaaFrameBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_pFxaaShader->bind();

    glUniform1f(m_pFxaaShader->getUniform("screenWidth"), (float)m_bufferWidth);
    glUniform1f(m_pFxaaShader->getUniform("screenHeight"), (float)m_bufferHeight);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_deferredColorBuffer);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    m_pFxaaShader->unbind();

    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_primaryFrameBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fxaaFrameBuffer);
    glBlitFramebuffer(0, 0, m_bufferWidth, m_bufferHeight, 0, 0, m_bufferWidth, m_bufferHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fxaaFrameBuffer);

    glBindVertexArray(0);

    glEnable(GL_BLEND);
    renderBlendedRenderables();
    glDisable(GL_BLEND);

    glBindVertexArray(m_quadVertexArrayObject);

    glBindFramebuffer(GL_FRAMEBUFFER, m_hdrFrameBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_pPostShader->bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fxaaColorBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    m_pPostShader->unbind();

    bool horizontal = true, firstIteration = true;
    int amount = 10;

    m_pBlurShader->bind();

    for (int i = 0; i < amount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_pingPongFrameBuffers[horizontal]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUniform1i(m_pBlurShader->getUniform("horizontal"), horizontal);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, firstIteration ? m_hdrColorBuffer : m_pingPongColorBuffers[!horizontal]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        horizontal = !horizontal;
        
        if (firstIteration)
            firstIteration = false;
    }

    m_pBlurShader->unbind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    int x = (int)(m_defaultBufferWidth * m_offsetRatio.x);
    int y = (int)(m_defaultBufferHeight * m_offsetRatio.y);

    glViewport(x, y, m_bufferWidth, m_bufferHeight);

    m_pBloomShader->bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fxaaColorBuffer);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_pingPongColorBuffers[!horizontal]);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);

    m_pBloomShader->unbind();
}

void ProcessedCamera::createFrameBuffers()
{
    if (m_bufferWidth != -1 || m_bufferHeight != -1)
        deleteBuffers();

    m_bufferWidth = (int)(m_defaultBufferWidth * m_sizeRatio.x);
    m_bufferHeight = (int)(m_defaultBufferHeight * m_sizeRatio.y);

    // Primary frame buffer
    glGenFramebuffers(1, &m_primaryFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_primaryFrameBuffer);

    // Primary color texture
    glGenTextures(1, &m_primaryColorBuffer);
    glBindTexture(GL_TEXTURE_2D, m_primaryColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_bufferWidth, m_bufferHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_primaryColorBuffer, 0);
    
    // Primary position texture
    glGenTextures(1, &m_primaryPositionBuffer);
    glBindTexture(GL_TEXTURE_2D, m_primaryPositionBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_bufferWidth, m_bufferHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_primaryPositionBuffer, 0);

    // Primary normal texture
    glGenTextures(1, &m_primaryNormalBuffer);
    glBindTexture(GL_TEXTURE_2D, m_primaryNormalBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_bufferWidth, m_bufferHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_primaryNormalBuffer, 0);

    // Primary material texture
    glGenTextures(1, &m_primaryMaterialBuffer);
    glBindTexture(GL_TEXTURE_2D, m_primaryMaterialBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, m_bufferWidth, m_bufferHeight, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_primaryMaterialBuffer, 0);

    glGenRenderbuffers(1, &m_depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_bufferWidth, m_bufferHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    GLenum frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (frameBufferStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Unable to create primary frame buffer: " << frameBufferStatus << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Shadow map frame buffer
    glGenFramebuffers(1, &m_shadowMapFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFrameBuffer);

    glGenTextures(1, &m_shadowMapDepthBuffer);

    glBindTexture(GL_TEXTURE_2D, m_shadowMapDepthBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(glm::vec3(1.0)));
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMapDepthBuffer, 0);

    frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (frameBufferStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Unable to create shadow map frame buffer: " << frameBufferStatus << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Deferred frame buffer
    glGenFramebuffers(1, &m_deferredFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_deferredFrameBuffer);

    glGenTextures(1, &m_deferredColorBuffer);

    glBindTexture(GL_TEXTURE_2D, m_deferredColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_bufferWidth, m_bufferHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_deferredColorBuffer, 0);

    frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (frameBufferStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Unable to create deferred frame buffer: " << frameBufferStatus << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // FXAA frame buffer
    glGenFramebuffers(1, &m_fxaaFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fxaaFrameBuffer);

    glGenTextures(1, &m_fxaaColorBuffer);

    glBindTexture(GL_TEXTURE_2D, m_fxaaColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_bufferWidth, m_bufferHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fxaaColorBuffer, 0);

    glGenRenderbuffers(1, &m_blendedDepthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_blendedDepthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_bufferWidth, m_bufferHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_blendedDepthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (frameBufferStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Unable to create FXAA frame buffer: " << frameBufferStatus << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // HDR frame buffer
    glGenFramebuffers(1, &m_hdrFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_hdrFrameBuffer);

    glGenTextures(1, &m_hdrColorBuffer);

    glBindTexture(GL_TEXTURE_2D, m_hdrColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_bufferWidth, m_bufferHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_hdrColorBuffer, 0);

    frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (frameBufferStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Unable to create HDR frame buffer: " << frameBufferStatus << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Ping-pong frame buffers.
    glGenFramebuffers(2, m_pingPongFrameBuffers);
    glGenTextures(2, m_pingPongColorBuffers);

    for (int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_pingPongFrameBuffers[i]);
        glBindTexture(GL_TEXTURE_2D, m_pingPongColorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_bufferWidth, m_bufferHeight, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingPongColorBuffers[i], 0);

        GLenum frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        if (frameBufferStatus != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "Unable to create ping pong frame buffer " << i << ": " << frameBufferStatus << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ProcessedCamera::deleteBuffers()
{
    glDeleteFramebuffers(1, &m_primaryFrameBuffer);
    glDeleteFramebuffers(1, &m_shadowMapFrameBuffer);
    glDeleteFramebuffers(1, &m_deferredFrameBuffer);
    glDeleteFramebuffers(1, &m_fxaaFrameBuffer);
    glDeleteFramebuffers(1, &m_hdrFrameBuffer);
    glDeleteFramebuffers(2, m_pingPongFrameBuffers);
    glDeleteRenderbuffers(1, &m_depthRenderBuffer);
    glDeleteRenderbuffers(1, &m_blendedDepthRenderBuffer);
    glDeleteTextures(1, &m_shadowMapDepthBuffer);
    glDeleteTextures(1, &m_deferredColorBuffer);
    glDeleteTextures(1, &m_primaryColorBuffer);
    glDeleteTextures(1, &m_primaryPositionBuffer);
    glDeleteTextures(1, &m_primaryNormalBuffer);
    glDeleteTextures(1, &m_primaryMaterialBuffer);
    glDeleteTextures(1, &m_fxaaColorBuffer);
    glDeleteTextures(1, &m_hdrColorBuffer);
    glDeleteTextures(2, m_pingPongColorBuffers);
}
