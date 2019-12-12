#pragma once

#include <glm/gtc/type_ptr.hpp>

#include "Game.h"
#include "Joystick.h"
#include "Components/ProcessedCamera.h"

// Represents the possible player camera modes.
enum class PlayerCameraMode
{
    INTRO,
    FOLLOW
};

// Holds joystick info and key codes which describe which keys correspond with certain parts of bike functionality.
struct PlayerCameraControls
{
    // The ID of the joystick to be used.
    int joystickId;

    // The opponent view key code (fallback if no joystick found).
    int opponentView;
};

// A camera that follows the provided player GameObject.
// Usage: addComponent(GameObject* pPlayerObject, GameObject* pOpponentObject, int opponentViewKey)
class PlayerCamera : public ProcessedCamera
{
public:

    // Creates a new PlayerCamera instance.
    PlayerCamera(GameObject* pPlayerObject, GameObject* pOpponentObject, const PlayerCameraControls& playerCameraControls) :
        ProcessedCamera("postShader"),
        m_pPlayerObject(pPlayerObject),
        m_pOpponentObject(pOpponentObject),
        m_cameraControls(playerCameraControls),
        m_pJoystick(Game::getInstance().getJoystick(playerCameraControls.joystickId)),
        m_mode(PlayerCameraMode::INTRO) { }

    // Returns the camera mode of this player camera.
    PlayerCameraMode getCameraMode() const { return m_mode; }

    // Sets the camera mode of this player camera.
    void setCameraMode(PlayerCameraMode cameraMode) { m_mode = cameraMode; }

    // Updates the camera's transform after all other objects in the scene have finished updating.
    virtual void postUpdate(float deltaTime);

private:

    // The player GameObject to follow.
    GameObject* m_pPlayerObject;

    // The opponent GameObject to look at in opponent-view mode.
    GameObject* m_pOpponentObject;

    // The key code of the key that triggers opponent-view mode.
    PlayerCameraControls m_cameraControls;

    // The joystick controlling this camera.
    Joystick* m_pJoystick;

    // The current camera mode.
    PlayerCameraMode m_mode;

    // Processes the player camera in intro mode.
    void processIntroMode(float deltaTime);

    // Processes the player camera in player follow mode.
    void processFollowMode(float deltaTime);

    // Gets the player-view camera transform.
    glm::mat4 getPlayerViewTransform();

    // Gets the opponent-view camera transform.
    glm::mat4 getOpponentViewTransform();
};

