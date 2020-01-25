#include "Components/OrbitCamera.h"

#include "GameConstants.h"

namespace GC = GameConstants;

bool OrbitCamera::initialize()
{
    if (!Component::initialize())
        return false;

    updatePosition();

    return true;
}

void OrbitCamera::postUpdate(float deltaTime)
{
    m_angle += deltaTime * GC::orbitCameraSpeed;

    updatePosition();

    Component::postUpdate(deltaTime);
}

void OrbitCamera::updatePosition()
{
    getTransform()->setTransformMatrix(
        glm::rotate(glm::mat4(1.0f), m_angle, glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, GC::orbitCameraHeight,
            GC::orbitCameraDistance))
    );
}
