#pragma once

#include "Scenes/LightRiderScene.h"
#include "Components/PlayerCamera.h"
#include "Components/DeathCamera.h"

// The game's main scene.
class GameScene : public LightRiderScene
{
public:

    // Creates a new GameScene instance.
    GameScene() : m_gameConfig(Game::getInstance().getConfig()), m_pFreeroamCamera(nullptr), m_pPlayer1Camera(nullptr), m_pPlayer2Camera(nullptr),
        m_pDeathCamera(nullptr), m_pPlayer1Bike(nullptr), m_pPlayer2Bike(nullptr),
        m_introTick(0), m_ticksUntilReset(-1) { }

    // Initializes the GameScene.
    virtual void initialize();

    // Updates the GameScene.
    virtual void update(float deltaTime);

    // Runs scene logic dependent on the physics tick.
    virtual void physicsTick(float physicsTimeStep);

private:

    // The global game configuration.
    GameConfig& m_gameConfig;

    // The scene's freeroam camera.
    Camera* m_pFreeroamCamera;

    // The scene's player 1 camera.
    PlayerCamera* m_pPlayer1Camera;

    // The scene's player 2 camera.
    PlayerCamera* m_pPlayer2Camera;

    // The death camera GameObject.
    DeathCamera* m_pDeathCamera;

    // The player 1 bike GameObject.
    GameObject* m_pPlayer1Bike;

    // The player 2 bike GameObject.
    GameObject* m_pPlayer2Bike;

    // The number of ticks since the scene was initialized.
    int m_introTick;

    // The number of physics ticks until the scene gets reset. A value of -1 indicates that the
    // scene will stay active indefinitely.
    int m_ticksUntilReset;

    // Initializes the scene.
    void initScene();

    // Clears the scene.
    void clearScene();
};
