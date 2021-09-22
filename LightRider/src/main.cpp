#include <iostream>

#include "Game.h"
#include "GameConfig.h"
#include "Scenes/GameScene.h"
#include "Scenes/MenuScene.h"
#include "Tests/TestScene.h"

int main(int argc, char** argv)
{
    // Define configurable game settings.
    GameConfig config;
    config.argumentCount = argc;
    config.arguments = argv;
    config.initialWindowWidth = 1280;
    config.initialWindowHeight = 1080;
    config.initialIsFullscreen = false;
    config.windowTitle = "LightRider";
    config.physicsTickInterval = 1.0f / 60.0f;
    config.maxPhysicsSubSteps = 10;
    config.resourceDirectory = "../LightRider/resources/";

    // Initialize and run the game.
    Game& game = Game::getInstance();
    game.init(config);
    game.run(new MenuScene());
    //game.run(new GameScene());
    //game.run(new TestScene());

    return 0;
}
