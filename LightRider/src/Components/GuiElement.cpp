#include "Components/GuiElement.h"

#include "Game.h"

GuiElement::GuiElement(const std::string& shaderId, const std::string& textureId, Camera* pCamera) : 
    Renderable(shaderId, textureId, false),
    m_horizontalAnchor(HorizontalAnchor::LEFT),
    m_verticalAnchor(VerticalAnchor::BOTTOM),
    m_scale(1.0f, 1.0f),
    m_bloomFactor(1.0f)
{
    AssetManager* pAssets = Game::getInstance().getScene()->getAssetManager();

    m_pShaderProgram = pAssets->getShaderProgram(shaderId);
    m_pShaderProgram->setVerbose(false);

    if (m_pShaderProgram->getAttribute("vertexPosition") == -1)
        m_pShaderProgram->addAttribute("vertexPosition");

    if (m_pShaderProgram->getAttribute("vertexNormal") == -1)
        m_pShaderProgram->addAttribute("vertexNormal");

    if (m_pShaderProgram->getAttribute("vertexTexture") == -1)
        m_pShaderProgram->addAttribute("vertexTexture");

    m_pShaderProgram->setVerbose(true);

    m_pTexture = pAssets->getTexture(textureId);

    m_pShape = pAssets->getShape("planeShape");

    m_pCamera = pCamera;

    updateViewTransform();
}

glm::vec2 GuiElement::getScreenPosition() const
{
    glm::vec2 transformPosition = getTransform()->getPosition();
    glm::vec2 screenPosition = glm::vec2();

    switch (m_horizontalAnchor)
    {
    case HorizontalAnchor::LEFT:
        screenPosition.x = transformPosition.x;
        break;
    case HorizontalAnchor::RIGHT:
        screenPosition.x = m_viewSize.x - transformPosition.x;
        break;
    case HorizontalAnchor::CENTER:
        screenPosition.x = m_viewSize.x * 0.5f + transformPosition.x;
        break;
    case HorizontalAnchor::STRETCH:
        screenPosition.x = m_viewSize.x * transformPosition.x;
        break;
    }

    switch (m_verticalAnchor)
    {
    case VerticalAnchor::BOTTOM:
        screenPosition.y = transformPosition.y;
        break;
    case VerticalAnchor::TOP:
        screenPosition.y = m_viewSize.y - transformPosition.y;
        break;
    case VerticalAnchor::CENTER:
        screenPosition.y = m_viewSize.y * 0.5f + transformPosition.y;
        break;
    case VerticalAnchor::STRETCH:
        screenPosition.y = m_viewSize.y * transformPosition.y;
        break;
    }

    return screenPosition;
}

glm::vec2 GuiElement::getScreenSize() const
{
    return m_scale * glm::vec2(m_pTexture->getWidth(), m_pTexture->getHeight());
}

void GuiElement::update(float deltaTime)
{
    Renderable::update(deltaTime);

    updateViewTransform();
}

void GuiElement::render()
{
    if (Game::getInstance().getScene()->getActiveCamera() != m_pCamera)
        return;

    glm::vec2 screenPosition = getScreenPosition();
    glm::vec2 screenSize = getScreenSize();

    glm::mat4 transformMatrix =
        glm::translate(glm::mat4(1.0f), glm::vec3(
            (screenPosition.x / m_viewSize.x - 0.5f) * 2,
            (screenPosition.y / m_viewSize.y - 0.5f) * 2,
            getTransform()->getPosition().z)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(
            screenSize.x / m_viewSize.x,
            screenSize.y / m_viewSize.y,
            1.0f
        ));

    glUniform1f(m_pShaderProgram->getUniform("bloomFactor"), m_bloomFactor);
    glUniformMatrix4fv(m_pShaderProgram->getUniform("M"), 1, GL_FALSE, &transformMatrix[0][0]);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pShape->draw(m_pShaderProgram);
}

float GuiElement::getDepth(Camera* pCamera) const
{
    return getTransform()->getPosition().z;
}

void GuiElement::updateViewTransform()
{
    m_pCamera->getViewport(m_viewPosition.x, m_viewPosition.y, m_viewSize.x, m_viewSize.y);
}
