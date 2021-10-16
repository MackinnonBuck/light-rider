#include "ComputeProgram.h"

#include <iostream>
#include <assert.h>

#include <glm/gtc/type_ptr.hpp>

#include "Program.h"
#include "GLSL.h"

ComputeProgram* ComputeProgram::create(const std::string& fileName)
{
    std::string source = readFileAsString(fileName);
    const char* sourceCString = source.c_str();
    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    CHECKED_GL_CALL(glShaderSource(shader, 1, &sourceCString, nullptr));

    GLint infolog;
    CHECKED_GL_CALL(glCompileShader(shader));
    CHECKED_GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &infolog));
    
    if (infolog == 0)
    {
        GLSL::printShaderInfoLog(shader);
        std::cout << "Error compiling compute shader " << std::endl;
        exit(1);
    }

    GLuint program = glCreateProgram();
    CHECKED_GL_CALL(glAttachShader(program, shader));
    CHECKED_GL_CALL(glLinkProgram(program));

    return new ComputeProgram(shader, program);
}

ComputeProgram::ComputeProgram(GLuint shader, GLuint program) :
    m_shader(shader),
    m_program(program),
    m_isBound(false),
    m_isVerbose(false)
{
}

ComputeProgram::~ComputeProgram()
{
    for (auto& pair : m_buffers)
    {
        CHECKED_GL_CALL(glDeleteBuffers(1, &pair.second));
    }

    CHECKED_GL_CALL(glDeleteShader(m_shader));
    CHECKED_GL_CALL(glDeleteProgram(m_program));
}

void ComputeProgram::bind()
{
    m_isBound = true;
    CHECKED_GL_CALL(glUseProgram(m_program));
}

void ComputeProgram::unbind()
{
    m_isBound = false;
    CHECKED_GL_CALL(glUseProgram(0));
}

void ComputeProgram::addBuffer(const std::string& bufferName, GLsizeiptr size)
{
    std::map<std::string, GLuint>::const_iterator it = m_buffers.find(bufferName);

    if (it != m_buffers.end())
    {
        return;
    }

    bool wasBound = m_isBound;

    if (!wasBound)
    {
        bind();
    }

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    m_buffers[bufferName] = buffer;

    if (!wasBound)
    {
        unbind();
    }
}

void ComputeProgram::addUniform(const std::string& uniformName)
{
    m_uniforms[uniformName] = GLSL::getUniformLocation(m_program, uniformName.c_str(), m_isVerbose);
}

GLuint ComputeProgram::getBuffer(const std::string& bufferName) const
{
    std::map<std::string, GLuint>::const_iterator it = m_buffers.find(bufferName);

    if (it == m_buffers.end())
    {
        if (m_isVerbose)
        {
            std::cout << "The buffer '" << bufferName << "' does not exist." << std::endl;
            return 0;
        }
    }

    return it->second;
}

GLuint ComputeProgram::getUniform(const std::string& uniformName) const
{
    std::map<std::string, GLuint>::const_iterator it = m_uniforms.find(uniformName);

    if (it == m_uniforms.end())
    {
        if (m_isVerbose)
        {
            std::cout << "The uniform '" << uniformName << "' does not exist." << std::endl;
            return 0;
        }
    }

    return it->second;
}

