#include <iostream>

#include "Game.h"
#include "GameConfig.h"
#include "Scenes/GameScene.h"
#include "Scenes/MenuScene.h"
#include "Tests/TestScene.h"

// IDEA for blending with mulitple platforms:
// We would add a few stages in the rendering pipeline:
// 1. Rather than rendering all blended objects at once, we would only render the trails.
//    * Trails must be rendered without writing to the depth buffer. We would instead write to a separate texture storing depth.
//      This way, when the ground renders later, it can determine if a pixel was rendered as a trail or not.
// 2. Render the ground.
//    * We would get the "trail" distance from the texture we generated, and use that to determine if
//      a.) We need to use additive blending (the trail is in front of the ground), or
//      b.) We need to use the ONE_MINUS_SOURCE_ALPHA blending (the tail is behind the ground).
//    * This would unfortunately lead to a slight artifact where if a trail is overlapping itself, but the ground is in between
//      the two trail layers, we would see an extra bright patch in the trail. This is a very small side effect so I don't think
//      we would need to worry too much about it.

int main(int argc, char** argv)
{
    // Define configurable game settings.
    GameConfig config;
    config.argumentCount = argc;
    config.arguments = argv;
    config.initialWindowWidth = 1280;
    config.initialWindowHeight = 720;
    //config.initialIsFullscreen = true;
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
