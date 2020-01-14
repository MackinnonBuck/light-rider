#pragma once

#include "Renderable.h"
#include "Program.h"
#include "Shape.h"

class GuiImageRenderer : public Renderable
{
public:

    GuiImageRenderer(const std::string& shaderId, const std::string& textureId, float depth = 0.0f);

    // TODO: Matrix scaling according to screen size, customizeable anchoring.

    virtual void render();

    virtual float getDepth(Camera* pCamera) const;

private:

    bool m_isPositionInPixels;

    bool m_isSizeInPixels;

    float m_depth;

    Program* m_pShaderProgram;

    Shape* m_pShape;
};

