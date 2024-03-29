#include "Components/FreeroamCamera.h"

#include <glm/gtx/euler_angles.hpp>

#include "Game.h"

constexpr float panSensitivity = 0.05f;
constexpr float flyTranslationSensitivity = 0.1f;
constexpr float flyRotationSensitivity = 0.001f;

void FreeroamCamera::postUpdate(float deltaTime)
{
    if (!isEnabled() && !m_isMoving)
        return;

    Game& game = Game::getInstance();

    bool leftButtonDown = game.isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT);
    bool rightButtonDown = game.isMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT);

    float cursorX = game.getCursorX();
    float cursorY = game.getCursorY();

    float dX = cursorX - m_lastCursorX;
    float dY = cursorY - m_lastCursorY;

    m_lastCursorX = cursorX;
    m_lastCursorY = cursorY;

    if (leftButtonDown || rightButtonDown)
    {
        if (!m_isMoving)
        {
            m_isMoving = true;
            m_initialCursorX = cursorX;
            m_initialCursorY = cursorY;

            glfwSetInputMode(game.getWindowHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else
        {
            if (leftButtonDown && rightButtonDown)
            {
                glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(dX * panSensitivity, -dY * panSensitivity, 0.0f));

                getTransform()->multiply(translation);
            }
            else if (leftButtonDown)
            {
                glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, dY * flyTranslationSensitivity));

                m_eulerAngles.y -= dX * flyRotationSensitivity;

                getTransform()->setRotation(glm::quat(m_eulerAngles));
                getTransform()->multiply(translation);
            }
            else
            {
                m_eulerAngles.y -= dX * flyRotationSensitivity;
                m_eulerAngles.x -= dY * flyRotationSensitivity;

                getTransform()->setRotation(glm::quat(m_eulerAngles));
            }
        }
    }
    else if (m_isMoving)
    {
        m_isMoving = false;

        glfwSetInputMode(game.getWindowHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPos(game.getWindowHandle(), m_initialCursorX, m_initialCursorY);
    }

    ProcessedCamera::postUpdate(deltaTime);
}
