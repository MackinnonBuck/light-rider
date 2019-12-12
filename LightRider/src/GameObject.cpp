#include "GameObject.h"

#include <iostream>
#include <assert.h>

#include "Game.h"

GameObject* GameObject::create(std::string name, GameObject* pParent)
{
    GameObject* pGameObject = new GameObject(name, pParent);

    if (pParent)
        pParent->registerChild(pGameObject);
    else
        Game::getInstance().getScene()->_registerGameObject(pGameObject);

    return pGameObject;
}

GameObject* GameObject::find(std::string name, GameObject* pParent)
{
    GameObject* pGameObject;
    auto matchesName = [&](GameObject* pGameObject) { return pGameObject->m_name == name; };

    if (pParent)
        pGameObject = pParent->m_children.find<GameObject>(matchesName);
    else
        pGameObject = Game::getInstance().getScene()->findGameObject(matchesName);

    return pGameObject;
}

void GameObject::destroy(GameObject* pGameObject)
{
    if (pGameObject)
        pGameObject->getContainer()->remove(pGameObject);
}

void GameObject::registerTransform(Transform* pTransform)
{
    m_pTransform = pTransform;
}

void GameObject::unregisterTransform(Transform* pTransform)
{
    if (pTransform != m_pTransform)
        return;

    m_defaultTransform.setPosition(m_pTransform->getPosition());
    m_defaultTransform.setRotation(m_pTransform->getRotation());

    m_pTransform = &m_defaultTransform;
}

void GameObject::registerChild(GameObject* pGameObject)
{
    assert(pGameObject);

    if (pGameObject->getId() != -1)
    {
        std::cerr << "Cannot register a child GameObject that has already been registered!";
        return;
    }

    m_children.add(pGameObject);
}

void GameObject::update(float deltaTime)
{
    m_components.update(deltaTime);
    m_children.update(deltaTime);
}

void GameObject::postUpdate(float deltaTime)
{
    m_components.postUpdate(deltaTime);
    m_children.postUpdate(deltaTime);
}

void GameObject::prePhysicsTick(float physicsTimeStep)
{
    m_components.prePhysicsTick(physicsTimeStep);
    m_children.prePhysicsTick(physicsTimeStep);
}

void GameObject::physicsTick(float physicsTimeStep)
{
    m_components.physicsTick(physicsTimeStep);
    m_children.physicsTick(physicsTimeStep);
}
