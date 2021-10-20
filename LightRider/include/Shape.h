


#pragma once

#ifndef LAB471_SHAPE_H_INCLUDED
#define LAB471_SHAPE_H_INCLUDED

#include <string>
#include <vector>
#include <unordered_set>
#include <memory>
#include <iostream>
#include <assert.h>

#include <glad/glad.h>


class Program;

class Shape
{

public:
    //stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp)
    void loadMesh(const std::string &meshName, std::string *mtlName = NULL, unsigned char *(loadimage)(char const *, int *, int *, int *, int) = NULL);
    void init();
    void resize();
    void draw(const Program* prog) const;
    void drawDepth(const Program* prog) const;
    bool usesInstancing() const { return instanceBufID != nullptr; }
    const std::vector<float>& getVertices(int shapeId) const { return posBuf[shapeId]; }
    const std::vector<unsigned int>& getIndices(int shapeId) const { return eleBuf[shapeId]; }

    template<typename T>
    void generateInstanceData(int length, GLsizeiptr size, GLenum type, const T* data)
    {
        assert(instanceBufID == nullptr);

        instanceBufID = new unsigned int[obj_count];
        instanceCount = length;
        instanceElementSize = sizeof(T);
        instanceElementType = type;
        instanceElementComponentCount = size;

        for (int i = 0; i < obj_count; i++)
        {
            glBindVertexArray(vaoID[i]);

            glGenBuffers(1, &instanceBufID[i]);
            glBindBuffer(GL_ARRAY_BUFFER, instanceBufID[i]);
            glBufferData(GL_ARRAY_BUFFER, length * sizeof(T), data, GL_STATIC_DRAW);

            int error = glGetError();

            if (error)
            {
                std::cerr << "Error when generating instance data: " << error << std::endl;
            }

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
    }

    unsigned int *textureIDs = NULL;

private:
    int obj_count = 0;
    std::vector<unsigned int> *eleBuf = NULL;
    std::vector<float> *posBuf = NULL;
    std::vector<float> *norBuf = NULL;
    std::vector<float> *texBuf = NULL;
    void** instanceBuf = NULL;
    unsigned int *materialIDs = NULL;
    GLsizeiptr instanceCount = 0;
    GLsizeiptr instanceElementComponentCount = 0;
    GLsizeiptr instanceElementSize = 0;
    GLenum instanceElementType = 0;
    mutable std::unordered_set<GLuint> instancedPrograms;


    unsigned int *eleBufID = 0;
    unsigned int *posBufID = 0;
    unsigned int *norBufID = 0;
    unsigned int *texBufID = 0;
    unsigned int *instanceBufID = 0;
    unsigned int *vaoID = 0;

    void drawInstanced(int shapeId, const Program* prog) const;
};

#endif // LAB471_SHAPE_H_INCLUDED
