#pragma once

#include <string>
#include <glad/glad.h>

enum class TextureType
{
    NONE = 0,
    IMAGE = GL_TEXTURE0,
    SPECULAR = GL_TEXTURE1,
    NORMAL = GL_TEXTURE2,
    BACKGROUND = GL_TEXTURE3
};

// Encapsulates an OpenGL texture.
class Texture
{
public:

    // Creates a new Texture instance.
    Texture() : m_textureId(0), m_textureType(TextureType::NONE), m_width(0), m_height(0) { }

    // Initializes the texture from the give file name and sepcified texture type.
    void init(std::string fileName, TextureType textureType);

    // Returns the ID of this texture.
    GLuint getTextureId() const { return m_textureId; }

    // Returns the type of this texture.
    TextureType getTextureType() const { return m_textureType; }

    int getWidth() const { return m_width; }

    int getHeight() const { return m_height; }

private:

    // The ID of the texture.
    GLuint m_textureId;

    // The type of the texture.
    TextureType m_textureType;

    int m_width;

    int m_height;
};
