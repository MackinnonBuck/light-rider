#include "States/MenuState.h"

#include <glm/gtx/euler_angles.hpp>

#include "Game.h"
#include "Scenes/GameScene.h"
#include "Components/ProcessedCamera.h"
#include "Components/MenuCamera.h"
#include "Components/GuiButton.h"

MenuState::~MenuState()
{
    GameObject::find("Camera", getGameObject())->removeComponent<MenuCamera>();

    GameObject::destroy(GameObject::find("StartButton", getGameObject()));
    GameObject::destroy(GameObject::find("SettingsButton", getGameObject()));
    GameObject::destroy(GameObject::find("ExitButton", getGameObject()));
}

void MenuState::initialize()
{
    State::initialize();

    GameObject* pCameraObject = GameObject::find("Camera", getGameObject());
    ProcessedCamera* pCamera = pCameraObject->getComponent<ProcessedCamera>();
    pCameraObject->addComponent<MenuCamera>(glm::vec3(-2.5f, 2.0f, 5.0f), glm::eulerAngleYX(-0.25f, -0.15f));

    GameObject* pStartButtonObject = GameObject::create("StartButton", getGameObject());
    GuiButton* pStartButton = pStartButtonObject->addComponent<GuiButton>("guiImageShader", "playTexture", pCamera);
    pStartButton->setVerticalAnchor(VerticalAnchor::TOP);
    pStartButton->setHorizontalAnchor(HorizontalAnchor::LEFT);
    pStartButton->setScale(glm::vec2(0.75f, 0.75f));
    pStartButton->getTransform()->setPosition(glm::vec3(256.0f, 196.0f, 0.0f));
    pStartButton->setOnClick([] { Game::getInstance().changeScene(new GameScene()); });

    GameObject* pSettingsButtonObject = GameObject::create("SettingsButton", getGameObject());
    GuiButton* pSettingsButton = pSettingsButtonObject->addComponent<GuiButton>("guiImageShader", "settingsTexture", pCamera);
    pSettingsButton->setVerticalAnchor(VerticalAnchor::TOP);
    pSettingsButton->setHorizontalAnchor(HorizontalAnchor::LEFT);
    pSettingsButton->setScale(glm::vec2(0.75f, 0.75f));
    pSettingsButton->getTransform()->setPosition(glm::vec3(391.0f, 392.0f, 0.0f));

    GameObject* pExitButtonObject = GameObject::create("ExitButton", getGameObject());
    GuiButton* pExitButton = pExitButtonObject->addComponent<GuiButton>("guiImageShader", "exitTexture", pCamera);
    pExitButton->setVerticalAnchor(VerticalAnchor::TOP);
    pExitButton->setHorizontalAnchor(HorizontalAnchor::LEFT);
    pExitButton->setScale(glm::vec2(0.75f, 0.75f));
    pExitButton->getTransform()->setPosition(glm::vec3(227.5f, 588.0f, 0.0f));
    pExitButton->setOnClick([] { Game::getInstance().exit(); });
}

