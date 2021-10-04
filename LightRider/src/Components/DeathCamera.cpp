#include "Components/DeathCamera.h"

#include <glm/gtx/euler_angles.hpp>

#include "GameConstants.h"

namespace GC = GameConstants;

void DeathCamera::setDeadObject(GameObject* pDeadObject)
{
    m_pDeadObject = pDeadObject;

    updatePosition();
}

bool DeathCamera::initialize()
{
    if (!ProcessedCamera::initialize())
        return false;

    getTransform()->setRotation(glm::eulerAngleX(GC::cameraDeathAngle));

    return true;
}

void DeathCamera::postUpdate(float deltaTime)
{
    if (m_pDeadObject)
        updatePosition();

    ProcessedCamera::postUpdate(deltaTime);
}

void DeathCamera::updatePosition()
{
    getTransform()->setPosition(m_pDeadObject->getTransform()->getPosition() +
        glm::vec3(0.0f, GC::cameraDeathHeight, GC::cameraDeathDistance));
}
