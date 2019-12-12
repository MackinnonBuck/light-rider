#include "Tests/TestScene.h"

#include <iostream>
#include <string>

#include "Game.h"
#include "Components/FreeroamCamera.h"
#include "Components/RigidBodyComponent.h"
#include "Tests/TestComponent.h"

void TestScene::initialize()
{
    Scene::initialize();

    setDebugDrawEnabled(true);

    GameObject* pCameraObject = GameObject::create("Camera");
    Camera* pCamera = pCameraObject->addComponent<FreeroamCamera>();
    pCameraObject->getTransform()->setPosition(glm::vec3(0.0f, 0.0, 50.0f));

    GameObject* pGroundObject = GameObject::create("Ground");

    btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setOrigin(btVector3(0, -56, 0));

    btVector3 groundLocalInertia(0, 0, 0);

    btDefaultMotionState* groundMotionState = new btDefaultMotionState(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo groundRbInfo(0, groundMotionState, groundShape, groundLocalInertia);

    RigidBodyComponent* pGroundComponent = pGroundObject->addComponent<RigidBodyComponent>(groundRbInfo);
    pGroundComponent->getRigidBody()->setRestitution(btScalar(0.75));
}

void TestScene::update(float deltaTime)
{
    if (Game::getInstance().isKeyDown(GLFW_KEY_SPACE))
    {
        m_pLast = GameObject::create(std::to_string(m_counter), m_pLast);
        m_pLast->addComponent<TestComponent>();

        TestComponent* pComponent = m_pLast->getComponent<TestComponent>();

        if (pComponent)
            std::cout << "Heck yeaaaaa!" << std::endl;

        m_counter++;
    }

    if (Game::getInstance().isKeyDown(GLFW_KEY_BACKSPACE))
    {
        GameObject* g = GameObject::find("0");

        if (g != nullptr)
        {
            std::cout << "DELETE!\n";
            GameObject::destroy(g);
            m_counter = 0;
            m_pLast = nullptr;
        }
    }

    if (Game::getInstance().isKeyDown(GLFW_KEY_UP))
    {
        std::cout << "CREATE SPHERE!\n";

        createSphere();
    }

    GameObject* pSphere = GameObject::find("Sphere");

    if (pSphere && Game::getInstance().isKeyDown(GLFW_KEY_DOWN))
    {
        std::cout << "REMOVE SPHERE!\n";
        GameObject::destroy(pSphere);
    }

    Scene::update(deltaTime);
}

void TestScene::createSphere()
{
    GameObject* pSphereObject = GameObject::create("Sphere");

    btCollisionShape* sphereShape = new btSphereShape(btScalar(1.0));

    btTransform sphereTransform;
    sphereTransform.setIdentity();

    btVector3 sphereLocalInertia(0, 0, 0);
    sphereShape->calculateLocalInertia(btScalar(1.0), sphereLocalInertia);

    sphereTransform.setOrigin(btVector3(0, 10, 0));

    btDefaultMotionState* sphereMotionState = new btDefaultMotionState(sphereTransform);
    btRigidBody::btRigidBodyConstructionInfo sphereRbInfo(btScalar(1.0), sphereMotionState, sphereShape, sphereLocalInertia);

    RigidBodyComponent* pSphereComponent = pSphereObject->addComponent<RigidBodyComponent>(sphereRbInfo);
    pSphereComponent->getRigidBody()->setRestitution(btScalar(0.75));
}
