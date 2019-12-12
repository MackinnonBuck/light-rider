#pragma once

#include <vector>
#include <set>
#include <functional>

#include "Entity.h"

class Entity;

class EntityContainer
{
public:

    // Creates a new EntityContainer instance.
    EntityContainer() : m_children(), m_availableIds() { }

    // Destroys the EntityContainer.
    ~EntityContainer();

    // Adds the given Entity to this container.
    void add(Entity* pEntity);

    // Removes the given Entity from this container.
    void remove(Entity* pEntity);

    // Updates all Entities in this container.
    void update(float deltaTime);

    // Updates all Entities in this container in the post-update stage.
    void postUpdate(float deltaTime);

    // Runs the prePhysicsTick callback on all Entities in this container.
    void prePhysicsTick(float physicsTimeStep);

    // Runs the physicsTick callback on all Entities in this container.
    void physicsTick(float physicsTimeStep);

    // Finds the first child of the provided type.
    template<typename T>
    T* find() const
    {
        for (Entity* pEntity : m_children)
        {
            if (!pEntity)
                continue;

            T* pCastedEntity = dynamic_cast<T*>(pEntity);

            if (pCastedEntity)
                return pCastedEntity;
        }

        return nullptr;
    }

    // Finds the first child of the provided type that passes the provided predicate.
    template<typename T, typename F>
    T* find(F& f) const
    {
        for (Entity* pEntity : m_children)
        {
            if (!pEntity)
                continue;

            T* pCastedEntity = dynamic_cast<T*>(pEntity);

            if (pCastedEntity && f(pCastedEntity))
                return pCastedEntity;
        }

        return nullptr;
    }

private:

    // The child Entities, indexed by ID.
    std::vector<Entity*> m_children;

    // A set of available IDs that point to empty slots in the child list.
    std::set<int> m_availableIds;

    // Executes the given operaton on each child that is instantiated and at least one frame old.
    template<typename F>
    inline void forAllValidChildren(F&& f);
};

