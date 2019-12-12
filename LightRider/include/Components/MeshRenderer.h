#pragma once

#include "Renderable.h"
#include "Program.h"
#include "Shape.h"

// Renders a mesh located at the attached GameObject.
// Usage: addComponent(const std::string& shaderProgramId, const std::string& primaryTextureId, const std::string& shapeId).
class MeshRenderer : public Renderable
{
public:

    // Creates a new MeshRenderer instance.
    MeshRenderer(const std::string& shaderProgramId, const std::string& primaryTextureId,
        const std::string& shapeId, bool useBlending = false);

    // Gets the transform of the mesh relative to the parent GameObject.
    glm::mat4& getLocalTransform() { return m_localTransform; }

    // Sets the local transform of the mesh relative to the parent GameObject.
    void setLocalTransform(glm::mat4 localTransform) { m_localTransform = localTransform; }

    // Sets the depth of the mesh renderer to the provided depth function pointer.
    void setDepthFunction(float (*pDepthFunc)(Camera*)) { m_pDepthFunc = pDepthFunc; }

    // Renders the mesh.
    virtual void render();

    // Gets the depth of this mesh renderer relative to the given camera.
    virtual float getDepth(Camera* pCamera) const;

protected:

    // Returns the shader program used to render the mesh.
    Program* getShaderProgram() const { return m_pShaderProgram; }

private:

    // The string ID of the shape being rendered.
    std::string m_shapeId;

    // The shader program uesd to render the mesh.
    Program* m_pShaderProgram;

    // The shape being rendered.
    Shape* m_pShape;

    // The local transform of the mesh relative to the parent GameObject.
    glm::mat4 m_localTransform;

    // A custom depth function that can be used to override the default depth function.
    float (*m_pDepthFunc)(Camera*);
};
