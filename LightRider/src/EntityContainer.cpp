#include "EntityContainer.h"

#include <iostream>
#include <assert.h>

#include "Game.h"

EntityContainer::~EntityContainer()
{
    for (unsigned int i = 0; i < m_children.size(); i++)
    {
        Entity* pEntity = m_children[i];
       
        if (pEntity)
        {
            delete pEntity;
            m_children[i] = nullptr;
        }
    }
}

void EntityContainer::add(Entity* pEntity)
{
    assert(pEntity);

    if (pEntity->getId() != -1)
    {
        std::cerr << "Cannot add a stamped entity to a container!" << std::endl;
        return;
    }

    int entityId;

    if (m_availableIds.empty())
    {
        entityId = m_children.size();
        m_children.push_back(pEntity);
    }
    else
    {
        auto idItr = m_availableIds.begin();
        entityId = *idItr;
        m_availableIds.erase(idItr);
        m_children[entityId] = pEntity;
    }

    pEntity->stamp(this, entityId, Game::getInstance().getTick());
}

void EntityContainer::remove(Entity* pEntity)
{
    assert(pEntity);

    int entityId = pEntity->getId();

    if (entityId < 0)
    {
        std::cerr << "Cannot remove an unstamped entity!" << std::endl;
        return;
    }

    if ((unsigned int)entityId >= m_children.size() || pEntity != m_children[entityId])
    {
        std::cerr << "Cannot remove an entity from a container to which it does not belong!" << std::endl;
        return;
    }

    delete pEntity;

    m_children[entityId] = nullptr;
    m_availableIds.insert(entityId);

    for (int i = m_children.size() - 1; i >= 0 && m_children[i] == nullptr; i--)
    {
        m_children.pop_back();
        m_availableIds.erase(i);
    }
}

void EntityContainer::update(float deltaTime)
{
    forAllValidChildren([=](Entity* pEntity) { pEntity->update(deltaTime); });
}

void EntityContainer::postUpdate(float deltaTime)
{
    forAllValidChildren([=](Entity* pEntity) { pEntity->postUpdate(deltaTime); });
}

void EntityContainer::prePhysicsTick(float physicsTimeStep)
{
    forAllValidChildren([=](Entity* pEntity) { pEntity->prePhysicsTick(physicsTimeStep); });
}

void EntityContainer::physicsTick(float physicsTimeStep)
{
    forAllValidChildren([=](Entity* pEntity) { pEntity->physicsTick(physicsTimeStep); });
}

template<typename F>
inline void EntityContainer::forAllValidChildren(F&& f)
{
    long frameHash = Game::getInstance().getTick();

    for (Entity* pEntity : m_children)
        if (pEntity && pEntity->getFrameHash() != frameHash)
            f(pEntity);
}
