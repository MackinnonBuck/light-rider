#include "Renderable.h"

#include "Game.h"

Renderable::Renderable(const std::string& shaderProgramId, const std::string& primaryTextureId, bool useBlending) :
    m_shaderProgramId(shaderProgramId), m_imageTextureId(primaryTextureId), m_useBlending(useBlending)
{
    Game::getInstance().getScene()->getAssetManager()->_registerRenderable(this);
}

Renderable::~Renderable()
{
    Game::getInstance().getScene()->getAssetManager()->_unregisterRenderable(this);
}

float Renderable::getDepth(Camera* pCamera) const
{
    return glm::length(getTransform()->getPosition() - pCamera->getTransform()->getPosition());
}
