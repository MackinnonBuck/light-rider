#include "Components/PlayerCamera.h"

#include <glm/gtx/matrix_interpolation.hpp>

#include "GameConstants.h"

namespace GC = GameConstants;

void PlayerCamera::postUpdate(float deltaTime)
{
    switch (m_mode)
    {
    case PlayerCameraMode::INTRO:
        processIntroMode(deltaTime);
        break;
    case PlayerCameraMode::FOLLOW:
        processFollowMode(deltaTime);
        break;
    }

    ProcessedCamera::postUpdate(deltaTime);
}

void PlayerCamera::processIntroMode(float deltaTime)
{
    Transform* pTransform = getTransform();
    glm::vec3 position = pTransform->getPosition();

    if (position.x < 0.0f)
        pTransform->setPosition(position + glm::vec3(deltaTime * GC::introCameraSpeed, 0.0f, 0.0f));
    else
        pTransform->setPosition(position - glm::vec3(deltaTime * GC::introCameraSpeed, 0.0f, 0.0f));
}

void PlayerCamera::processFollowMode(float deltaTime)
{
    glm::mat4 targetMatrix;

    if (m_pJoystick)
    {
        targetMatrix = m_pJoystick->getButton(GC::opponentViewButton) == GLFW_PRESS ?
            getOpponentViewTransform() :
            getPlayerViewTransform();
    }
    else
    {
        targetMatrix = Game::getInstance().isKeyDown(m_cameraControls.opponentView) ?
            getOpponentViewTransform() :
            getPlayerViewTransform();
    }

    Transform* pCameraTransform = getTransform();

    glm::vec3 cameraPosition = pCameraTransform->getPosition();
    glm::quat cameraRotation = pCameraTransform->getRotation();

    glm::vec3 targetPosition = glm::vec3(targetMatrix[3]);
    glm::quat targetRotation = glm::quat_cast(targetMatrix);

    glm::vec3 lerpedPosition = glm::mix(cameraPosition, targetPosition, std::fmin(1.0f, GC::cameraTranslationStiffness * deltaTime));
    glm::quat lerpedRotation = glm::slerp(cameraRotation, targetRotation, std::fmin(1.0f, GC::cameraRotationStiffness * deltaTime));

    pCameraTransform->setPosition(lerpedPosition);
    pCameraTransform->setRotation(lerpedRotation);
}

glm::mat4 PlayerCamera::getPlayerViewTransform()
{
    Transform* pPlayerTransform = m_pPlayerObject->getTransform();
    glm::mat4& playerMatrix = pPlayerTransform->getTransformMatrix();

    float yaw = -glm::atan(playerMatrix[0][2], playerMatrix[0][0]);

    return
        glm::translate(glm::mat4(1.0f), pPlayerTransform->getPosition()) *
        glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::translate(glm::mat4(1.0f), glm::vec3(GC::cameraNudgeOffset, GC::cameraHeight, GC::cameraDistance));
}

glm::mat4 PlayerCamera::getOpponentViewTransform()
{
    Transform* pPlayerTransform = m_pPlayerObject->getTransform();
    Transform* pOpponentTransform = m_pOpponentObject->getTransform();

    glm::vec3 cameraPosition = pPlayerTransform->getPosition() -
        glm::normalize(pOpponentTransform->getPosition() - pPlayerTransform->getPosition()) * GC::cameraDistance;
    cameraPosition.y = pPlayerTransform->getPosition().y + GC::cameraHeight;

    float yaw = glm::atan(cameraPosition.x - pOpponentTransform->getPosition().x, cameraPosition.z - pOpponentTransform->getPosition().z);

    return
        glm::translate(glm::mat4(1.0f), cameraPosition) *
        glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0.0f, 1.0f, 0.0f));
}
