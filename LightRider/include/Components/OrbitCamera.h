#pragma once

#include "Components/ProcessedCamera.h"

class OrbitCamera : public Component
{
public:

    // Initializes a new OrbitCamera instance.
    OrbitCamera() : m_angle(0.0f) { }

    // Initializes the OrbitCamera.
    virtual bool initialize();

    // Updates the OrbitCamera after the rest of the scene has updated.
    virtual void postUpdate(float deltaTime);

private:

    // The angle of the camera.
    float m_angle;

    // Updates the camera's position.
    void updatePosition();
};

