#pragma once

#include "Program.h"
#include "Camera.h"

// Extends the Camera component, integrating post processing.
class ProcessedCamera : public Camera
{
public:

    // Creates a new ProcessedCamera instance.
    ProcessedCamera(bool enabled = true, float layerDepth = 0.0f);
    
    // Destroys the ProcessedCamera instance.
    virtual ~ProcessedCamera();

    // Sets the offset ratio of the camera to screen size (each axis has a range from 0 to 1).
    void setOffsetRatio(const glm::vec2& offsetRatio);

    // Sets the scale ratio of the camera to screen size.
    void setSizeRatio(const glm::vec2& sizeRatio);

    // Returns the offset ratio of the camera to screen size.
    glm::vec2 getOffsetRatio() const { return m_offsetRatio; }

    // Returns the size ratio of the camera to screen size.
    glm::vec2 getSizeRatio() const { return m_sizeRatio; }

protected:

    // Returns the viewport dimensions.
    virtual void getViewport(int& x, int& y, int& width, int& height);

    // Updates the frame buffer if necessary, then binds it as the active render target.
    virtual void preRender();

    // Draws the processed frame buffer to the screen.
    virtual void postRender();

private:

    // The shader program used to apply SSR to the scene.
    Program* m_pSsrShader;

    // The shader program used to generate an HDR texture of the scene.
    Program* m_pHdrShader;

    // The shader program used for performing FXAA antialiasing.
    Program* m_pFxaaShader;

    // The shader used to blur the bright scene elements.
    Program* m_pBlurShader;

    // The shader that combines the blur with the scene's first pass to create a 
    // "bloom" effect.
    Program* m_pBloomShader;

    // The frame buffer that the scene gets rnedered to.
    GLuint m_primaryFrameBuffer;

    // The color buffer for the primary frame buffer.
    GLuint m_primaryColorAttachments[4];

    // The frame buffer to render the scene to with SSR applied.
    GLuint m_ssrFrameBuffer;

    // The color buffer to store the scene with SSR applied.
    GLuint m_ssrColorBuffer;

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

    // The offset ratio of the camera to screen size.
    glm::vec2 m_offsetRatio;

    // The size ratio of the camera to screen size.
    glm::vec2 m_sizeRatio;

    // Creates a new frame buffer, freeing the old one if it exists.
    void createFrameBuffers();

    // Deltes all frame buffers and textures.
    inline void deleteBuffers();
};
