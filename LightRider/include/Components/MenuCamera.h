#pragma once

#include "Component.h"

class MenuCamera : public Component
{
public:

    MenuCamera(const glm::vec3& targetPosition, const glm::quat& targetRotation);

    virtual void postUpdate(float deltaTime);

private:

    glm::vec3 m_targetPosition;

    glm::quat m_targetRotation;
};

