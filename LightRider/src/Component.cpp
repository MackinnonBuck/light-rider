#include "Component.h"

#include <assert.h>

GameObject* Component::getGameObject() const
{
    return m_pGameObject;
}

Transform* Component::getTransform() const
{
    return m_pGameObject->getTransform();
}

bool Component::_registerGameObject(GameObject* pGameObject)
{
    assert(m_pGameObject == nullptr);

    m_pGameObject = pGameObject;

    return initialize();
}
