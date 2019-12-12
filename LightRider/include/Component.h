#pragma once

#include "Entity.h"
#include "GameObject.h"
#include "Transform.h"

// Represents a Component of a GameObject with custom behavior.
class Component : public Entity
{
public:

    // Destroys the given component.
    static void destroy(Component* pComponent) { pComponent->getContainer()->remove(pComponent); }

    // Creates a new Component instance.
    // Note: Components should be created via GameObject::addComponent<T>().
    Component() : m_pGameObject(nullptr) { }

    // Destroys this Component instance.
    // Note: Components should be destroyed via GameObject::removeComponent<T>().
    virtual ~Component() { }

    // Gets the GameObject that owns this component.
    GameObject* getGameObject() const;

    // Gets the Transform of the GameObject that owns this component.
    Transform* getTransform() const;

    // Called just after the component was added to a GameObject.
    // If true is returned, the component initialized successfully. Otherwise,
    // the component could not initialize, and it will be promptly removed from its
    // parent GameObject.
    virtual bool initialize() { return true; }

    // Updates the component.
    virtual void update(float deltaTime) { }

    // Updates the component in the post-update stage.
    virtual void postUpdate(float deltaTime) { }

    // Called just before the physics simulation is advanced.
    virtual void prePhysicsTick(float physicsTimeStep) { }

    // Called just after the physics simulation is advanced.
    virtual void physicsTick(float physicsTimeStep) { }
    
    // Registers the parent GameObject for this component and initializes this component.
    // Returns true if initialization succeeded, otherwise false.
    // For internal use only - use GameObject::addComponent<T>() instead.
    bool _registerGameObject(GameObject* pGameObject);

private:

    // The GameObject that owns this component.
    GameObject* m_pGameObject;
};

