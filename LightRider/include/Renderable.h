#pragma once

#include "Component.h"
#include "Camera.h"

// A base Component that exposes render functionality.
class Renderable : public Component
{
public:

    // Instantiates a new Renderable instance, adding it to the scene's asset manager render tree.
    Renderable(const std::string& shaderProgramId, const std::string& imageTextureId, bool useBlending = false);

    // Destroys a Renderable instance, removing it from the scene's asset manager render tree.
    virtual ~Renderable();

    // Gets the ID of the shader program used.
    std::string& getShaderProgramId() { return m_shaderProgramId; }

    // Gets the ID of the texture used.
    std::string& getImageTextureId() { return m_imageTextureId; }

    // If true, the renderable uses alpha blending and will be added to the dynamic render tree.
    bool usesBlending() { return m_useBlending; }

    // Renders the Renderable.
    virtual void render() = 0;

    // Returns the depth of the object relative to the camera provided.
    virtual float getDepth(Camera* pCamera) const;

private:

    // The shader program ID.
    std::string m_shaderProgramId;

    // The texture ID.
    std::string m_imageTextureId;

    // If true, this renderable uses alpha blending and will be added to the dynamic render tree.
    bool m_useBlending;
};
