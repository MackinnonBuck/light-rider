#include "Game.h"

#include <iostream>
#include <assert.h>
#include <glad/glad.h>

#include "GLSL.h"

void glfwErrorCallback(int error, const char* description)
{
    std::cerr << description << std::endl;
}

bool Game::init(GameConfig config)
{
    if (m_gameState != GameState::UNINITIALIZED)
    {
        std::cerr << "Game has already been initialized!" << std::endl;
        return false;
    }

    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit())
    {
        std::cerr << "Could not init GLFW!" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    m_pWindow = glfwCreateWindow(config.initialWindowWidth, config.initialWindowHeight, config.windowTitle,
        config.initialIsFullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
    m_windowWidth = config.initialWindowWidth;
    m_windowHeight = config.initialWindowHeight;

    if (!m_pWindow)
    {
        glfwTerminate();

        std::cerr << "Could not create the game window!" << std::endl;
        return false;
    }

    glfwMakeContextCurrent(m_pWindow);

    if (!gladLoadGL())
    {
        std::cerr << "Could not initialize OpenGL!" << std::endl;
        return false;
    }

    glfwSwapInterval(1);

    glfwSetWindowCloseCallback(m_pWindow, Game::glfwWindowCloseCallback);
    glfwSetWindowSizeCallback(m_pWindow, Game::glfwWindowSizeCallback);
    glfwSetKeyCallback(m_pWindow, Game::glfwKeyCallback);
    glfwSetMouseButtonCallback(m_pWindow, Game::glfwMouseButtonCallback);
    glfwSetCursorPosCallback(m_pWindow, Game::glfwCursorPositionCallback);

    for (
        m_joystickCount = GLFW_JOYSTICK_1;
        m_joystickCount <= GLFW_JOYSTICK_LAST && glfwJoystickPresent(m_joystickCount);
        m_joystickCount++
    );

    m_pJoysticks = new Joystick*[m_joystickCount];

    for (int i = 0; i < m_joystickCount; i++)
    {
        Joystick* pJoystick = new Joystick();
        pJoystick->initialize(i);
        m_pJoysticks[i] = pJoystick;
    }

    m_config = config;
    m_gameState = GameState::INITIALIZED;

    return true;
}

void Game::run(Scene *pScene)
{
    assert(pScene);

    if (m_gameState != GameState::INITIALIZED)
    {
        std::cerr << "Game must be initialized but not running in order to start running!" << std::endl;
        return;
    }

    m_gameState = GameState::RUNNING;
    m_pNextScene = pScene;

    float maxDeltaTime = m_config.maxPhysicsSubSteps * m_config.physicsTickInterval;
    float lastTime = 0.0f;

    while (m_gameState == GameState::RUNNING)
    {
        float deltaTime;

        if (m_pNextScene)
        {
            if (m_pCurrentScene)
                delete m_pCurrentScene;

            m_pCurrentScene = m_pNextScene;
            m_pNextScene = nullptr;

            m_pCurrentScene->initialize();

            lastTime = (float)glfwGetTime();
            deltaTime = 0.0f;
        }
        else
        {
            float currentTime = (float)glfwGetTime();
            deltaTime = currentTime - lastTime;

            if (deltaTime < m_config.minDeltaTime)
                continue;

            lastTime = currentTime;
        }

        if (deltaTime > maxDeltaTime)
            deltaTime = maxDeltaTime;

        glfwPollEvents();

        for (int i = 0; i < m_joystickCount; i++)
            m_pJoysticks[i]->refresh();

        m_pCurrentScene->update(deltaTime);
        m_pCurrentScene->postUpdate(deltaTime);
        m_pCurrentScene->render();

        glfwSwapBuffers(m_pWindow);

        m_tick++;
    }

    if (m_pCurrentScene)
    {
        delete m_pCurrentScene;
        m_pCurrentScene = nullptr;
    }

    for (int i = 0; i < m_joystickCount; i++)
        delete m_pJoysticks[i];

    delete[] m_pJoysticks;

    glfwDestroyWindow(m_pWindow);
    glfwTerminate();
}

void Game::exit()
{
    if (m_gameState == GameState::RUNNING)
        m_gameState = GameState::UNINITIALIZED;
    else
        std::cerr << "Cannot exit a game that is not running!" << std::endl;
}

void Game::glfwWindowCloseCallback(GLFWwindow* pWindow)
{
    getInstance().exit();
}

void Game::glfwWindowSizeCallback(GLFWwindow* pWindow, int width, int height)
{
    Game& game = getInstance();

    game.m_windowWidth = width;
    game.m_windowHeight = height;
}

void Game::glfwKeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{
    Game& game = getInstance();

    switch (action)
    {
    case GLFW_PRESS:
        game.m_keys[key] = true;
        break;
    case GLFW_RELEASE:
        game.m_keys[key] = false;
        break;
    }
}

void Game::glfwMouseButtonCallback(GLFWwindow* pWindow, int button, int action, int mods)
{
    Game& game = getInstance();

    switch (action)
    {
    case GLFW_PRESS:
        game.m_mouseButtons[button] = true;
        break;
    case GLFW_RELEASE:
        game.m_mouseButtons[button] = false;
        break;
    }
}

void Game::glfwCursorPositionCallback(GLFWwindow* pWindow, double x, double y)
{
    Game& game = getInstance();

    game.m_cursorX = (float)x;
    game.m_cursorY = (float)y;
}
