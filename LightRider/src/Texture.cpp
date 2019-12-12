#include "Texture.h"

#include "stb_image.h"

void Texture::init(std::string fileName, TextureType textureType)
{
    m_textureType = textureType;
    
    int width, height, channels;
    unsigned char* pData = stbi_load(fileName.c_str(), &width, &height, &channels, 4);

    glGenTextures(1, &m_textureId);
    glActiveTexture((GLenum)textureType);
    glBindTexture(GL_TEXTURE_2D, m_textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);
    glGenerateMipmap(GL_TEXTURE_2D);
}
