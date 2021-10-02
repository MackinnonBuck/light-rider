#include "Scenes/GameScene.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "GameConstants.h"
#include "GameObject.h"
#include "Presets.h"
#include "Components/BikeController.h"
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

    initScene();
}

void GameScene::update(float deltaTime)
{
    if (Game::getInstance().isKeyDown(GLFW_KEY_0))
    {
        m_pFreeroamCamera->disable();
        m_pPlayer1Camera->enable(0.0f);
        m_pPlayer2Camera->enable(0.0f);

        setDebugDrawEnabled(false);
    }
    else if (Game::getInstance().isKeyDown(GLFW_KEY_1))
    {
        m_pPlayer1Camera->disable();
        m_pPlayer2Camera->disable();
        m_pFreeroamCamera->enable(0.0f);

        setDebugDrawEnabled(true);
    }

    LightRiderScene::update(deltaTime);
}

void GameScene::physicsTick(float physicsTimeStep)
{
    LightRiderScene::physicsTick(physicsTimeStep);

    if (m_introTick <= GC::introEndFrame)
    {
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

            break;
        }

        m_introTick++;
    }
    else if (m_ticksUntilReset == -1)
    {
        bool player1Dead = m_pPlayer1Bike->getComponent<BikeController>() == nullptr;
        bool player2Dead = m_pPlayer2Bike->getComponent<BikeController>() == nullptr;

        if (player1Dead || player2Dead)
        {
            m_pPlayer1Camera->disable();
            m_pPlayer2Camera->disable();
            m_pFreeroamCamera->disable();

            m_pDeathCamera->enable(0.0f);
            m_pDeathCamera->setDeadObject(player1Dead ? m_pPlayer1Bike : m_pPlayer2Bike);

            m_ticksUntilReset = 240;
        }
    }
    else if (--m_ticksUntilReset == 0)
    {
        clearScene();
        initScene();
    }
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
        player1BikeControls, glm::vec3(-190.0f, 1.0f, 0.0f), -glm::pi<float>() * 0.5f);

    m_pPlayer2Bike = Presets::createLightRiderBike("Player2Bike", 1,
        player2BikeControls, glm::vec3(190.0f, 1.0f, 0.0f), glm::pi<float>() * 0.5f);
    
    GameObject* pPlayer1CameraObject = GameObject::create("Player1Camera");
    pPlayer1CameraObject->getTransform()->setPosition(GC::introCamera1Position);
    pPlayer1CameraObject->getTransform()->setRotation(glm::eulerAngleY(glm::pi<float>() * 0.5f));

    m_pPlayer1Camera = pPlayer1CameraObject->addComponent<PlayerCamera>(m_pPlayer1Bike, m_pPlayer2Bike, player1CameraControls);
    m_pPlayer1Camera->setFieldOfView(glm::pi<float>() / 3.5f);
    m_pPlayer1Camera->setSky("skyShader", "skyTexture", "sphereShape");
    m_pPlayer1Camera->setSizeRatio(glm::vec2(0.5f, 1.0f));

    GameObject* pPlayer2CameraObject = GameObject::create("Player2Camera");
    pPlayer2CameraObject->getTransform()->setPosition(GC::introCamera2Position);
    pPlayer2CameraObject->getTransform()->setRotation(glm::eulerAngleY(-glm::pi<float>() * 0.5f));

    m_pPlayer2Camera = pPlayer2CameraObject->addComponent<PlayerCamera>(m_pPlayer2Bike, m_pPlayer1Bike, player2CameraControls);
    m_pPlayer2Camera->setFieldOfView(glm::pi<float>() / 3.5f);
    m_pPlayer2Camera->setSky("skyShader", "skyTexture", "sphereShape");
    m_pPlayer2Camera->setOffsetRatio(glm::vec2(0.5f, 0.0f));
    m_pPlayer2Camera->setSizeRatio(glm::vec2(0.5f, 1.0f));
    
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
}
