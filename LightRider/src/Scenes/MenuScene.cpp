#include "Scenes/MenuScene.h"

#include "GameConstants.h"
#include "Presets.h"
#include "Components/OrbitCamera.h"
#include "Components/BikeRenderer.h"
#include "Components/GuiImageRenderer.h"
#include "Scenes/GameScene.h"

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

    GameObject* pTitleImage = GameObject::create("TitleImage");
    GuiImageRenderer* titleImage = pTitleImage->addComponent<GuiImageRenderer>("guiImageShader", "titleTexture", pOrbitCamera);
    titleImage->setVerticalAnchor(VerticalAnchor::STRETCH);
    titleImage->setHorizontalAnchor(HorizontalAnchor::CENTER);
    titleImage->getTransform()->setPosition(glm::vec3(0, 0.7f, 0.0f));
}

void MenuScene::postUpdate(float deltaTime)
{
    LightRiderScene::postUpdate(deltaTime);

    if (Game::getInstance().wasAnyKeyPressed())
        Game::getInstance().changeScene(new GameScene());
}

void MenuScene::loadAssets()
{
    LightRiderScene::loadAssets();

    AssetManager* pAssets = getAssetManager();

    // UI assets.
    Program* pGuiImageShader = pAssets->loadShaderProgram("guiImageShader", "gui_image_vertex.glsl", "gui_image_fragment.glsl",
        ShaderUniform::M_MATRIX
      | ShaderUniform::TEXTURE_0
    );

    Texture* pTitleTexture = pAssets->loadTexture("titleTexture", "title_image.png", TextureType::IMAGE);
}
