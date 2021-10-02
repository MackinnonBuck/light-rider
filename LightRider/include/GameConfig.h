#pragma once

#include <string>

// Represents immutable game settings.
struct GameConfig
{
    // The number of command-line arguments passed to the game.
    int argumentCount = 0;

    // The command-line arguments passed to the game.
    char** arguments = nullptr;

    // The initial width of the game window.
    int initialWindowWidth = 1280;

    // The initial height of the game window.
    int initialWindowHeight = 720;

    // If true, the window launches initially fullscreen.
    int initialIsFullscreen = false;

    // The title of the game window.
    const char* windowTitle = nullptr;

    // The minimum delta time of the game. This value prevents the game from running at
    // extreme frame rates and drawing too much power.
    float minDeltaTime = 1.0f / 250.0f;

    // The interval of the game's physics tick.
    float physicsTickInterval = 1.0f / 60.0f;

    // The maximum number of physics ticks per frame.
    int maxPhysicsSubSteps = 1;

    // The root directory from where game resources (shaders, objects, etc.) are loaded.
    std::string resourceDirectory = std::string();
};
