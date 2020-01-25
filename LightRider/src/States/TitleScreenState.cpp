#include "States/TitleScreenState.h"

#include "Game.h"
#include "Components/GuiImageRenderer.h"
#include "Components/OrbitCamera.h"

TitleScreenState::~TitleScreenState()
{
    GameObject::destroy(GameObject::find("TitleImage"));
    GameObject::destroy(GameObject::find("PressToBeginImage"));
}

void TitleScreenState::initialize()
{
    State::initialize();

    Camera* pOrbitCamera = GameObject::find("Camera")->getComponent<OrbitCamera>();

    GameObject* pTitleImage = GameObject::create("TitleImage");
    GuiImageRenderer* pTitleRenderer = pTitleImage->addComponent<GuiImageRenderer>("guiImageShader", "titleTexture", pOrbitCamera);
    pTitleRenderer->setVerticalAnchor(VerticalAnchor::STRETCH);
    pTitleRenderer->setHorizontalAnchor(HorizontalAnchor::CENTER);
    pTitleRenderer->getTransform()->setPosition(glm::vec3(0, 0.7f, 0.0f));
    pTitleRenderer->setBloomFactor(3.0f);

    GameObject* pPressToBeginImage = GameObject::create("PressToBeginImage");
    GuiImageRenderer* pPressToBeginRenderer = pPressToBeginImage->addComponent<GuiImageRenderer>("guiImageShader", "pressToBeginTexture", pOrbitCamera);
    pPressToBeginRenderer->setVerticalAnchor(VerticalAnchor::STRETCH);
    pPressToBeginRenderer->setHorizontalAnchor(HorizontalAnchor::CENTER);
    pPressToBeginRenderer->getTransform()->setPosition(glm::vec3(0, 0.4f, 0.0f));
    pPressToBeginRenderer->setScale(glm::vec2(0.5f, 0.5f));
}

void TitleScreenState::update(float deltaTime)
{
    State::update(deltaTime);

    if (Game::getInstance().wasAnyKeyPressed())
        getStateMachine()->popState();
}
