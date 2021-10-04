#include "Components/MenuCamera.h"

MenuCamera::MenuCamera(const glm::vec3& targetPosition, const glm::quat& targetRotation) :
    m_targetPosition(targetPosition),
    m_targetRotation(targetRotation)
{
}

void MenuCamera::postUpdate(float deltaTime)
{
    Component::postUpdate(deltaTime);

    Transform* pCameraTransform = getTransform();

    glm::vec3 cameraPosition = pCameraTransform->getPosition();
    glm::quat cameraRotation = pCameraTransform->getRotation();

    glm::vec3 lerpedPosition = glm::mix(cameraPosition, m_targetPosition, std::fmin(1.0f, 5 * deltaTime));
    glm::quat lerpedRotation = glm::slerp(cameraRotation, m_targetRotation, std::fmin(1.0f, 5 * deltaTime));

    pCameraTransform->setPosition(lerpedPosition);
    pCameraTransform->setRotation(lerpedRotation);
}
