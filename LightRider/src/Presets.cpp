#include "Presets.h"

#include <iostream>
#include <btBulletDynamicsCommon.h>

#include "Game.h"
#include "GameConstants.h"
#include "ConversionUtils.h"
#include "Components/RigidBodyComponent.h"
#include "Components/MeshRenderer.h"
#include "Components/BikeController.h"
#include "Components/BikeRenderer.h"
#include "Components/LightTrail.h"

namespace GC = GameConstants;

namespace Presets
{
    GameObject* createLightRiderGround(const std::string& name)
    {
        GameObject* pGroundObject = GameObject::create(name);

        btCollisionShape* groundShape = new btBoxShape(btVector3(GC::mapHalfWidth, GC::mapHalfHeight, GC::mapHalfWidth));

        btTransform groundTransform;
        groundTransform.setIdentity();
        groundTransform.setOrigin(btVector3(0.0f, -GC::mapHalfHeight, 0.0f));

        btVector3 groundLocalInertia(0.0f, 0.0f, 0.0f);

        btDefaultMotionState* groundMotionState = new btDefaultMotionState(groundTransform);
        btRigidBody::btRigidBodyConstructionInfo groundRbInfo(0, groundMotionState, groundShape, groundLocalInertia);

        RigidBodyComponent* pGroundComponent = pGroundObject->addComponent<RigidBodyComponent>(groundRbInfo);
        pGroundComponent->getRigidBody()->setFriction(1.0f);
        pGroundComponent->getRigidBody()->setRestitution(1.0f);

        MeshRenderer* pGroundMeshRenderer = pGroundObject->addComponent<MeshRenderer>("groundShader", "groundTexture", "planeShape", true);
        pGroundMeshRenderer->setDepthFunction([](Camera* pCamera) { return 1000.0f; });
        pGroundMeshRenderer->setLocalTransform(
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, GC::mapHalfHeight, 0.0f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(GC::mapHalfWidth, 1.0f, GC::mapHalfWidth))
        );

        return pGroundObject;
    }

    GameObject* createLightRiderBike(const std::string& name, const glm::vec3& color, const BikeControls& bikeControls,
        const glm::vec3& position, float yaw)
    {
        GameObject* pBikeObject = GameObject::create(name);

        btVector3 bikePosition = toBullet(position);

        btQuaternion bikeRotation;
        bikeRotation.setEuler(yaw, 0.0f, 0.0f);

        btTransform bikeTransform = btTransform(bikeRotation, bikePosition);

        btCompoundShape* pCompoundShape = new btCompoundShape(true, 3);

        btBoxShape* pFrameShape = new btBoxShape(btVector3(btScalar(GC::bikeHalfWidth), btScalar(GC::bikeHalfHeight), btScalar(GC::bikeHalfLength)));
        btSphereShape* pFrontWheelShape = new btSphereShape(btScalar(GC::bikeWheelRadius));
        btSphereShape* pBackWheelShape = new btSphereShape(btScalar(GC::bikeWheelRadius));

        btVector3 localIntertia(0, 0, 0);

        float wheelbaseOffset = GC::bikeHalfLength - GC::bikeWheelRadius - GC::bikeWheelBodyPadding;

        btVector3 comOffset(0, -0.1f, 0);

        pCompoundShape->addChildShape(btTransform(btQuaternion::getIdentity(), btVector3(0, 0, 0) - comOffset), pFrameShape);
        pCompoundShape->addChildShape(btTransform(btQuaternion::getIdentity(), btVector3(0, GC::bikeWheelVerticalOffset, -wheelbaseOffset) - comOffset), pFrontWheelShape);
        pCompoundShape->addChildShape(btTransform(btQuaternion::getIdentity(), btVector3(0, GC::bikeWheelVerticalOffset, wheelbaseOffset) - comOffset), pBackWheelShape);
        pCompoundShape->calculateLocalInertia(btScalar(GC::bikeMass), localIntertia);

        btDefaultMotionState* pMotionState = new btDefaultMotionState(bikeTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(GC::bikeMass), pMotionState, pCompoundShape, localIntertia);

        pBikeObject->addComponent<RigidBodyComponent>(rbInfo);
        pBikeObject->addComponent<BikeController>(bikeControls);

        BikeRenderer* pBikeRenderer = pBikeObject->addComponent<BikeRenderer>(color);
        pBikeRenderer->setLocalTransform(
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, GC::bikeShapeVerticalOffset, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), -glm::pi<float>() * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::pi<float>() * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f))
        );

        return pBikeObject;
    }
}
