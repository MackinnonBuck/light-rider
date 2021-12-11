#pragma once

#include "Component.h"
#include "Camera.h"
#include "Program.h"

// A base Component that exposes render functionality.
class Renderable : public Component
{
public:
    // Instantiates a new Renderable instance, adding it to the scene's asset manager render tree.
    Renderable(
        const std::string& shaderProgramId,
        const std::string& imageTextureId,
        bool useBlending = false,
        bool useDetailedShadows = false);

    // Destroys a Renderable instance, removing it from the scene's asset manager render tree.
    virtual ~Renderable();

    // Gets the ID of the shader program used.
    const std::string& getShaderProgramId() const { return m_shaderProgramId; }

    // Gets the ID of the image texture used.
    const std::string& getImageTextureId() const { return m_imageTextureId; }

    // Gets the ID of the normal texture used.
    virtual const std::string& getNormalTextureId() const { return ""; }

    // If true, the renderable uses alpha blending and will be added to the dynamic render tree.
    bool usesBlending() { return m_useBlending; }

    // If true, the renderable uses detailed shadows using its specified shader rather than the depth pass shader.
    bool usesDetailedShadows() { return m_useDetailedShadows; }

    // Renders the Renderable.
    virtual void render() = 0;

    // Renders the Renderable to a depth buffer given the shader program to do so.
    virtual bool renderDepth(Program* pDepthProgram) { return false; }

    // Returns the depth of the object relative to the camera provided.
    virtual float getDepth(Camera* pCamera) const;

private:

    // The shader program ID.
    std::string m_shaderProgramId;

    // The texture ID.
    std::string m_imageTextureId;

    // If true, this renderable uses alpha blending and will be added to the dynamic render tree.
    bool m_useBlending;

    bool m_useDetailedShadows;
};
