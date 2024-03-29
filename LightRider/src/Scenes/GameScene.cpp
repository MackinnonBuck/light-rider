#include "Scenes/GameScene.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "GameConstants.h"
#include "GameObject.h"
#include "Presets.h"
#include "Components/BikeController.h"
#include "Components/BikeRenderer.h"
#include "Components/LightTrail.h"
#include "Components/FreeroamCamera.h"
#include "Components/PlayerCamera.h"
#include "Components/DeathCamera.h"

namespace GC = GameConstants;

void GameScene::initialize()
{
    LightRiderScene::initialize();

    loadAssets();

    Presets::createLightRiderGround("Ground");
    Presets::createLightRiderRamp(glm::vec3(50.0f, 0.0f, 50.0), glm::pi<float>() * 0.25f * 3);
    Presets::createLightRiderRamp(glm::vec3(50.0f, 0.0f, -50.0), glm::pi<float>() * 0.25f * 5);
    Presets::createLightRiderRamp(glm::vec3(-50.0f, 0.0f, 50.0), glm::pi<float>() * 0.25f);
    Presets::createLightRiderRamp(glm::vec3(-50.0f, 0.0f, -50.0), glm::pi<float>() * 0.25f * 7);

    GameObject* pFreeroamCameraObject = GameObject::create("Camera");
    m_pFreeroamCamera = pFreeroamCameraObject->addComponent<FreeroamCamera>(false);
    m_pFreeroamCamera->setSky("skyShader", "skyTexture", "sphereShape");
    pFreeroamCameraObject->getTransform()->setPosition(glm::vec3(0.0f, 5.0f, 20.0f));

    GameObject* pChunkManagerObject = GameObject::create("ChunkManager");
    m_pChunkManager = pChunkManagerObject->addComponent<ChunkManager>();

    ContainerGroupBuilder containerGroupBuilder(0.75f, 0.1f);

    // Center field group
    containerGroupBuilder.addGroup(glm::vec2(10.0f, 10.0f), glm::ivec3(2, 2, 2));
    containerGroupBuilder.addGroup(glm::vec2(10.0f, -10.0f), glm::ivec3(2, 2, 2));
    containerGroupBuilder.addGroup(glm::vec2(-10.0f, 10.0f), glm::ivec3(2, 2, 2));
    containerGroupBuilder.addGroup(glm::vec2(-10.0f, -10.0f), glm::ivec3(2, 2, 2));

    // Side grids
    containerGroupBuilder.addGroup(glm::vec2(0.0f, 150.0f), glm::ivec3(10, 1, 5));
    containerGroupBuilder.addGroup(glm::vec2(0.0f, -150.0f), glm::ivec3(10, 1, 5));

    // Corner patterns
    buildCornerContainers(containerGroupBuilder, glm::vec2(160.0f, 160.0f));
    buildCornerContainers(containerGroupBuilder, glm::vec2(160.0f, -160.0f));
    buildCornerContainers(containerGroupBuilder, glm::vec2(-160.0f, 160.0f));
    buildCornerContainers(containerGroupBuilder, glm::vec2(-160.0f, -160.0f));

    // Side strips
    containerGroupBuilder.addGroup(glm::vec2(0.0f, 75.0f), glm::ivec3(1, 1, 30));
    containerGroupBuilder.addGroup(glm::vec2(0.0f, -75.0f), glm::ivec3(1, 1, 30));

    // Back strips
    containerGroupBuilder.addGroup(glm::vec2(160.0f, 60.0f), glm::ivec3(1, 1, 8));
    containerGroupBuilder.addGroup(glm::vec2(160.0f, -60.0f), glm::ivec3(1, 1, 8));
    containerGroupBuilder.addGroup(glm::vec2(-160.0f, 60.0f), glm::ivec3(1, 1, 8));
    containerGroupBuilder.addGroup(glm::vec2(-160.0f, -60.0f), glm::ivec3(1, 1, 8));

    // Diagonal boxes
    containerGroupBuilder.addGroup(glm::vec2(100.0f, 50.0f), glm::ivec3(1, 1, 1));
    containerGroupBuilder.addGroup(glm::vec2(50.0f, 100.0f), glm::ivec3(1, 1, 1));
    containerGroupBuilder.addGroup(glm::vec2(100.0f, -50.0f), glm::ivec3(1, 1, 1));
    containerGroupBuilder.addGroup(glm::vec2(50.0f, -100.0f), glm::ivec3(1, 1, 1));
    containerGroupBuilder.addGroup(glm::vec2(-100.0f, 50.0f), glm::ivec3(1, 1, 1));
    containerGroupBuilder.addGroup(glm::vec2(-50.0f, 100.0f), glm::ivec3(1, 1, 1));
    containerGroupBuilder.addGroup(glm::vec2(-100.0f, -50.0f), glm::ivec3(1, 1, 1));
    containerGroupBuilder.addGroup(glm::vec2(-50.0f, -100.0f), glm::ivec3(1, 1, 1));

    // Build all containers
    containerGroupBuilder.build("containerShader", "containerTexture", "containerShape");

    initScene();
}

void GameScene::update(float deltaTime)
{
    if (Game::getInstance().isKeyDown(GLFW_KEY_0))
    {
        m_pFreeroamCamera->disable();
        m_pPlayer1Camera->enable(0.0f);
        m_pPlayer2Camera->enable(0.0f);

        //setDebugDrawEnabled(false);
    }
    else if (Game::getInstance().isKeyDown(GLFW_KEY_1))
    {
        m_pPlayer1Camera->disable();
        m_pPlayer2Camera->disable();
        m_pFreeroamCamera->enable(0.0f);

        //setDebugDrawEnabled(true);
    }

    LightRiderScene::update(deltaTime);
}

void GameScene::physicsTick(float physicsTimeStep)
{
    LightRiderScene::physicsTick(physicsTimeStep);

    if (m_introTick <= GC::introEndFrame)
    {
        float transitionAmount = glm::min(1.0f, (float)m_introTick / GC::introCameraFocusFrame);

        m_pPlayer1Bike->getComponent<BikeRenderer>()->setTransitionAmount(transitionAmount);
        m_pPlayer2Bike->getComponent<BikeRenderer>()->setTransitionAmount(transitionAmount);

        switch (m_introTick)
        {
        case GC::introLightTrailFrame:
            m_pPlayer1Bike->addComponent<LightTrail>(0);
            m_pPlayer2Bike->addComponent<LightTrail>(1);
            break;
        case GC::introCameraFocusFrame:
            m_pPlayer1Camera->setCameraMode(PlayerCameraMode::FOLLOW);
            m_pPlayer2Camera->setCameraMode(PlayerCameraMode::FOLLOW);
            break;
        case GC::introEndFrame:
            BikeControlMode controlMode = m_gameConfig.argumentCount >= 2 &&
                strcmp(m_gameConfig.arguments[1], "-demo") == 0 ?
                BikeControlMode::MANUAL_ACCELERATE :
                BikeControlMode::AUTO_ACCELERATE;

            BikeController* pBikeController = m_pPlayer1Bike->getComponent<BikeController>();

            if (pBikeController)
                pBikeController->setControlMode(controlMode);

            pBikeController = m_pPlayer2Bike->getComponent<BikeController>();

            if (pBikeController)
                pBikeController->setControlMode(controlMode);

            m_pPlayer1Bike->getComponent<BikeRenderer>()->setTransitionAmount(1.0f);
            m_pPlayer2Bike->getComponent<BikeRenderer>()->setTransitionAmount(1.0f);

            break;
        }

        m_introTick++;
    }
    else if (m_ticksUntilReset == -1)
    {
        bool player1Dead = isPlayerDead(m_pPlayer1Bike);
        bool player2Dead = isPlayerDead(m_pPlayer2Bike);

        if (player1Dead || player2Dead)
        {
            m_pPlayer1Camera->disable();
            m_pPlayer2Camera->disable();
            m_pFreeroamCamera->disable();

            auto pDeadBike = player1Dead ? m_pPlayer1Bike : m_pPlayer2Bike;

            m_pDeathCamera->enable(0.0f);
            m_pDeathCamera->setDeadObject(pDeadBike);
            m_pDeathCamera->setSubject(pDeadBike);

            m_ticksUntilReset = GC::deathSequenceTicks;
        }
    }
    else if (--m_ticksUntilReset == 0)
    {
        clearScene();
        initScene();
    }
}

bool GameScene::isPlayerDead(const GameObject* pPlayerGameObject) const
{
    BikeController* pBikeController = pPlayerGameObject->getComponent<BikeController>();
    assert(pBikeController != nullptr);
    return pBikeController->isDead();
}

void GameScene::initScene()
{
    m_introTick = 0;
    m_ticksUntilReset = -1;

    setDebugDrawEnabled(false);

    getDynamicsWorld()->setGravity(btVector3(0.0f, GC::worldGravity, 0.0f));

    BikeControls player1BikeControls{ GLFW_JOYSTICK_1, GLFW_KEY_W, GLFW_KEY_SPACE, GLFW_KEY_A, GLFW_KEY_D };
    BikeControls player2BikeControls{ GLFW_JOYSTICK_2, GLFW_KEY_UP, GLFW_KEY_ENTER, GLFW_KEY_LEFT, GLFW_KEY_RIGHT };

    PlayerCameraControls player1CameraControls{ GLFW_JOYSTICK_1, GLFW_KEY_LEFT_SHIFT };
    PlayerCameraControls player2CameraControls{ GLFW_JOYSTICK_2, GLFW_KEY_RIGHT_SHIFT };

    m_pPlayer1Bike = Presets::createLightRiderBike("Player1Bike", 0,
        player1BikeControls, m_pChunkManager, glm::vec3(-190.0f, 1.0f, 0.0f), -glm::pi<float>() * 0.5f);

    m_pPlayer2Bike = Presets::createLightRiderBike("Player2Bike", 1,
        player2BikeControls, m_pChunkManager, glm::vec3(190.0f, 1.0f, 0.0f), glm::pi<float>() * 0.5f);
    
    GameObject* pPlayer1CameraObject = GameObject::create("Player1Camera");
    pPlayer1CameraObject->getTransform()->setPosition(GC::introCamera1Position);
    pPlayer1CameraObject->getTransform()->setRotation(glm::eulerAngleY(glm::pi<float>() * 0.5f));

    m_pPlayer1Camera = pPlayer1CameraObject->addComponent<PlayerCamera>(m_pPlayer1Bike, m_pPlayer2Bike, player1CameraControls);
    m_pPlayer1Camera->setSky("skyShader", "skyTexture", "sphereShape");
    m_pPlayer1Camera->setSizeRatio(glm::vec2(0.5f, 1.0f));
    m_pPlayer1Camera->setSubject(m_pPlayer1Bike);

    GameObject* pPlayer2CameraObject = GameObject::create("Player2Camera");
    pPlayer2CameraObject->getTransform()->setPosition(GC::introCamera2Position);
    pPlayer2CameraObject->getTransform()->setRotation(glm::eulerAngleY(-glm::pi<float>() * 0.5f));

    m_pPlayer2Camera = pPlayer2CameraObject->addComponent<PlayerCamera>(m_pPlayer2Bike, m_pPlayer1Bike, player2CameraControls);
    m_pPlayer2Camera->setSky("skyShader", "skyTexture", "sphereShape");
    m_pPlayer2Camera->setOffsetRatio(glm::vec2(0.5f, 0.0f));
    m_pPlayer2Camera->setSizeRatio(glm::vec2(0.5f, 1.0f));
    m_pPlayer2Camera->setSubject(m_pPlayer2Bike);
    
    GameObject* pDeathCameraObject = GameObject::create("DeathCamera");
    m_pDeathCamera = pDeathCameraObject->addComponent<DeathCamera>();
    m_pDeathCamera->setSky("skyShader", "skyTexture", "sphereShape");
}

void GameScene::clearScene()
{
    GameObject::destroy(m_pDeathCamera->getGameObject());
    GameObject::destroy(m_pPlayer1Camera->getGameObject());
    GameObject::destroy(m_pPlayer2Camera->getGameObject());
    GameObject::destroy(m_pPlayer1Bike);
    GameObject::destroy(m_pPlayer2Bike);

    m_pChunkManager->clearChunks();
}

void GameScene::buildCornerContainers(ContainerGroupBuilder& builder, const glm::vec2& position)
{
    builder.addGroup(position, glm::ivec3(1, 30, 1));
    builder.addGroup(position + glm::vec2(15.0f, 15.0f), glm::ivec3(2, 3, 2));
    builder.addGroup(position + glm::vec2(15.0f, -15.0f), glm::ivec3(2, 3, 2));
    builder.addGroup(position + glm::vec2(-15.0f, 15.0f), glm::ivec3(2, 3, 2));
    builder.addGroup(position + glm::vec2(-15.0f, -15.0f), glm::ivec3(2, 3, 2));
}
