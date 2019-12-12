#pragma once

#include "EntityContainer.h"

class EntityContainer;

// Represents an entity in the game world.
class Entity
{
public:

    // Creates a new Entity instance.
    Entity() : m_pContainer(nullptr), m_id(-1), m_frameHash(-1) { }

    // Destroys the Entity instance.
    virtual ~Entity() { }

    // Stamps an Entity, assigning it a container and giving it an ID and unique hash for the current frame.
    void stamp(EntityContainer* pContainer, int id, long frameHash);

    // Returns the ID of this Entity.
    int getId() const { return m_id; }

    // Returns the hash of the frame on which this Entity was created.
    int getFrameHash() const { return m_frameHash; }

    // Updates this Entity.
    virtual void update(float deltaTime) = 0;

    // Updates this entity in the post-update stage.
    virtual void postUpdate(float deltaTime) = 0;

    // Called just before the scene's physics are updated.
    virtual void prePhysicsTick(float physicsTimeStep) = 0;

    // Called just after the scene's physics are updated.
    virtual void physicsTick(float physicsTimeStep) = 0;

protected:

    // Returns the container of this Entity.
    EntityContainer* getContainer() const { return m_pContainer; }

private:

    // The parent EntityContainer.
    EntityContainer* m_pContainer;

    // The ID of this Entity.
    int m_id;

    // The hash of hte frame on which this Entity was created.
    long m_frameHash;
};

