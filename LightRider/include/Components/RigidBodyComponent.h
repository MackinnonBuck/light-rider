#pragma once

#include <btBulletDynamicsCommon.h>

#include "Component.h"
#include "Transform.h"
#include "ContactHandler.h"

// Encapsulates a btRigidBody, adding Component and Transform capabilities.
// Usage: addComponent(btRigidBody::btRigidBodyConstructionInfo& constructionInfo)
class RigidBodyComponent : public Component, public Transform
{
public:

    // Creates a new RigidBodyComponent instance from the provided rigid body construction info.
    RigidBodyComponent(btRigidBody::btRigidBodyConstructionInfo& constructionInfo);

    // Destroys the RigidBodyComponent, removing it from the scene and destroying associated physics
    // objects.
    ~RigidBodyComponent();

    // Returns the btRigidBody owned by this RigidBodyComponent.
    btRigidBody* getRigidBody() const { return m_pRigidBody; }

    // Sets the contact handler for this RigidBodyComponent.
    void setContactHandler(ContactHandler* pContactHandler);

    // Initializes the RigidBodyComponent.
    virtual bool initialize();

    // Updates the RigidBodyComponent position just before each physics tick.
    virtual void prePhysicsTick(float physicsTimeStep);

    // Updates the RigidBodyComponent position just after each physics tick.
    virtual void physicsTick(float physicsTimeStep);

    // Updates the RigidBodyComponent each frame.
    virtual void update(float deltaTime);

    // Returns a reference to the interpolated transform matrix representing the rigid body.
    virtual glm::mat4& getTransformMatrix();

    // Returns the interpolated position of the rigid body.
    virtual glm::vec3 getPosition() const;

    // Returns the interpolated rotation of the rigid body.
    virtual glm::quat getRotation() const;

    // Sets this transform's matrix directly.
    virtual void setTransformMatrix(glm::mat4 matrix);

    // Sets the postion of the rigid body.
    virtual void setPosition(glm::vec3 position);

    // Sets the rotation of the rigid body.
    virtual void setRotation(glm::quat rotation);

    // Multiplies the rigid body's transform matrix with the given matrix.
    virtual void multiply(const glm::mat4& matrix);

private:

    // The btRigidBody owned by this RigidBodyComponent.
    btRigidBody* m_pRigidBody;

    // The interpolated body transform updated each frame.
    glm::mat4 m_interpolatedTransform;

    // Updates the interpolated transform of the rigid body.
    void updateInterpolatedTransform();
};

