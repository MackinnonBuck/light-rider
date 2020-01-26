#include "Components/GuiButton.h"

#include "Game.h"

GuiButton::GuiButton(const std::string& shaderId, const std::string& textureId, Camera* pCamera) :
    GuiElement(shaderId, textureId, pCamera),
    m_pOnClick(nullptr)
{
}

void GuiButton::setOnClick(void (*pOnClick)())
{
    m_pOnClick = pOnClick;
}

void GuiButton::update(float deltaTime)
{
    GuiElement::update(deltaTime);

    Game& game = Game::getInstance();

    float cursorX = game.getCursorX();
    float cursorY = game.getWindowHeight() - game.getCursorY();

    glm::vec2 screenPosition = getScreenPosition();
    glm::vec2 screenSize = getScreenSize();

    bool mouseButtonDown = game.isMouseButtonDown(GLFW_MOUSE_BUTTON_1);

    if (cursorX >= screenPosition.x - screenSize.x * 0.5f &&
        cursorX <= screenPosition.x + screenSize.x * 0.5f &&
        cursorY >= screenPosition.y - screenSize.y * 0.5f &&
        cursorY <= screenPosition.y + screenSize.y * 0.5f)
    {
        setBloomFactor(std::fmin(1.5f, getBloomFactor() + deltaTime * 6.0f));

        if (!m_pressed)
        {
            if (mouseButtonDown)
            {
                m_pressed = true;
                
                if (m_pOnClick)
                    m_pOnClick();
            }
        }
    }
    else
    {
        setBloomFactor(std::fmax(0.35f, getBloomFactor() - deltaTime * 6.0f));
    }

    if (!mouseButtonDown)
        m_pressed = false;
}
