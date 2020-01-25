#include "States/MenuState.h"

#include <glm/gtx/euler_angles.hpp>

#include "Components/MenuCamera.h"

MenuState::~MenuState()
{
    GameObject* pCameraObject = GameObject::find("Camera");

    if (pCameraObject)
        pCameraObject->removeComponent<MenuCamera>();
}

void MenuState::initialize()
{
    State::initialize();

    GameObject* pCameraObject = GameObject::find("Camera");
    pCameraObject->addComponent<MenuCamera>(glm::vec3(-2.5f, 2.0f, 5.0f), glm::eulerAngleYX(-0.25f, -0.15f));
}

