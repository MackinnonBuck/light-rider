#pragma once

#include <GLFW/glfw3.h>

#include "GameConfig.h"
#include "Scene.h"
#include "Joystick.h"

// Represents the current state of the game.
enum class GameState
{
    UNINITIALIZED,
    INITIALIZED,
    RUNNING,
};

// Handles the game loop and window events.
class Game
{
public:

    // Returns the singleton Game instance.
    static Game& getInstance()
    {
        static Game instance;
        return instance;
    }

    // Initializes the game with the given GameConfig.
    bool init(GameConfig config);

    // Runs the game, starting with the provided scene.
    void run(Scene* pScene);

    // Changes the scene that the game is running.
    void changeScene(Scene *pScene);

    // Exits the game.
    void exit();

    // Returns the game configuration structure.
    GameConfig& getConfig() { return m_config; }

    // Returns the GLFW window handle.
    GLFWwindow* getWindowHandle() const { return m_pWindow; }

    // Returns the width of the game window.
    int getWindowWidth() const { return m_windowWidth; }

    // Returns the height of the game window.
    int getWindowHeight() const { return m_windowHeight; }

    // Returns true if the key of the given key code is being pressed, otherwise false.
    bool isKeyDown(int keyCode) const { return m_keys[keyCode]; }

    // Returns true if the mouse button of the given button code is being pressed, otherwise false.
    bool isMouseButtonDown(int buttonCode) const { return m_mouseButtons[buttonCode]; }

    // Returns the X position of the cursor.
    float getCursorX() const { return m_cursorX; }

    // Returns the Y position of the cursor.
    float getCursorY() const { return m_cursorY; }

    // Returns the joystick with the given ID, or nullptr if the controller could not be found.
    Joystick* getJoystick(int id) const { return id >= m_joystickCount ? nullptr : m_pJoysticks[id]; }

    // Returns true if a key or mouse button was pressed within the last frame.
    bool wasAnyKeyPressed() const { return m_wasAnyKeyPressed; }

    // Returns the currently loaded scene.
    Scene* getScene() const { return m_pCurrentScene; }

    // Returns the current game tick. This is guaranteed to be unique and can be treated as a
    // frame hash.
    long getTick() const { return m_tick; }

    // Prevents creating copies of the game instance.
    Game(Game const&) = delete;

    // Prevents reassigning references to the game instance.
    void operator=(Game const&) = delete;

private:

    // The global game configuration.
    GameConfig m_config;

    // The current state of the game.
    GameState m_gameState;

    // The game window.
    GLFWwindow *m_pWindow;

    // The width of the game window.
    int m_windowWidth;

    // The height of the game window.
    int m_windowHeight;

    // The current scene.
    Scene *m_pCurrentScene;

    // The scene waiting to be started.
    Scene *m_pNextScene;

    // The current game tick.
    long m_tick;

    // Key states, indexed by GLFW key code.
    bool m_keys[GLFW_KEY_LAST + 1];

    // Mouse button states, indexed by GLFW button code.
    bool m_mouseButtons[GLFW_MOUSE_BUTTON_LAST + 1];

    // The number of joysticks connected.
    int m_joystickCount;

    // An array of all connected joysticks.
    Joystick** m_pJoysticks;

    // The X position of the cursor.
    float m_cursorX;

    // The Y position of the cursor.
    float m_cursorY;

    // If true, a key was pressed within the last frame.
    bool m_wasAnyKeyPressed;

    // Called when the close button is clicked.
    static void glfwWindowCloseCallback(GLFWwindow* pWindow);

    // Called when the window is resized.
    static void glfwWindowSizeCallback(GLFWwindow* pWindow, int width, int height);

    // Called when a key event occurs.
    static void glfwKeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods);

    // Called when a button event occurs.
    static void glfwMouseButtonCallback(GLFWwindow* pWindow, int button, int action, int mods);

    // Called when the cursor moves.
    static void glfwCursorPositionCallback(GLFWwindow* pWindow, double x, double y);

    // Creates the Game instance.
    Game() : m_config(), m_gameState(GameState::UNINITIALIZED), m_pWindow(nullptr),
        m_pCurrentScene(nullptr), m_pNextScene(nullptr), m_tick(0), m_keys{false} { }
};

