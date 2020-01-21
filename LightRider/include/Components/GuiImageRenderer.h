#pragma once

#include "Renderable.h"
#include "Program.h"
#include "Texture.h"
#include "Shape.h"

enum class HorizontalAnchor : unsigned char
{
    LEFT    = 0,
    RIGHT   = 1,
    CENTER  = 2,
    STRETCH = 3
};

enum class VerticalAnchor : unsigned char
{
    BOTTOM  = 0,
    TOP     = 1,
    CENTER  = 2,
    STRETCH = 3
};

class GuiImageRenderer : public Renderable
{
public:

    GuiImageRenderer(const std::string& shaderId, const std::string& textureId, Camera* pCamera);

    HorizontalAnchor getHorizontalAnchor() const { return m_horizontalAnchor; }

    void setHorizontalAnchor(HorizontalAnchor anchor) { m_horizontalAnchor = anchor; }

    VerticalAnchor getVeritcalAnchor() const { return m_verticalAnchor; }

    void setVerticalAnchor(VerticalAnchor anchor) { m_verticalAnchor = anchor; };

    glm::vec2 getScale() const { return m_scale; }
    
    void setScale(const glm::vec2& scale) { m_scale = scale; };

    glm::vec2 getScreenPosition() const;

    glm::vec2 getScreenSize() const;

    virtual void update(float deltaTime);

    virtual void render();

    virtual float getDepth(Camera* pCamera) const;

private:

    HorizontalAnchor m_horizontalAnchor;

    VerticalAnchor m_verticalAnchor;

    glm::vec2 m_scale;

    Program* m_pShaderProgram;

    Texture* m_pTexture;

    Shape* m_pShape;

    Camera* m_pCamera;

    glm::ivec2 m_viewPosition;

    glm::ivec2 m_viewSize;

    inline void updateViewTransform();
};

