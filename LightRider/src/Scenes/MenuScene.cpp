#include "Scenes/MenuScene.h"

#include "StateMachine.h"
#include "GameConstants.h"
#include "Presets.h"
#include "Components/OrbitCamera.h"
#include "Components/BikeRenderer.h"
#include "Scenes/GameScene.h"
#include "States/TitleScreenState.h"

namespace GC = GameConstants;

void MenuScene::initialize()
{
    LightRiderScene::initialize();
    
    loadAssets();

    Presets::createLightRiderGround("Ground");
    Presets::createLightRiderBikeDisplay("Player1BikeDisplay", 0,
        glm::vec3(1.0f, 0.4f, 0.0f), -glm::pi<float>() * 0.5f - 0.2f);
    Presets::createLightRiderBikeDisplay("Player2BikeDisplay", 1,
        glm::vec3(-1.0f, 0.4f, 0.0f), -glm::pi<float>() * 0.5f + 0.2f);

    GameObject* pStateMachineObject = GameObject::create("StateMachine");
    StateMachine* pStateMachine = pStateMachineObject->addComponent<StateMachine>();

    GameObject* pCamera = GameObject::create("Camera", pStateMachineObject);
    ProcessedCamera* pOrbitCamera = pCamera->addComponent<ProcessedCamera>(true);
    pOrbitCamera->setSky("skyShader", "skyTexture", "sphereShape");

    pStateMachine->pushState(new TitleScreenState());
}

void MenuScene::loadAssets()
{
    LightRiderScene::loadAssets();

    AssetManager* pAssets = getAssetManager();

    // UI assets.
    Program* pImageShader = pAssets->loadShaderProgram("guiImageShader", "gui_image_vertex.glsl", "gui_image_fragment.glsl",
        ShaderUniform::M_MATRIX
      | ShaderUniform::TEXTURE_0
    );

    pImageShader->addUniform("bloomFactor");

    pAssets->loadTexture("titleTexture", "title_image.png", TextureType::IMAGE);
    pAssets->loadTexture("pressToBeginTexture", "press_to_begin.png", TextureType::IMAGE);
    pAssets->loadTexture("playTexture", "play.png", TextureType::IMAGE);
    pAssets->loadTexture("settingsTexture", "settings.png", TextureType::IMAGE);
    pAssets->loadTexture("exitTexture", "exit.png", TextureType::IMAGE);
}
