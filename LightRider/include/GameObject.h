#pragma once

#include <string>

#include "EntityContainer.h"
#include "SimpleTransform.h"

// Represents an object in the game.
class GameObject : public Entity
{
public:

    // Creates a new GameObject with the given name and parent.
    static GameObject* create(std::string name, GameObject* pParent);

    // Creates a new GameObject with the given name.
    static GameObject* create(std::string name) { return create(name, nullptr); }

    // Creates a new GameObject with the given parent and default name of "GameObject".
    static GameObject* create(GameObject* pParent) { return create("GameObject", pParent); };

    // Finds the first GameObject of the given name from the given parent. If no parent is provided,
    // Scene-level GameObjects are searched.
    static GameObject* find(std::string name, GameObject* pParent = nullptr);

    // Destroys the given GameObject.
    static void destroy(GameObject* pGameObject);

    // Returns the name of the GameObject.
    std::string getName() const { return m_name; }

    // Returns a reference to the GameObject's transform.
    Transform* getTransform() const { return m_pTransform; }

    // Sets the GameObject's transform to reference the provided transform.
    void registerTransform(Transform* pTransform);

    // Unregisters the GameObject's current transform if it matches the provided transform,
    // resetting it to a simple transform with the existing position and rotation.
    void unregisterTransform(Transform* pTransform);

    // Finds the first Component of the given type from the GameObject.
    template<typename T>
    T* getComponent() const { return m_pComponents->find<T>(); }

    // Adds a Component of the given type to the GameObject. If the Component could not be
    // initialized, a null pointer is returned.
    template<typename T, typename... U>
    T* addComponent(U... args)
    {
        T* pComponent = new T(args...);

        if (pComponent->_registerGameObject(this))
        {
            m_pComponents->add(pComponent);
            return pComponent;
        }
        else
        {
            delete pComponent;
            return nullptr;
        }
    }

    // Removes the first Component found of the given type from the GameObject.
    template<typename T>
    void removeComponent()
    {
        T* pComponent = m_pComponents->find<T>();

        if (pComponent)
            m_pComponents->remove(pComponent);
    }

    // Frees the GameObject.
    virtual ~GameObject();

    // Initializes the GameObject.
    virtual void initialize() { }

    // Updates the GameObject.
    virtual void update(float deltaTime);

    // Updates the GameObject in the post-update stage.
    virtual void postUpdate(float deltaTime);

    // Updates the GameObject just before the physics world updates.
    virtual void prePhysicsTick(float physicsTimeStep);

    // Updates the GameObject just after the physics world updates.
    virtual void physicsTick(float physicsTimeStep);

private:

    // The name of the GameObject.
    std::string m_name;

    // The parent GameObject.
    GameObject* m_pParent;

    // The default GameObject transform.
    SimpleTransform m_defaultTransform;

    // The GameObject's transform.
    Transform* m_pTransform;

    // The container for child GameObjects.
    EntityContainer* m_pChildren;

    // The container for Components.
    EntityContainer* m_pComponents;

    // Creates a new GameObject instance.
    GameObject(std::string name, GameObject* pParent) : m_name(name), m_pParent(pParent),
        m_defaultTransform(), m_pTransform(&m_defaultTransform),
        m_pChildren(new EntityContainer()), m_pComponents(new EntityContainer()) { }

    // Registers a new chlid GameObject.
    void registerChild(GameObject* pGameObject);
};
