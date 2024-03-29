#pragma once

#include "GameObject.h"
#include "ComputeProgram.h"
#include "Camera.h"
#include "Program.h"
#include "Texture.h"

// Extends the Camera component, integrating post processing.
class ProcessedCamera : public Camera
{
public:

    // Creates a new ProcessedCamera instance.
    ProcessedCamera(bool enabled = true, float layerDepth = 0.0f);
    
    // Destroys the ProcessedCamera instance.
    virtual ~ProcessedCamera();

    // Sets the subject of the camera. This is used to determine the voxel light map origin.
    void setSubject(GameObject* pSubject) { m_pSubject = pSubject; }

    // Gets the subject of the camera. This is used to determine the voxel light map origin.
    GameObject* getSubject() const { return m_pSubject; }

    // Sets the offset ratio of the camera to screen size (each axis has a range from 0 to 1).
    void setOffsetRatio(const glm::vec2& offsetRatio);

    // Sets the scale ratio of the camera to screen size.
    void setSizeRatio(const glm::vec2& sizeRatio);

    // Returns the offset ratio of the camera to screen size.
    glm::vec2 getOffsetRatio() const { return m_offsetRatio; }

    // Returns the size ratio of the camera to screen size.
    glm::vec2 getSizeRatio() const { return m_sizeRatio; }
    
    // Gets the shadow map texture ID.
    virtual GLuint getShadowMapTextureId() const { return m_shadowMapDepthBuffer; }

    virtual void update(float deltaTime);

    virtual void postUpdate(float deltaTime);

protected:

    // Initializes the camera component.
    virtual bool initialize();

    // Returns the viewport dimensions.
    virtual void getViewport(int& x, int& y, int& width, int& height);

    // Updates the frame buffer if necessary, then binds it as the active render target.
    virtual void preRender();

    // Draws the processed frame buffer to the screen.
    virtual void postRender();

private:

    // The subject of the camera. This dictates where the voxel light map should be centered.
    GameObject* m_pSubject;

    // The shader used to render objects that cast shadows.
    Program* m_pShadowShader;

    // The shader program used for deferred rendering.
    Program* m_pDeferredShader;

    // The shader used to compute global illumination.
    Program* m_pGiShader;

    // The shader program used for deferred rendering, including blended objects.
    Program* m_pBlendedDeferredShader;

    // The shader program used to render the scene's first pass.
    Program* m_pPostShader;

    // The shader program used for performing FXAA antialiasing.
    Program* m_pFxaaShader;

    // The shader used to blur the bright scene elements.
    Program* m_pBlurShader;

    // The shader that combines the blur with the scene's first pass to create a 
    // "bloom" effect.
    Program* m_pBloomShader;

    // The sky texture used for environment sampling.
    Texture* m_pSkyTexture;

    // The compute shader used to determine the overall luminance of the scene.
    ComputeProgram* m_pLuminanceComputeShader;

    // The compute shader used to clear the voxel texture.
    ComputeProgram* m_pVoxelClearComputeShader;

    // The compute shader used to combine each voxel map component texture.
    ComputeProgram* m_pVoxelCombineComputeShader;

    // The compute shader used to generate a mipmap for the voxel texture.
    ComputeProgram* m_pVoxelMipmapComputeShader;

    // The frame buffer that the scene gets rendered to.
    GLuint m_primaryFrameBuffer;

    // The color buffer for the primary frame buffer.
    GLuint m_primaryColorBuffer;

    // The position buffer for the primary frame buffer.
    GLuint m_primaryPositionBuffer;

    // The normal buffer for the primary frame buffer.
    GLuint m_primaryNormalBuffer;

    // The material info buffer for the primary frame buffer.
    GLuint m_primaryMaterialBuffer;

    // The frame buffer used to store the shadow map.
    GLuint m_shadowMapFrameBuffer;

    // The frame buffer that handles deferred rendering.
    GLuint m_deferredFrameBuffer;

    // The color buffer for the deferred frame buffer.
    GLuint m_deferredColorBuffer;

    // The texture containing noise for HBAO and global illumination.
    GLuint m_noiseTexture;

    // The frame buffer used to render global illumination.
    GLuint m_giFrameBuffer;

    // The color buffer for storing the ambient occlusion output.
    GLuint m_giAoColorBuffer;

    // The color buffer for storing indirect lighting output.
    GLuint m_giIndirectColorBuffer;

    // The frame buffer to store the antialiased scene.
    GLuint m_fxaaFrameBuffer;

    // The color buffer for the FXAA frame buffer.
    GLuint m_fxaaColorBuffer;

    // The HDR frame buffer object.
    GLuint m_hdrFrameBuffer;

    // The frame buffer texture.
    GLuint m_hdrColorBuffer;

    // The frmae buffer object used for processing bloom.
    GLuint m_pingPongFrameBuffers[2];

    // The color buffers used to process bloom.
    GLuint m_pingPongColorBuffers[2];

    // The depth render buffer.
    GLuint m_depthRenderBuffer;

    // The depth render buffer for the shadow map.
    GLuint m_shadowMapDepthBuffer;

    // The framebuffer used to render to the voxel light map.
    GLuint m_voxelFrameBuffer;

    // The color buffer used in rendering the voxel light map.
    // This buffer is only attached in order to make the framebuffer valid - we don't write to it.
    GLuint m_voxelColorBuffer;

    // The 3D texture storing the voxel light map.
    GLuint m_voxelMapTexture;

    // The 3D textures representing each component in the voxel light map.
    GLuint m_voxelMapTextureComponents[4];

    // The VAO for the quad to display the texture.
    GLuint m_quadVertexArrayObject;

    // The VBO for the quad vertices.
    GLuint m_quadVertexBufferObject;

    // The VBO for the quad texture coordinates.
    GLuint m_quadTextureBufferObject;

    // The width of the default frame buffer.
    int m_defaultBufferWidth;

    // The height of the default frame buffer.
    int m_defaultBufferHeight;

    // The camera's frame buffer width.
    int m_bufferWidth;

    // The camera's frame buffer height.
    int m_bufferHeight;

    // If true, a property of the frmae buffer has been modified, so the frame buffer
    // needs to be recreated.
    bool m_areFrameBuffersDirty;

    // The render configuration used to render the voxel map.
    RenderConfiguration m_voxelMapRenderConfiguration;

    // The offset ratio of the camera to screen size.
    glm::vec2 m_offsetRatio;

    // The size ratio of the camera to screen size.
    glm::vec2 m_sizeRatio;

    // The current exposure level.
    float m_currentExposure;

    // The target exposure level.
    float m_targetExposure;

    // The subject position snapped to the voxel grid.
    glm::vec3 m_snappedSubjectPosition;

    // The current perspective matrix used to render the voxel map.
    glm::mat4 m_voxelPerspectiveMatrix;

    // The current view matrix used to render the voxel map.
    glm::mat4 m_voxelViewMatrix;

    // Creates a new frame buffer, freeing the old one if it exists.
    void createFrameBuffers();

    // Deltes all frame buffers and textures.
    inline void deleteBuffers();
    
    // Renders the current scene to the shadow map.
    void renderShadowMap();

    // Performs a deferred rendering pass.
    void renderDeferred();

    // Renders the scene from the given camera matrices into the voxel light map.
    void renderToVoxelMap();

    glm::vec3 getVoxelSnappedSubjectPosition() const;

    void generateNoise(glm::vec3* noise, unsigned int size);
};
