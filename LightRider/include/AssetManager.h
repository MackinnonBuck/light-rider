#pragma once

#include <string>
#include <map>
#include <vector>
#include <unordered_map>

#include "GameConfig.h"
#include "Camera.h"
#include "Renderable.h"
#include "Program.h"
#include "ComputeProgram.h"
#include "Shape.h"
#include "Texture.h"

// Represents the render heirarchy, sorted first by shader, then by texture.
typedef std::vector<Renderable*> TextureNode;
typedef std::map<Texture*, TextureNode*> ProgramNode;
typedef std::map<Program*, ProgramNode*> RenderTree;

// Contains data necessary to render a Renderable with blending enabled.
struct BlendedNode
{
    Renderable* pRenderable;
    Program* pShaderProgram;
    Texture* pTexture;
};

// Represents commonly used shader uniforms.
enum class ShaderUniform : unsigned
{
    NONE = 0,
    P_MATRIX = 1,
    V_MATRIX = 2,
    M_MATRIX = 4,
    TEXTURE_0 = 8,
    TEXTURE_1 = 16,
    TEXTURE_2 = 32,
    TEXTURE_3 = 64,
    DEFAULT = 15,
    ALL = 127,
};

// Bitwise ORs two shader uniforms together, returning the resulting ShaderUniform.
ShaderUniform operator|(ShaderUniform lhs, ShaderUniform rhs);

// Bitwise ANDs two shader uniforms together, returning the result as an unsigned integer.
unsigned operator&(ShaderUniform lhs, ShaderUniform rhs);

// Manages game assets.
class AssetManager
{
public:

    // Creates a new AssetManager instance.
    AssetManager();

    // Destroys the AsseManager, releasing all loaded assets.
    ~AssetManager();

    // Loads a shader program from the given vertex and fragment shader file names.
    // Uniforms will be added according to the specified defaultUniforms argument.
    Program* loadShaderProgram(const std::string& id, const std::string& vertexShaderFileName,
        const std::string& fragmentShaderFileName, ShaderUniform defaultUniforms = ShaderUniform::DEFAULT);

    // Loads a shader program from the given vertex and fragment shader file names.
    // Uniforms will be added according to the specified defaultUniforms argument.
    Program* loadShaderProgram(const std::string& id, const std::string& vertexShaderFileName,
        const std::vector<std::string>& fragmentShaderFileNames, ShaderUniform defaultUniforms = ShaderUniform::DEFAULT);

    // Loads a compute shader program from the given file name.
    ComputeProgram* loadComputeShaderProgram(const std::string& id, const std::string& fileName);

    // Loads a texture from the given texture file name. The TextureType is meant to indicate how the
    // texture will be used.
    Texture* loadTexture(const std::string& id, const std::string& fileName, TextureType textureType);

    // Loads a shape from the given file name.
    Shape* loadShape(const std::string& id, const std::string& fileName);

    // Returns a loaded shader program by ID.
    Program* getShaderProgram(const std::string& id) const { return m_shaderPrograms.at(id); }

    // Returns a loaded compute shader program by ID.
    ComputeProgram* getComputeShaderProgram(const std::string& id) const { return m_computeShaderPrograms.at(id); }

    // Returns a loaded texture by ID.
    Texture* getTexture(const std::string& id) const { return m_textures.at(id); }

    // Returns a loaded shape by ID.
    Shape* getShape(const std::string& id) const { return m_shapes.at(id); }

    // Gets the generated render tree.
    RenderTree& getRenderTree() { return m_renderTree; }

    // Gets the latest sorted list of blending renderables.
    std::vector<BlendedNode*>& getSortedBlendedRenderables() { return m_sortedBlendedNodes; }

    // Sorts the Renderables 
    void sortBlendedRenderables(Camera* pCamera);

    // Registers a renderable, adding it to the render tree.
    void _registerRenderable(Renderable* pRenderable);
    
    // Unregisters a renderable, removing it from the render tree.
    void _unregisterRenderable(Renderable* pRenderable);

private:

    // Maps shader program IDs to loaded shader programs.
    std::map<std::string, Program*> m_shaderPrograms;

    // Maps compute shader program IDs to loaded compute shader programs.
    std::map<std::string, ComputeProgram*> m_computeShaderPrograms;

    // Maps texture IDs to loaded textures.
    std::map<std::string, Texture*> m_textures;

    // Maps shape IDs to loaded shapes.
    std::map<std::string, Shape*> m_shapes;

    // The global game configuration.
    GameConfig& m_gameConfig;

    // The render tree generated when Renderables are registered.
    RenderTree m_renderTree;

    // A set of Renderables with blending enabled.
    std::unordered_map<Renderable*, BlendedNode*> m_blendedNodes;

    // A sorted list of Renderables with blending enabled.
    std::vector<BlendedNode*> m_sortedBlendedNodes;

    // Registers a texture uniform to the given shader program.
    void registerTextureUniform(Program* pProgram, const std::string& shaderId, unsigned textureUniformId);

    // Visits a node in the render tree, returning the child node of the given parent, matching the
    // provided value.
    template<typename T, typename U, typename V>
    T* visitRenderTreeNode(U* pParentNode, V* value);
};
