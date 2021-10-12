#include "Components/RigidBodyComponent.h"

#include "Game.h"
#include "ConversionUtils.h"

RigidBodyComponent::RigidBodyComponent(btRigidBody::btRigidBodyConstructionInfo& constructionInfo) :
    m_interpolatedTransform(1.0f)
{
    m_pRigidBody = new btRigidBody(constructionInfo);
    
    Game::getInstance().getScene()->getDynamicsWorld()->addRigidBody(m_pRigidBody);

    updateInterpolatedTransform();
}

RigidBodyComponent::~RigidBodyComponent()
{
    getGameObject()->unregisterTransform(this);

    if (m_pRigidBody)
    {
        Game::getInstance().getScene()->getDynamicsWorld()->removeCollisionObject(m_pRigidBody);

        if (m_pRigidBody->getMotionState())
            delete m_pRigidBody->getMotionState();

        btCollisionShape* pCollisionShape = m_pRigidBody->getCollisionShape();

        if (pCollisionShape)
        {
            btCompoundShape* pCompoundShape = dynamic_cast<btCompoundShape*>(pCollisionShape);

            if (pCompoundShape)
            {
                while (pCompoundShape->getNumChildShapes() > 0)
                    pCompoundShape->removeChildShapeByIndex(0);
            }

            delete pCollisionShape;
        }

        delete m_pRigidBody;
    }
}

void RigidBodyComponent::setInfo(CollisionObjectInfo* pInfo)
{
    m_pRigidBody->setUserPointer(pInfo);
}

bool RigidBodyComponent::initialize()
{
    getGameObject()->registerTransform(this);

    return true;
}

void RigidBodyComponent::prePhysicsTick(float physicsTimeStep)
{
    Component::prePhysicsTick(physicsTimeStep);

    updateInterpolatedTransform();
}

void RigidBodyComponent::physicsTick(float physicsTimeStep)
{
    Component::physicsTick(physicsTimeStep);

    updateInterpolatedTransform();
}

void RigidBodyComponent::update(float physicsTimeStep)
{
    Component::update(physicsTimeStep);

    updateInterpolatedTransform();
}

glm::mat4& RigidBodyComponent::getTransformMatrix()
{
    return m_interpolatedTransform;
}

glm::vec3 RigidBodyComponent::getPosition() const
{
    return m_interpolatedTransform[3];
}

glm::quat RigidBodyComponent::getRotation() const
{
    return glm::quat_cast(m_interpolatedTransform);
}

void RigidBodyComponent::setTransformMatrix(glm::mat4 matrix)
{
    m_pRigidBody->getWorldTransform().setFromOpenGLMatrix(&matrix[0][0]);
}

void RigidBodyComponent::setPosition(glm::vec3 position)
{
    m_pRigidBody->getWorldTransform().setOrigin(toBullet(position));
}

void RigidBodyComponent::setRotation(glm::quat rotation)
{
    m_pRigidBody->getWorldTransform().setRotation(toBullet(rotation));
}

void RigidBodyComponent::multiply(const glm::mat4& matrix)
{
    btTransform bulletTransform;
    bulletTransform.setFromOpenGLMatrix(&matrix[0][0]);

    m_pRigidBody->getWorldTransform() *= bulletTransform;
}

void RigidBodyComponent::updateInterpolatedTransform()
{
    btTransform bulletInterpolatedTransform;

    m_pRigidBody->getMotionState()->getWorldTransform(bulletInterpolatedTransform);

    bulletInterpolatedTransform.getOpenGLMatrix(&m_interpolatedTransform[0][0]);
}
