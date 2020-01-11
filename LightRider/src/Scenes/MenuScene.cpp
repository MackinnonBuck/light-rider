#include "Scenes/MenuScene.h"

#include "GameConstants.h"
#include "Presets.h"
#include "Components/OrbitCamera.h"
#include "Components/BikeRenderer.h"

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
}
