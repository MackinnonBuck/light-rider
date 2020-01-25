#include "States/TitleScreenState.h"

#include "Game.h"
#include "Components/GuiElement.h"
#include "Components/ProcessedCamera.h"
#include "Components/OrbitCamera.h"
#include "States/MenuState.h"

void TitleScreenState::resume()
{
    State::resume();

    GameObject* pCameraObject = GameObject::find("Camera");
    pCameraObject->addComponent<OrbitCamera>();

    ProcessedCamera* pOrbitCamera = pCameraObject->getComponent<ProcessedCamera>();

    GameObject* pTitleImage = GameObject::create("TitleImage", getStateMachine()->getGameObject());
    GuiElement* pTitle = pTitleImage->addComponent<GuiElement>("guiImageShader", "titleTexture", pOrbitCamera);
    pTitle->setVerticalAnchor(VerticalAnchor::STRETCH);
    pTitle->setHorizontalAnchor(HorizontalAnchor::CENTER);
    pTitle->getTransform()->setPosition(glm::vec3(0, 0.7f, 0.0f));
    pTitle->setBloomFactor(3.0f);

    GameObject* pPressToBeginImage = GameObject::create("PressToBeginImage", getStateMachine()->getGameObject());
    GuiElement* pPressToBegin = pPressToBeginImage->addComponent<GuiElement>("guiImageShader", "pressToBeginTexture", pOrbitCamera);
    pPressToBegin->setVerticalAnchor(VerticalAnchor::STRETCH);
    pPressToBegin->setHorizontalAnchor(HorizontalAnchor::CENTER);
    pPressToBegin->getTransform()->setPosition(glm::vec3(0, 0.4f, 0.0f));
    pPressToBegin->setScale(glm::vec2(0.5f, 0.5f));
}

void TitleScreenState::pause()
{
    State::pause();

    GameObject* pCameraObject = GameObject::find("Camera");

    if (pCameraObject)
        pCameraObject->removeComponent<OrbitCamera>();

    GameObject::destroy(GameObject::find("TitleImage", getStateMachine()->getGameObject()));
    GameObject::destroy(GameObject::find("PressToBeginImage", getStateMachine()->getGameObject()));
}

void TitleScreenState::update(float deltaTime)
{
    State::update(deltaTime);

    if (Game::getInstance().wasAnyKeyPressed())
        getStateMachine()->pushState(new MenuState());
}
