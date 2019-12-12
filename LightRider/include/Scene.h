#pragma once

#include <set>
#include <btBulletDynamicsCommon.h>

#include "GameConfig.h"
#include "EntityContainer.h"
#include "AssetManager.h"
#include "GameObject.h"
#include "ContactHandler.h"
#include "Camera.h"
#include "DebugDrawer.h"

class Scene
{
public:

    // Creates a new Scene instance.
    Scene();

    // Destroys the scene.
    virtual ~Scene();

    // Returns the first top-level GameObject that passes the given predicate,
    // or nullptr if no GameObject could be found.
    template<typename F>
    GameObject* findGameObject(F f) { return m_pGameObjects->find<GameObject>(f); }

    // Returns the scene's asset manager.
    AssetManager* getAssetManager() const { return m_pAssetManager; }

    // Returns the physics simulation world.
    btDynamicsWorld* getDynamicsWorld() const { return m_pDynamicsWorld; }

    // Returns the Scene's active camera. This will be null except during the render stage.
    Camera* getActiveCamera() const { return m_pActiveCamera; };

    // Returns true if debug drawing is enabled for the scene, otherwise false.
    bool getDebugDrawEnabled() const { return m_debugDrawEnabled; }

    // Enalbes or disables debug drawing for the scene.
    void setDebugDrawEnabled(bool isDebugDrawEnabled) { m_debugDrawEnabled = isDebugDrawEnabled; }

    // Initializes the Scene.
    virtual void initialize();

    // Updates the Scene.
    virtual void update(float deltaTime);

    // Updates the Scene in the post-update stage.
    virtual void postUpdate(float deltaTime);

    // Renders the Scene.
    virtual void render();

    // Registers the given GameObject with the scene.
    // For internal use only - use GameObject::create() instead.
    void _registerGameObject(GameObject* pGameObject);

    // Registers the given Camera with the scene.
    // For internal use only - use Camera::enable() instead.
    void _registerCamera(Camera* pCamera);

    // Unregisters the given Camera with the scene.
    // For internal use only - use Camera::disable() instead.
    void _unregisterCamera(Camera* pCamera);

protected:

    // Updates the scene just before physics is updated.
    virtual void prePhysicsTick(float physicsTimeStep);

    // Updates the scene just after physics is updated.
    virtual void physicsTick(float physicsTimeStep);

private:

    // Sorts cameras by increasing layer depth.
    static bool cameraComparator(const Camera* c1, const Camera* c2);

    // The scene's asset manager.
    AssetManager* m_pAssetManager;

    // The maximum number of physics substeps in a single frame.
    int m_maxPhysicsSubSteps;

    // The internal tick interval for the physics simulation.
    float m_physicsTickInterval;

    // The container for all GameObjects in the scene.
    EntityContainer* m_pGameObjects;

    // A collection of cameras, ordered by layer depth.
    std::multiset<Camera*, decltype(&cameraComparator)> m_cameras;

    // The scene's active camera. Will be null except during the render stage.
    Camera* m_pActiveCamera;

    // If true, debug drawing is enabled in the scene.
    bool m_debugDrawEnabled;

    // The memory configuration for collision detection. We use the default configuration provided by Bullet.
    btDefaultCollisionConfiguration* m_pCollisionConfiguration;

    // Handles collision detection. We use the default collision dispatcher since we don't multi-thread collision handling.
    btCollisionDispatcher* m_pDispatcher;

    // Detects overlapping object pairs to optimize when we check for collisions.
    btBroadphaseInterface* m_pOverlappingPairCache;

    // Solves constraint relationships between bodies.
    btSequentialImpulseConstraintSolver* m_pSolver;

    // The physics simulation world.
    btDiscreteDynamicsWorld* m_pDynamicsWorld;

    // Draws the simulation world when in debug mode.
    DebugDrawer* m_pDebugDrawer;

    // Invokes collision callbacks for all RigidBodyComponents in the world.
    void invokeCollisionCallbacks();

    // Invokes the collision callback on the given handler with the provided contact point.
    inline void invokeCollisionHandlerCallback(btPersistentManifold* pContactManifold,
        btCollisionObject* pBodySelf, btCollisionObject* pBodyOther, bool isBodyA);

    // Called just before the physics world is updated.
    static void physicsPreTickCallback(btDynamicsWorld* pDynamicsWorld, btScalar timeStep);

    // Called just after the physics world is updated.
    static void physicsTickCallback(btDynamicsWorld* pDynamicsWorld, btScalar timeStep);
};
