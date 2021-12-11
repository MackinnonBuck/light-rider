#include "AssetManager.h"

#include <iostream>
#include <algorithm>

#include "Game.h"

ShaderUniform operator|(ShaderUniform lhs, ShaderUniform rhs)
{
    return static_cast<ShaderUniform>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
}

unsigned operator&(ShaderUniform lhs, ShaderUniform rhs)
{
    return static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs);
}

AssetManager::AssetManager() :
    m_shaderPrograms(),
    m_textures(),
    m_shapes(),
    m_gameConfig(Game::getInstance().getConfig()),
    m_renderTree(),
    m_blendedNodes(),
    m_sortedBlendedNodes()
{
}

AssetManager::~AssetManager()
{
    m_renderTree.clear();

    for (auto& pair : m_shaderPrograms)
    {
        delete pair.second;
    }
    m_shaderPrograms.clear();

    for (auto& pair : m_computeShaderPrograms)
    {
        delete pair.second;
    }
    m_computeShaderPrograms.clear();

    for (auto& pair : m_textures)
    {
        delete pair.second;
    }
    m_textures.clear();

    for (auto& pair : m_shapes)
    {
        delete pair.second;
    }
    m_shapes.clear();
}

Program* AssetManager::loadShaderProgram(const std::string& id, const std::string& vertexShaderFileName,
    const std::vector<std::string>& fragmentShaderFileNames, ShaderUniform defaultUniforms)
{
    if (id.empty())
    {
        std::cout << "Cannot create a shader with an empty ID!" << std::endl;
        return nullptr;
    }

    std::vector<std::string> fragmentShaderFilePaths;
    fragmentShaderFilePaths.reserve(fragmentShaderFileNames.size());

    for (auto& fileName : fragmentShaderFileNames)
    {
        fragmentShaderFilePaths.push_back(m_gameConfig.resourceDirectory + fileName);
    }

    Program* pProgram = new Program();
    pProgram->setVerbose(true);
    pProgram->setShaderNames(m_gameConfig.resourceDirectory + vertexShaderFileName, fragmentShaderFilePaths);

    if (!pProgram->init())
    {
        std::cerr << "Could not initialize shader from \"" << vertexShaderFileName << '"';

        for (auto& fragmentShaderName : fragmentShaderFileNames)
        {
            std::cerr << ", \"" << fragmentShaderName << '"';
        }

        std::cerr << std::endl;

        delete pProgram;
        return nullptr;
    }

    if (defaultUniforms & ShaderUniform::P_MATRIX)
        pProgram->addUniform("P");

    if (defaultUniforms & ShaderUniform::V_MATRIX)
        pProgram->addUniform("V");

    if (defaultUniforms & ShaderUniform::M_MATRIX)
        pProgram->addUniform("M");

    if (defaultUniforms & (
        ShaderUniform::TEXTURE_0
      | ShaderUniform::TEXTURE_1
      | ShaderUniform::TEXTURE_2
      | ShaderUniform::TEXTURE_3
        ))
    {
        glUseProgram(pProgram->getPid());

        if (defaultUniforms & ShaderUniform::TEXTURE_0)
            registerTextureUniform(pProgram, id, 0);

        if (defaultUniforms & ShaderUniform::TEXTURE_1)
            registerTextureUniform(pProgram, id, 1);

        if (defaultUniforms & ShaderUniform::TEXTURE_2)
            registerTextureUniform(pProgram, id, 2);

        if (defaultUniforms & ShaderUniform::TEXTURE_3)
            registerTextureUniform(pProgram, id, 3);
    }

    m_shaderPrograms[id] = pProgram;

    return pProgram;

}

Program* AssetManager::loadShaderProgram(const std::string& id, const std::string& vertexShaderFileName,
        const std::string& fragmentShaderFileName, ShaderUniform defaultUniforms)
{
    return loadShaderProgram(id, vertexShaderFileName, std::vector<std::string>({ fragmentShaderFileName }), defaultUniforms);
}

ComputeProgram* AssetManager::loadComputeShaderProgram(const std::string& id, const std::string& fileName)
{
    if (id.empty())
    {
        std::cout << "Cannot create a shader with an empty ID!" << std::endl;
        return nullptr;
    }

    ComputeProgram* pProgram = ComputeProgram::create(m_gameConfig.resourceDirectory + fileName);

    m_computeShaderPrograms[id] = pProgram;

    return pProgram;
}

Texture* AssetManager::loadTexture(const std::string& id, const std::string& fileName, TextureType textureType)
{
    if (id.empty())
    {
        std::cout << "Cannot create a texture with an empty ID!" << std::endl;
        return nullptr;
    }

    Texture* pTexture = new Texture();

    pTexture->init(m_gameConfig.resourceDirectory + fileName, textureType);

    m_textures[id] = pTexture;

    return pTexture;
}

Shape* AssetManager::loadShape(const std::string& id, const std::string& fileName)
{
    if (id.empty())
    {
        std::cout << "Cannot create a shape with an empty ID!" << std::endl;
        return nullptr;
    }

    Shape* pShape = new Shape();

    pShape->loadMesh(m_gameConfig.resourceDirectory + fileName);
    pShape->resize();
    pShape->init();

    m_shapes[id] = pShape;

    return pShape;
}

void AssetManager::sortBlendedRenderables(Camera* pCamera)
{
    m_sortedBlendedNodes.clear();

    for (auto pair : m_blendedNodes)
        m_sortedBlendedNodes.push_back(pair.second);

    std::sort(m_sortedBlendedNodes.begin(), m_sortedBlendedNodes.end(),
        [=](BlendedNode* pNode1, BlendedNode* pNode2)
        {
            return pNode1->pRenderable->getDepth(pCamera) > pNode2->pRenderable->getDepth(pCamera);
        });
}

void AssetManager::_registerRenderable(Renderable* pRenderable)
{
    const std::string& shaderProgramId = pRenderable->getShaderProgramId();
    const std::string& imageTextureId = pRenderable->getImageTextureId();

    Program* pShaderProgram = m_shaderPrograms[shaderProgramId];

    if (!pShaderProgram && !shaderProgramId.empty())
        std::cerr << "Warning: could not find shader program with ID \"" << shaderProgramId << "\"!" << std::endl;

    Texture* pTexture = m_textures[imageTextureId];

    if (!pTexture)
    {
        if (!imageTextureId.empty())
            std::cerr << "Warning: could not find texture with ID \"" << imageTextureId << "\"!" << std::endl;
    }
    else if (pTexture->getTextureType() != TextureType::IMAGE)
    {
        std::cerr << "Warning: Texture \"" << imageTextureId << "\" is not an image texture!" << std::endl;
    }

    if (pRenderable->usesBlending())
    {
        m_blendedNodes.insert(std::pair<Renderable*, BlendedNode*>(pRenderable, new BlendedNode
            {
                pRenderable,
                pShaderProgram,
                pTexture
            }));
        return;
    }

    ProgramNode* pProgramNode = visitRenderTreeNode<ProgramNode>(&m_renderTree, pShaderProgram);
    TextureNode* pTextureNode = visitRenderTreeNode<TextureNode>(pProgramNode, pTexture);

    pTextureNode->push_back(pRenderable);
}

void AssetManager::_unregisterRenderable(Renderable* pRenderable)
{
    if (pRenderable->usesBlending())
    {
        m_blendedNodes.erase(pRenderable);
        return;
    }

    const std::string& shaderProgramId = pRenderable->getShaderProgramId();
    const std::string& imageTextureId = pRenderable->getImageTextureId();

    Program* pShaderProgram = m_shaderPrograms[shaderProgramId];
    Texture* pTexture = m_textures[imageTextureId];

    auto shaderNode = m_renderTree[pShaderProgram];
    auto textureNode = (*shaderNode)[pTexture];

    textureNode->erase(std::remove(textureNode->begin(), textureNode->end(), pRenderable));

    if (textureNode->empty())
    {
        shaderNode->erase(pTexture);

        if (shaderNode->empty())
            m_renderTree.erase(pShaderProgram);
    }
}

void AssetManager::registerTextureUniform(Program* pProgram, const std::string& shaderId, unsigned textureUniformId)
{
    std::string textureUniformName = "texture" + std::to_string(textureUniformId);
    GLuint textureUniformLocation = glGetUniformLocation(pProgram->getPid(), textureUniformName.c_str());

    if (textureUniformLocation == -1)
        std::cerr << "Unable to find uniform \"" << textureUniformName <<
            "\" in shader \"" << shaderId << "\"!" << std::endl;
    else
        glUniform1i(textureUniformLocation, textureUniformId);
}

template<typename T, typename U, typename V>
T* AssetManager::visitRenderTreeNode(U* pParentNode, V* value)
{
    T* pNode;

    auto nodeItr = pParentNode->find(value);

    if (nodeItr == pParentNode->end())
    {
        pNode = new T();
        pParentNode->insert(std::pair<V*, T*>(value, pNode));
    }
    else
    {
        pNode = nodeItr->second;
    }

    return pNode;
}
