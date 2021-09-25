#pragma once

#include "Component.h"
#include "Program.h"
#include "Texture.h"
#include "Shape.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Used for rendering the scene.
// Usage: addComponent(bool enabled = true, float layerDepth = 0.0f)
class Camera : public Component
{
public:

    // Creates a new Camera instsance with the provided layer depth and enabled setting.
    Camera(bool enabled = true, float layerDepth = 0.0f);

    // Destroys this camera instance, removing it from the scene.
    virtual ~Camera();

    // Sets the sky background of the camera.
    void setSky(const std::string& skyShaderProgramId, const std::string& skyTextureId, const std::string& skyShapeId);

    // If true, the camera is enabled and actively rendering in the current scene.
    bool isEnabled() const { return m_isEnabled; }

    // Gets the camera's view matrix (updated just before each render cycle).
    glm::mat4& getViewMatrix() { return m_viewMatrix; }

    // Gets the camera's perspective matrix (updated just before each render cycle).
    glm::mat4& getPerspectiveMatrix() { return m_perspectiveMatrix; }

    // Gets the camera's layer depth set when it was enabled.
    float getLayerDepth() const { return m_layerDepth; }

    // Gets the camera's field of view.
    float getFieldOfView() const { return m_fieldOfView; }

    // Gets the camera's near-plane rendering distance.
    float getNearPlane() const { return m_nearPlane; }

    // Gets the camera's far-plane rendering distance.
    float getFarPlane() const { return m_farPlane; }

    // Sets the camera's field of view.
    void setFieldOfView(float fieldOfView) { m_fieldOfView = fieldOfView; }

    // Sets the camera's near-plane rendering distance.
    void setNearPlane(float nearPlane) { m_nearPlane = nearPlane; }

    // Sets the camera's far-plane rendering distance.
    void setFarPlane(float farPlane) { m_farPlane = farPlane; }

    // Enables this camera, rendering to the screen at the given layer depth.
    void enable(float layerDepth);

    // Disables the camera, stopping it from rendering to the screen.
    void disable();

    // Renders scene from this camera.
    void render();

    // Gets the viewport dimensions.
    virtual void getViewport(int& x, int& y, int& width, int& height);
    
protected:

    // Called just before the camera renders the scene.
    virtual void preRender() { }

    // Called just after the camera renders the scene.
    virtual void postRender() { }

    // Renders Renderables with blending disabled.
    void renderUnblendedRenderables();

    // Renders Renderables with blending enabled.
    void renderBlendedRenderables();

private:

    // The shader program used to render the sky.
    Program* m_pSkyShaderProgram;

    // The texture used to render the sky background.
    Texture* m_pSkyTexture;

    // The shape used to render the sky (i.e. sphre, box, etc.)
    Shape* m_pSkyShape;

    // If true, the camera is enabled and actively rendering in the current scene.
    bool m_isEnabled;

    // The layer depth of the camera. Cameras with higher layer depths will render over cameras
    // with lower layer depths.
    float m_layerDepth;

    // The camera's view matrix.
    glm::mat4 m_viewMatrix;

    // The camera's perspective matrix.
    glm::mat4 m_perspectiveMatrix;

    // The field of view of the camera.
    float m_fieldOfView;

    // The near-plane rendering distance.
    float m_nearPlane;

    // The far-plane rendering distance.
    float m_farPlane;

    // Renders the sky.
    void renderSky(const glm::mat4& transformMatrix);

    // Sets up the given shader program for use.
    void setUpShaderProgram(Program* pShaderProgram);

    // Sets up the given image texture for use.
    void setUpImageTexture(Texture* pTexture);
};
