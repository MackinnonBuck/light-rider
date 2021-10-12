#include "Scene.h"

#include <iostream>
#include <assert.h>

#include <BulletCollision/CollisionDispatch/btInternalEdgeUtility.h>

#include "Game.h"
#include "CollisionObjectInfo.h"

extern ContactAddedCallback gContactAddedCallback;

// This fixes broken triangle mesh collisions.
static bool fixEdgeContacts(
    btManifoldPoint& cp,
    const btCollisionObjectWrapper* colObj0Wrap,
    int partId0,
    int index0,
    const btCollisionObjectWrapper* colObj1Wrap,
    int partId1,
    int index1)
{
    btAdjustInternalEdgeContacts(cp, colObj1Wrap, colObj0Wrap, partId1, index1);
    return true;
}

Scene::Scene() :
    m_pAssetManager(new AssetManager()),
    m_maxPhysicsSubSteps(Game::getInstance().getConfig().maxPhysicsSubSteps),
    m_physicsTickInterval(Game::getInstance().getConfig().physicsTickInterval),
    m_pGameObjects(new EntityContainer()),
    m_cameras(&Scene::cameraComparator),
    m_debugDrawEnabled(false),
    m_pActiveCamera(nullptr),
    m_pCollisionConfiguration(nullptr),
    m_pDispatcher(nullptr),
    m_pOverlappingPairCache(nullptr),
    m_pSolver(nullptr),
    m_pDynamicsWorld(nullptr),
    m_pDebugDrawer(nullptr)
{
    gContactAddedCallback = fixEdgeContacts;
}

Scene::~Scene()
{
    delete m_pGameObjects;
    delete m_pDynamicsWorld;
    delete m_pSolver;
    delete m_pOverlappingPairCache;
    delete m_pDispatcher;
    delete m_pCollisionConfiguration;
    delete m_pDebugDrawer;
    delete m_pAssetManager;
}

void Scene::initialize()
{
    m_pCollisionConfiguration = new btDefaultCollisionConfiguration();

    m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);

    m_pOverlappingPairCache = new btDbvtBroadphase();

    m_pSolver = new btSequentialImpulseConstraintSolver();

    m_pDebugDrawer = new DebugDrawer();

    m_pDynamicsWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pOverlappingPairCache, m_pSolver, m_pCollisionConfiguration);
    m_pDynamicsWorld->setInternalTickCallback(physicsPreTickCallback, nullptr, true);
    m_pDynamicsWorld->setInternalTickCallback(physicsTickCallback, nullptr, false);
    m_pDynamicsWorld->setWorldUserInfo(this);
    m_pDynamicsWorld->setDebugDrawer(m_pDebugDrawer);
    m_pDynamicsWorld->getDebugDrawer()->setDebugMode(
        btIDebugDraw::DBG_DrawWireframe
      | btIDebugDraw::DBG_DrawAabb
      | btIDebugDraw::DBG_DrawConstraints
      | btIDebugDraw::DBG_DrawConstraintLimits
      | btIDebugDraw::DBG_DrawContactPoints
    );

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Scene::update(float deltaTime)
{
    m_pDynamicsWorld->stepSimulation(deltaTime, m_maxPhysicsSubSteps, m_physicsTickInterval);
    m_pGameObjects->update(deltaTime);
}

void Scene::postUpdate(float deltaTime)
{
    m_pGameObjects->postUpdate(deltaTime);
}

void Scene::prePhysicsTick(float physicsTimeStep)
{
    m_pGameObjects->prePhysicsTick(physicsTimeStep);
}

void Scene::physicsTick(float physicsTimeStep)
{
    invokeCollisionCallbacks();
    m_pGameObjects->physicsTick(physicsTimeStep);
}

void Scene::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (Camera* pCamera : m_cameras)
    {
        m_pActiveCamera = pCamera;
        pCamera->render();
    }

    m_pActiveCamera = nullptr;
}

void Scene::_registerGameObject(GameObject* pGameObject)
{
    assert(pGameObject);

    if (pGameObject->getId() != -1)
        return;

    m_pGameObjects->add(pGameObject);
}

void Scene::_registerCamera(Camera* pCamera)
{
    assert(pCamera);

    m_cameras.insert(pCamera);
}

void Scene::_unregisterCamera(Camera* pCamera)
{
    assert(pCamera);

    auto cameraItr = m_cameras.find(pCamera);

    while (cameraItr != m_cameras.end())
    {
        if ((*cameraItr) == pCamera)
        {
            m_cameras.erase(cameraItr);
            return;
        }

        ++cameraItr;
    }
}

void Scene::invokeCollisionCallbacks()
{
    btDispatcher* pDispatcher = m_pDynamicsWorld->getDispatcher();
    int numManifolds = pDispatcher->getNumManifolds();

    for (int i = 0; i < numManifolds; i++)
    {
        btPersistentManifold* pContactManifold = m_pDynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

        btCollisionObject* pBody0 = const_cast<btCollisionObject*>(pContactManifold->getBody0());
        btCollisionObject* pBody1 = const_cast<btCollisionObject*>(pContactManifold->getBody1());

        if (pBody0->getUserPointer())
            invokeCollisionHandlerCallback(pContactManifold, pBody0, pBody1, true);

        if (pBody1->getUserPointer())
            invokeCollisionHandlerCallback(pContactManifold, pBody1, pBody0, false);

        numManifolds = pDispatcher->getNumManifolds();
    }
}

void Scene::invokeCollisionHandlerCallback(btPersistentManifold* pContactManifold,
    btCollisionObject* pBodySelf, btCollisionObject* pBodyOther, bool isBodyA)
{
    int numContacts = pContactManifold->getNumContacts();

    for (int i = 0; i < numContacts; i++)
    {
        btManifoldPoint& contactPoint = pContactManifold->getContactPoint(i);

        if (contactPoint.getDistance() < 0.0f)
        {
            void* pUserPointer = pBodySelf->getUserPointer();

            if (!pUserPointer)
                return;

            CollisionObjectInfo* pInfo = static_cast<CollisionObjectInfo*>(pUserPointer);
            ContactHandler* pContactHandler = pInfo->getContactHandler();

            if (pContactHandler != nullptr)
            {
                pContactHandler->handleContact(ContactInfo(contactPoint, isBodyA), pBodySelf, pBodyOther);
            }
        }

        numContacts = pContactManifold->getNumContacts();
    }
}

void Scene::physicsPreTickCallback(btDynamicsWorld* pDynamicsWorld, btScalar timeStep)
{
    Scene* pWorldScene = static_cast<Scene*>(pDynamicsWorld->getWorldUserInfo());
    pWorldScene->prePhysicsTick((float)timeStep);
}

void Scene::physicsTickCallback(btDynamicsWorld* pDynamicsWorld, btScalar timeStep)
{
    Scene* pWorldScene = static_cast<Scene*>(pDynamicsWorld->getWorldUserInfo());
    pWorldScene->physicsTick((float)timeStep);
}

bool Scene::cameraComparator(const Camera* c1, const Camera* c2)
{
    return c1->getLayerDepth() < c2->getLayerDepth();
}
