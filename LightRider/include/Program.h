
#pragma  once

#ifndef LAB471_PROGRAM_H_INCLUDED
#define LAB471_PROGRAM_H_INCLUDED

#include <map>
#include <string>
#include <vector>

#include <glad/glad.h>


std::string readFileAsString(const std::string &fileName);

class Program
{

public:

    void setVerbose(const bool v) { verbose = v; }
    bool isVerbose() const { return verbose; }

    void setShaderNames(const std::string &v, const std::string &f);
    void setShaderNames(const std::string &v, const std::vector<std::string> &f);
    virtual bool init();
    virtual void bind();
    virtual void unbind();

    void addAttribute(const std::string &name);
    void addUniform(const std::string &name);
    GLint getAttribute(const std::string &name) const;
    GLint getUniform(const std::string &name) const;
    GLuint getPid() const;

    void* getUserPointer() const { return userPointer; }
    void setUserPointer(void* userPointer) { this->userPointer = userPointer; }

protected:

    std::string vShaderName;
    std::vector<std::string> fShaderNames;

private:

    GLuint pid = 0;
    std::map<std::string, GLint> attributes;
    std::map<std::string, GLint> uniforms;
    bool verbose = true;
    void* userPointer = nullptr;
};

#endif // LAB471_PROGRAM_H_INCLUDED
