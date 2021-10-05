#include "Presets.h"

#include <iostream>
#include <btBulletDynamicsCommon.h>
#include <glm/gtx/euler_angles.hpp>

#include <BulletCollision/CollisionDispatch/btInternalEdgeUtility.h>

#include "Game.h"
#include "GameConstants.h"
#include "ConversionUtils.h"
#include "Components/RigidBodyComponent.h"
#include "Components/MeshRenderer.h"
#include "Components/BikeController.h"
#include "Components/BikeRenderer.h"
#include "Components/LightTrail.h"
#include "Components/RampRenderer.h"

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
        btRigidBody* pGroundRigidBody = pGroundComponent->getRigidBody();
        pGroundRigidBody->setFriction(1.0f);
        pGroundRigidBody->setRestitution(1.0f);

        MeshRenderer* pGroundMeshRenderer = pGroundObject->addComponent<MeshRenderer>("groundShader", "groundTexture", "planeShape", true);
        pGroundMeshRenderer->setUsingForwardShadowRendering(true);
        pGroundMeshRenderer->setDepthFunction([](Camera* pCamera) { return 1000.0f; });
        pGroundMeshRenderer->setLocalTransform(
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, GC::mapHalfHeight, 0.0f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(GC::mapHalfWidth, 1.0f, GC::mapHalfWidth)) *
            glm::rotate(glm::mat4(1.0f), glm::pi<float>() * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f))
        );

        return pGroundObject;
    }

    GameObject* createLightRiderRamp(const glm::vec3& position, float rotation)
    {
        GameObject* pRampObject = GameObject::create("Ramp");

        glm::vec3 scale = glm::vec3(15.0f, 40.0f, 20.0f);
        glm::mat4 localTransform = glm::scale(glm::mat4(1.0f), scale);

        MeshRenderer* pRampMeshRenderer = pRampObject->addComponent<RampRenderer>();
        pRampMeshRenderer->setLocalTransform(localTransform);

        const Shape* pRampMesh = Game::getInstance().getScene()->getAssetManager()->getShape("rampShapeCollision");
        auto& vertices = pRampMesh->getVertices(0);
        auto& indices = pRampMesh->getIndices(0);

        btTriangleIndexVertexArray* pIndexVertexArray = new btTriangleIndexVertexArray(
            indices.size() / 3,
            (int*)indices.data(),
            3 * sizeof(unsigned int),
            vertices.size() / 3,
            (float*)vertices.data(),
            3 * sizeof(float));

        btBvhTriangleMeshShape* rampShape = new btBvhTriangleMeshShape(pIndexVertexArray, true, true);
        rampShape->setLocalScaling(toBullet(scale));

        btTriangleInfoMap* infoMap = new btTriangleInfoMap();
        btGenerateInternalEdgeInfo(rampShape, infoMap);

		btTransform rampTransform;
		rampTransform.setIdentity();
		rampTransform.setOrigin(toBullet(position + glm::vec3(0.0f, 2.4f, 0.0f)));
        rampTransform.setRotation(toBullet(glm::eulerAngleY(rotation)));

        btVector3 rampLocalInertia(0.0f, 0.0f, 0.0f);

        btDefaultMotionState* rampMotionState = new btDefaultMotionState(rampTransform);
        btRigidBody::btRigidBodyConstructionInfo rampRbInfo(0, rampMotionState, rampShape, rampLocalInertia);

        RigidBodyComponent* pRampRigidBodyComponent = pRampObject->addComponent<RigidBodyComponent>(rampRbInfo);
        btRigidBody* pRampRigidBody = pRampRigidBodyComponent->getRigidBody();
        pRampRigidBody->setFriction(0.0f);
        pRampRigidBody->setRestitution(0.0f);
        pRampRigidBody->setCollisionFlags(pRampRigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

        return pRampObject;
    }

    GameObject* createLightRiderBike(const std::string& name, int playerId, const BikeControls& bikeControls,
        ChunkManager* pChunkManager, const glm::vec3& position, float yaw)
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
        pBikeObject->addComponent<BikeController>(bikeControls, pChunkManager);

        BikeRenderer* pBikeRenderer = pBikeObject->addComponent<BikeRenderer>(playerId);
        pBikeRenderer->setLocalTransform(
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, GC::bikeShapeVerticalOffset, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), -glm::pi<float>() * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::pi<float>() * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f))
        );

        return pBikeObject;
    }

    GameObject* createLightRiderBikeDisplay(const std::string& name, int playerId,
        const glm::vec3& position, float yaw)
    {
        GameObject* pBikeDisplayObject = GameObject::create(name);
        pBikeDisplayObject->getTransform()->setTransformMatrix(
            glm::translate(glm::mat4(1.0f), position) *
            glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), -glm::pi<float>() * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f))
        );
        BikeRenderer* pBikeRenderer = pBikeDisplayObject->addComponent<BikeRenderer>(playerId);
        pBikeRenderer->setTransitionAmount(1.0f);

        return pBikeDisplayObject;
    }
}
