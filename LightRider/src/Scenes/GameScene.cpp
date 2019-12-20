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
    Scene::initialize();

    loadAssets();

    Presets::createLightRiderGround("Ground");

    GameObject* pFreeroamCameraObject = GameObject::create("Camera");
    m_pFreeroamCamera = pFreeroamCameraObject->addComponent<FreeroamCamera>(false);
    pFreeroamCameraObject->getTransform()->setPosition(glm::vec3(0.0f, 5.0f, 20.0f));

    initScene();
}

void GameScene::update(float deltaTime)
{
    if (Game::getInstance().isKeyDown(GLFW_KEY_KP_0))
    {
        m_pFreeroamCamera->disable();
        m_pPlayer1Camera->enable(0.0f);
        m_pPlayer2Camera->enable(0.0f);

        setDebugDrawEnabled(false);
    }
    else if (Game::getInstance().isKeyDown(GLFW_KEY_KP_1))
    {
        m_pPlayer1Camera->disable();
        m_pPlayer2Camera->disable();
        m_pFreeroamCamera->enable(0.0f);

        setDebugDrawEnabled(true);
    }

    Scene::update(deltaTime);
}

void GameScene::physicsTick(float physicsTimeStep)
{
    Scene::physicsTick(physicsTimeStep);

    if (m_introTick <= GC::introEndFrame)
    {
        switch (m_introTick)
        {
        case GC::introLightTrailFrame:
            m_pPlayer1Bike->addComponent<LightTrail>(GC::player1Color);
            m_pPlayer2Bike->addComponent<LightTrail>(GC::player2Color);
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

void GameScene::loadAssets()
{
    AssetManager* pAssets = getAssetManager();

    // Bike assets.
    Program* pBikeShader = pAssets->loadShaderProgram("bikeShader", "bike_vertex.glsl", "bike_fragment.glsl");
    pBikeShader->addUniform("bikeColor");
    pBikeShader->addUniform("campos");

    Program* pTrailShader = pAssets->loadShaderProgram("trailShader", "trail_vertex.glsl", "trail_fragment.glsl",
        ShaderUniform::P_MATRIX
      | ShaderUniform::V_MATRIX
    );
    pTrailShader->addUniform("baseColor");
    pTrailShader->addUniform("noiseSeed");
    pTrailShader->addUniform("currentTime");

    pAssets->loadTexture("bikeTexture", "light_cycle_texture.png", TextureType::IMAGE);
    pAssets->loadShape("bikeShape", "light_cycle.shape");

    // Ground assets.
    pAssets->loadShaderProgram("groundShader", "ground_vertex.glsl", "ground_fragment.glsl");
    pAssets->loadTexture("groundTexture", "ground_texture.png", TextureType::IMAGE);

    // Sky assets.
    pAssets->loadShaderProgram("skyShader", "sky_vertex.glsl", "sky_fragment.glsl",
        ShaderUniform::P_MATRIX
      | ShaderUniform::V_MATRIX
      | ShaderUniform::M_MATRIX
      | ShaderUniform::TEXTURE_3
    );
    pAssets->loadTexture("skyTexture", "sky.jpg", TextureType::BACKGROUND);

    // Miscellaneous assets.
    Program* pSsrShader = pAssets->loadShaderProgram("ssrShader", "ssr_vertex.glsl", "ssr_fragment.glsl", ShaderUniform::NONE);
    pSsrShader->addUniform("screenTexture");
    pSsrShader->addUniform("screenPositions");
    pSsrShader->addUniform("screenNormals");
    pSsrShader->addUniform("reflectionMap");

    Program* pPostShader = pAssets->loadShaderProgram("hdrShader", "hdr_vertex.glsl", "hdr_fragment.glsl", ShaderUniform::NONE);
    pPostShader->addUniform("screenTexture");

    Program* pFxaaShader = pAssets->loadShaderProgram("fxaaShader", "fxaa_vertex.glsl", "fxaa_fragment.glsl", ShaderUniform::NONE);
    pFxaaShader->addUniform("screenTexture");
    pFxaaShader->addUniform("screenWidth");
    pFxaaShader->addUniform("screenHeight");

    Program* pBlurShader = pAssets->loadShaderProgram("blurShader", "blur_vertex.glsl", "blur_fragment.glsl", ShaderUniform::NONE);
    pBlurShader->addUniform("screenTexture");
    pBlurShader->addUniform("horizontal");

    Program* pBloomShader = pAssets->loadShaderProgram("bloomShader", "bloom_vertex.glsl", "bloom_fragment.glsl", ShaderUniform::NONE);
    pBloomShader->addUniform("screenTexture");
    pBloomShader->addUniform("blurTexture");
    pBloomShader->addUniform("bloom");
    pBloomShader->addUniform("exposure");

    pAssets->loadShape("sphereShape", "sphere.shape");
    pAssets->loadShape("planeShape", "plane.shape");
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

    m_pPlayer1Bike = Presets::createLightRiderBike("Player1Bike", GC::player1Color,
        player1BikeControls, glm::vec3(-190.0f, 1.0f, 0.0f), -glm::pi<float>() * 0.5f);

    m_pPlayer2Bike = Presets::createLightRiderBike("Player2Bike", GC::player2Color,
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
