#pragma once

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <btBulletDynamicsCommon.h>

#include "Program.h"

class DebugDrawer : public btIDebugDraw
{
public:

    // Creates a new DebugDrawer instance.
    DebugDrawer();

    // Draws a line of the given color connecting the two provided points.
    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

    // Draws a contact point.
    virtual void drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);

    // Reports an error (unimplemented).
    virtual void reportErrorWarning(const char* warningString) { }

    // Draws 3D text (unimplemented).
    virtual void draw3dText(const btVector3& location, const char* textString) { }

    // Sets the debug draw mode.
    virtual void setDebugMode(int debugMode) { m_debugMode = debugMode; }

    // Returns the debug draw mode.
    virtual int getDebugMode() const { return m_debugMode; };

    // Draws all information to the screen.
    virtual void flushLines();

private:

    // The debug draw mode.
    int m_debugMode;

    // A list of all line vertices.
    std::vector<glm::vec3> m_lineVertices;

    // A list of all line colors.
    std::vector<glm::vec3> m_lineColors;

    // A list of all point vertices.
    std::vector<glm::vec3> m_pointVertices;

    // A list of all point colors.
    std::vector<glm::vec3> m_pointColors;

    // The maximum buffer size of the line vertex list.
    int m_lineVertexBufferSize;

    // The maximum buffer size of the line color list.
    int m_lineColorBufferSize;

    // The maximum buffer size of the point vertex list.
    int m_pointVertexBufferSize;

    // The maximum buffer size of the point color list.
    int m_pointColorBufferSize;

    // The line VAO identifier.
    GLuint m_lineVertexArray;

    // The line vertex VBO identifier.
    GLuint m_lineVertexBuffer;

    // The line color VBO identifier.
    GLuint m_lineColorBuffer;

    // The point VAO identifier.
    GLuint m_pointVertexArray;

    // The point vertex VBO identifier.
    GLuint m_pointVertexBuffer;

    // The point color VBO identifier.
    GLuint m_pointColorBuffer;

    // The shader program used to draw debug information.
    Program* m_pLineProgram;
};

