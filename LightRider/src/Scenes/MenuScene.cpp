#include "Scenes/MenuScene.h"

#include "StateMachine.h"
#include "GameConstants.h"
#include "Presets.h"
#include "Components/OrbitCamera.h"
#include "Components/BikeRenderer.h"
#include "Components/GuiImageRenderer.h"
#include "Scenes/GameScene.h"
#include "States/TitleScreenState.h"

namespace GC = GameConstants;

void MenuScene::initialize()
{
    LightRiderScene::initialize();
    
    loadAssets();

    Presets::createLightRiderGround("Ground");
    Presets::createLightRiderBikeDisplay("Player1BikeDisplay", GC::player1Color,
        glm::vec3(1.0f, 0.4f, 0.0f), -glm::pi<float>() * 0.5f - 0.2f);
    Presets::createLightRiderBikeDisplay("Player2BikeDisplay", GC::player2Color,
        glm::vec3(-1.0f, 0.4f, 0.0f), -glm::pi<float>() * 0.5f + 0.2f);

    GameObject* pCamera = GameObject::create("Camera");
    OrbitCamera* pOrbitCamera = pCamera->addComponent<OrbitCamera>(true);
    pOrbitCamera->setSky("skyShader", "skyTexture", "sphereShape");

    GameObject* pStateMachineObject = GameObject::create("StateMachine");
    StateMachine* pStateMachine = pStateMachineObject->addComponent<StateMachine>();
    pStateMachine->pushState(new TitleScreenState());
}

void MenuScene::postUpdate(float deltaTime)
{
    LightRiderScene::postUpdate(deltaTime);

    //if (Game::getInstance().wasAnyKeyPressed())
    //    Game::getInstance().changeScene(new GameScene());
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
}
