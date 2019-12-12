#pragma once

#include "Camera.h"

#include <glm/gtc/type_ptr.hpp>

// Useful for efficiently exploring the scene.
// Usage: addComponent(bool enabled = true, float layerDepth = 0.0f)
class FreeroamCamera : public Camera
{
public:

    // Creates a new FreeroamCamera instance.
    FreeroamCamera(bool enabled = true, float layerDepth = 0.0f) : Camera(enabled, layerDepth),
        m_eulerAngles(0.0f, 0.0f, 0.0f), m_isMoving(false),
        m_initialCursorX(0.0f), m_initialCursorY(0.0f) { }

    // Destroys the FreeroamCamera, removing it from the scene.
    virtual ~FreeroamCamera() { }

    // Updates the FreeroamCamera after other scene elements have finished updating.
    virtual void postUpdate(float deltaTime);

private:

    // The euler angles of the camera. These help ensure that the camera stays gimbal locked.
    glm::vec3 m_eulerAngles;

    // If true, the user is actively moving the camera. This is used to track whether we should
    // make the cursor visible/invisible, and if we should reset the cursor to its initial position.
    bool m_isMoving;

    // The cursor X position when the user starts moving the camera.
    float m_initialCursorX;

    // The cursor Y position when the user starts moving the camera.
    float m_initialCursorY;
};

