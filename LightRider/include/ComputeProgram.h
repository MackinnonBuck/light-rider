#pragma once

#include <map>
#include <string>
#include <glad/glad.h>

class ComputeProgram
{
public:
    static ComputeProgram* create(const std::string& fileName);

    virtual ~ComputeProgram();

    void bind();
    void unbind();

    void addBuffer(const std::string& bufferName, GLsizeiptr size);
    void addUniform(const std::string& uniformName);

    GLuint getBuffer(const std::string& bufferName) const;
    GLuint getUniform(const std::string& uniformName) const;

    bool isVerbose() const { return m_isVerbose; }
    void setVerbose(bool isVerbose) { m_isVerbose = isVerbose; }

private:
    ComputeProgram(GLuint shader, GLuint program);

    GLuint m_shader;
    GLuint m_program;

    std::map<std::string, GLuint> m_buffers;
    std::map<std::string, GLuint> m_uniforms;
    
    bool m_isBound;
    bool m_isVerbose;
};

