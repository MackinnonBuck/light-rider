#include "Components/BikeController.h"

#include <iostream>
#include <GLFW/glfw3.h>

#include "Game.h"
#include "GameConstants.h"
#include "ConversionUtils.h"
#include "Components/LightTrail.h"

namespace GC = GameConstants;

BikeController::~BikeController()
{
    RigidBodyComponent* pRigidBodySibling = getGameObject()->getComponent<RigidBodyComponent>();

    if (pRigidBodySibling)
        pRigidBodySibling->setContactHandler(nullptr);
}

void BikeController::debugDrawPhysicsData(
    btIDebugDraw* pDebugDrawer,
    const btTransform& transform,
    const btVector3& localForward,
    const btVector3& frontWheelLeft,
    const btVector3& rearWheelLeft,
    const btVector3& linearVelocity,
    const btVector3& frontImpulse,
    const btVector3& rearImpulse)
{
    btVector3 localFrontWheelContactPoint = toBullet(m_localFrontWheelContactPoint);
    btVector3 localRearWheelContactPoint = toBullet(m_localRearWheelContactPoint);

    pDebugDrawer->drawLine(transform * localFrontWheelContactPoint,
        transform * localFrontWheelContactPoint + frontImpulse * 0.1f, btVector3(1.0f, 1.0f, 0.0f));

    pDebugDrawer->drawLine(transform * localRearWheelContactPoint,
        transform * localRearWheelContactPoint + rearImpulse * 0.1f, btVector3(1.0f, 1.0f, 0.0f));

    pDebugDrawer->drawLine(transform * localFrontWheelContactPoint,
        transform * localFrontWheelContactPoint + frontWheelLeft * 2, btVector3(0.0f, 0.5f, 1.0f));

    pDebugDrawer->drawLine(transform * localRearWheelContactPoint,
        transform * localRearWheelContactPoint + rearWheelLeft * 2, btVector3(0.0f, 0.5f, 1.0f));

    pDebugDrawer->drawLine(transform.getOrigin(),
        transform.getOrigin() + linearVelocity, btVector3(1.0f, 0.5f, 0.0f));

    pDebugDrawer->drawLine(transform.getOrigin(),
        transform.getOrigin() + localForward * 4, btVector3(0.0f, 1.0f, 0.0f));
}

bool BikeController::initialize()
{
    m_pRigidBodyComponent = getGameObject()->getComponent<RigidBodyComponent>();

    if (!m_pRigidBodyComponent)
        return false;

    btRigidBody* pRigidBody = m_pRigidBodyComponent->getRigidBody();

    m_pRigidBodyComponent->setContactHandler(this);
    pRigidBody->setDamping(0.0f, 0.0f);
    pRigidBody->setSleepingThresholds(0, 0);
    pRigidBody->setFriction(0.0f);
    pRigidBody->setRestitution(0.0f);

    return true;
}

void BikeController::prePhysicsTick(float physicsTimeStep)
{
    if (m_localFrontWheelContactPoint == glm::zero<glm::vec3>() && m_localRearWheelContactPoint == glm::zero<glm::vec3>())
        updateAerialPhysics();
    else
        updateDrivingPhysics();

    m_jumpTimer -= physicsTimeStep;

    if (m_jumpTimer < 0.0f)
        m_jumpTimer = 0.0f;
}

void BikeController::handleContact(const ContactInfo& contactInfo, btCollisionObject* pBodySelf, btCollisionObject* pBodyOther)
{
    if (pBodyOther->getCollisionShape()->getShapeType() == COMPOUND_SHAPE_PROXYTYPE)
    {
        kill();
        return;
    }

    btVector3& localPoint = contactInfo.localPointSelf;

    if (localPoint.z() < 0)
    {
        if (localPoint.y() < m_localFrontWheelContactPoint.y)
        {
            m_localFrontWheelContactPoint = toGlm(localPoint);
            m_frontWheelGroundNormal = toGlm(contactInfo.manifoldPoint.m_normalWorldOnB);
        }
    }
    else
    {
        if (localPoint.y() < m_localRearWheelContactPoint.y)
        {
            m_localRearWheelContactPoint = toGlm(localPoint);
            m_rearWheelGroundNormal = toGlm(contactInfo.manifoldPoint.m_normalWorldOnB);
        }
    }
}

void BikeController::updateDrivingPhysics()
{
    btVector3 localFrontWheelContactPoint = toBullet(m_localFrontWheelContactPoint);
    btVector3 localRearWheelContactPoint = toBullet(m_localRearWheelContactPoint);
    btVector3 frontWheelGroundNormal = toBullet(m_frontWheelGroundNormal);
    btVector3 rearWheelGroundNormal = toBullet(m_rearWheelGroundNormal);

    btRigidBody* pRigidBody = m_pRigidBodyComponent->getRigidBody();
    btTransform& transform = pRigidBody->getWorldTransform();
    btVector3 linearVelocity = pRigidBody->getLinearVelocity();
    btVector3 angularVelocty = pRigidBody->getAngularVelocity();

    float speed = linearVelocity.length();

    btVector3 localUp = transform.getBasis().getColumn(1);
    btVector3 localForward = -transform.getBasis().getColumn(2);

    btVector3 frontWheelLeft = -localForward.cross(frontWheelGroundNormal);
    btVector3 rearWheelLeft = -localForward.cross(rearWheelGroundNormal);

    float acceleration;

    switch (m_controlMode)
    {
    case BikeControlMode::LOCKED:
        acceleration = GC::bikeLockedAccelerationInput;
        break;
    case BikeControlMode::AUTO_ACCELERATE:
        acceleration = 1.0f;
        break;
    case BikeControlMode::MANUAL_ACCELERATE:
        acceleration = getActionInput(BikeActions::ACCELERATE);
        break;
    default:
        acceleration = 0.0f;
        break;
    }

    float jump =
        m_controlMode == BikeControlMode::LOCKED || m_jumpTimer > 0.0f ||
        localFrontWheelContactPoint.isZero() || localRearWheelContactPoint.isZero() ? 0.0f :
        getActionInput(BikeActions::JUMP);

    float turn =
        m_controlMode == BikeControlMode::LOCKED ? 0.0f :
        getActionInput(BikeActions::TURN);

    float targetFrontWheelAngle = 0.0f;

    localForward.setY(0.0f);

    if (acceleration >= GC::accelerationDeadzone)
        pRigidBody->applyCentralImpulse((GC::bikeMaxSpeed - speed) * localForward * GC::bikeAccelerationForceFactor * acceleration);
    else
        pRigidBody->applyCentralImpulse(-linearVelocity * GC::bikeDecelerationForceFactor);

    if (jump > 0.0f)
    {
        pRigidBody->applyCentralImpulse(localUp * GC::bikeJumpImpulse);

        m_jumpTimer = GC::bikeJumpCooldown;
        m_frontWheelAngle = 0.0f;
    }
    else if (abs(turn) >= GC::turningDeadzone)
    {
        targetFrontWheelAngle = GC::bikeWheelMaxTurningAngle * turn;
        localUp = localUp.rotate(localForward, GC::bikeMaxLeanAngle * turn);
    }

    if (m_frontWheelAngle < targetFrontWheelAngle)
        m_frontWheelAngle = std::fmin(m_frontWheelAngle + GC::bikeWheelTurningVelocity, targetFrontWheelAngle);
    else if (m_frontWheelAngle > targetFrontWheelAngle)
        m_frontWheelAngle = std::fmax(m_frontWheelAngle - GC::bikeWheelTurningVelocity, targetFrontWheelAngle);

    frontWheelLeft = frontWheelLeft.rotate(frontWheelGroundNormal, m_frontWheelAngle);

    btVector3 predictedUp = localUp;

    if (angularVelocty.length() > 0)
    {
        btQuaternion angle;
        angle.setRotation(angularVelocty, angularVelocty.length() * (GC::bikeBalanceStability / GC::bikeBalanceSpeed));
        predictedUp = btMatrix3x3(angle) * localUp;
    }

    btVector3 torque = predictedUp.cross(btVector3(0, 1, 0));
    pRigidBody->applyTorqueImpulse(torque * GC::bikeBalanceSpeed * GC::bikeBalanceSpeed);

    float frontFrictionFactor = linearVelocity.dot(frontWheelLeft);
    float rearFrictionFactor = linearVelocity.dot(rearWheelLeft);

    btVector3 frontImpulse = -frontWheelLeft * frontFrictionFactor * GC::bikeWheelForceFactor;
    btVector3 rearImpulse = -rearWheelLeft * rearFrictionFactor * GC::bikeWheelForceFactor;

    frontImpulse.setY(0.0f);
    rearImpulse.setY(0.0f);

    pRigidBody->applyCentralImpulse(frontImpulse);
    pRigidBody->applyCentralImpulse(rearImpulse);

    btVector3 frontTorque;
    btVector3 rearTorque;
    
    if (speed > GC::bikeWheelTorqueSpeedEpsilon)
    {
        float invSpeed = 1.0f / speed;

        frontTorque = frontWheelGroundNormal * frontFrictionFactor * localFrontWheelContactPoint.z() * invSpeed * GC::bikeWheelTorqueFactor;
        rearTorque = rearWheelGroundNormal * rearFrictionFactor * localRearWheelContactPoint.z() * invSpeed * GC::bikeWheelTorqueFactor;
    }
    else
    {
        frontTorque = btVector3(0.0f, 0.0f, 0.0f);
        rearTorque = btVector3(0.0f, 0.0f, 0.0f);
    }

    pRigidBody->applyTorqueImpulse(frontTorque);
    pRigidBody->applyTorqueImpulse(rearTorque);

    btVector3 avgNormal = (frontWheelGroundNormal + rearWheelGroundNormal) * 0.5f;
    float correctionFactor = avgNormal.dot(angularVelocty);

    pRigidBody->applyTorqueImpulse(-avgNormal * correctionFactor * GC::bikeDampingTorqueFactor);

    Scene* pScene = Game::getInstance().getScene();

    if (pScene->getDebugDrawEnabled())
    {
        btIDebugDraw* pDebugDrawer = pScene->getDynamicsWorld()->getDebugDrawer();

        debugDrawPhysicsData(
            pDebugDrawer,
            transform,
            localForward,
            frontWheelLeft,
            rearWheelLeft,
            linearVelocity,
            frontImpulse,
            rearImpulse
        );
    }

    m_localFrontWheelContactPoint = glm::zero<glm::vec3>();
    m_localRearWheelContactPoint = glm::zero<glm::vec3>();
    m_frontWheelGroundNormal = glm::zero<glm::vec3>();
    m_rearWheelGroundNormal = glm::zero<glm::vec3>();
}

void BikeController::updateAerialPhysics()
{
    btRigidBody* pRigidBody = m_pRigidBodyComponent->getRigidBody();
    btTransform& transform = pRigidBody->getWorldTransform();
    btVector3 linearVelocity = pRigidBody->getLinearVelocity();
    btVector3 angularVelocty = pRigidBody->getAngularVelocity();

    btVector3 localUp = transform.getBasis().getColumn(1);

    btVector3 predictedUp = localUp;

    if (angularVelocty.length() > 0)
    {
        btQuaternion angle;
        angle.setRotation(angularVelocty, angularVelocty.length() * (GC::bikeBalanceStability / GC::bikeBalanceSpeed));
        predictedUp = btMatrix3x3(angle) * localUp;
    }

    btVector3 torque = predictedUp.cross(btVector3(0, 1, 0));
    pRigidBody->applyTorqueImpulse(torque * GC::bikeBalanceSpeed * GC::bikeBalanceSpeed);

    if (angularVelocty.length() > 0 && linearVelocity.length() > 0)
    {
        btVector3 localForward = -transform.getBasis().getColumn(2);

        btQuaternion angle;
        angle.setRotation(angularVelocty, angularVelocty.length() * (GC::bikeAerialCorrectionStability / GC::bikeAerialCorrectionSpeed));
        btVector3 predictedForward = btMatrix3x3(angle) * localForward;

        btVector3 torque = predictedForward.cross(linearVelocity.normalized());
        pRigidBody->applyTorqueImpulse(torque * GC::bikeAerialCorrectionSpeed * GC::bikeAerialCorrectionSpeed);
    }
}

float BikeController::getActionInput(BikeActions bikeAction)
{
    return m_pJoystick ? getJoystickActionInput(bikeAction) : getKeyboardActionInput(bikeAction);
}

float BikeController::getKeyboardActionInput(BikeActions bikeAction)
{
    Game& game = Game::getInstance();

    switch (bikeAction)
    {
    case BikeActions::ACCELERATE:
        return game.isKeyDown(m_bikeControls.accelerate) ? 1.0f : 0.0f;
    case BikeActions::JUMP:
        return game.isKeyDown(m_bikeControls.jump) ? 1.0f : 0.0f;
    case BikeActions::TURN:
    {
        float value = 0.0f;

        if (game.isKeyDown(m_bikeControls.turnLeft))
            value += 1.0f;

        if (game.isKeyDown(m_bikeControls.turnRight))
            value -= 1.0f;

        return value;
    }
    default:
        return 0.0f;
    }
}

float BikeController::getJoystickActionInput(BikeActions bikeAction)
{
    switch (bikeAction)
    {
    case BikeActions::ACCELERATE:
        return std::fmin(1.0f, (m_pJoystick->getAxis(GC::accelerateAxis) + 1.0f) * 0.5f);
    case BikeActions::JUMP:
        return m_pJoystick->getButton(GC::jumpButton) == GLFW_PRESS ? 1.0f : 0.0f;
    case BikeActions::TURN:
        return std::fmin(1.0f, std::fmax(-1.0f, -m_pJoystick->getAxis(GC::turnAxis)));
    default:
        return 0.0f;
    }
}

void BikeController::kill()
{
    m_pRigidBodyComponent->getRigidBody()->setDamping(0.0f, GC::bikeDeathAngularDamping);
    m_pRigidBodyComponent->getRigidBody()->setRestitution(GC::bikeDeathRestitution);
    m_pRigidBodyComponent->getRigidBody()->setFriction(GC::bikeDeathFriction);

    LightTrail* pLightTrail = getGameObject()->getComponent<LightTrail>();

    if (pLightTrail)
        pLightTrail->setEnabled(false);

    Component::destroy(this);
}
