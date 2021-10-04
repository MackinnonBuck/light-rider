#pragma once

#include "Components/GuiElement.h"

class GuiButton : public GuiElement
{
public:

    GuiButton(const std::string& shaderId, const std::string& textureId, Camera* pCamera);

    void setOnClick(void (*pOnClick)());

    virtual void update(float deltaTime);

private:

    void (*m_pOnClick)();

    bool m_pressed;
};

