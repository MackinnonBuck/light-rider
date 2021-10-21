#include "DebugDrawer.h"

#include <iostream>
#include <algorithm>

#include "Game.h"
#include "Scene.h"
#include "GameConfig.h"
#include "ConversionUtils.h"

constexpr float contactPointSize = 10.0f;

DebugDrawer::DebugDrawer() :
    m_debugMode(0),
    m_lineVertexBufferSize(0),
    m_lineColorBufferSize(0),
    m_pointVertexBufferSize(0),
    m_pointColorBufferSize(0)
{
    glGenVertexArrays(1, &m_lineVertexArray);
    glBindVertexArray(m_lineVertexArray);

    glGenBuffers(1, &m_lineVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_lineVertexBuffer);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glGenBuffers(1, &m_lineColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_lineColorBuffer);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glGenVertexArrays(1, &m_pointVertexArray);
    glBindVertexArray(m_pointVertexArray);

    glGenBuffers(1, &m_pointVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_pointVertexBuffer);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glGenBuffers(1, &m_pointColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_pointColorBuffer);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);

    GameConfig& config = Game::getInstance().getConfig();

    m_pLineProgram = new Program();
    m_pLineProgram->setVerbose(true);
    m_pLineProgram->setShaderNames(config.resourceDirectory + "line_vertex.glsl", config.resourceDirectory + "line_fragment.glsl");

    if (!m_pLineProgram->init())
    {
        std::cerr << "Debug drawer line shader could not compile!" << std::endl;
        return;
    }

    m_pLineProgram->addUniform("P");
    m_pLineProgram->addUniform("V");
    m_pLineProgram->addAttribute("vertexPosition");
    m_pLineProgram->addAttribute("vertexColor");
}

void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
    m_lineVertices.push_back(toGlm(from));
    m_lineVertices.push_back(toGlm(to));

    glm::vec3 glmColor = toGlm(color);

    m_lineColors.push_back(glmColor);
    m_lineColors.push_back(glmColor);
}

void DebugDrawer::drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB,
    btScalar distance, int lifeTime, const btVector3& color)
{
    if (distance <= 0.0f)
    {
        m_pointVertices.push_back(toGlm(pointOnB));
        m_pointColors.push_back(toGlm(color));
    }
}

void DebugDrawer::flushLines()
{
    Camera* pCamera = Game::getInstance().getScene()->getActiveCamera();

    const glm::mat4& P = pCamera->getPerspectiveMatrix();
    const glm::mat4& V = pCamera->getViewMatrix();

    int lineVertexCount = (int)m_lineVertices.size();
    int lineColorCount = (int)m_lineColors.size();

    int pointVertexCount = (int)m_pointVertices.size();
    int pointColorCount = (int)m_pointColors.size();

    m_lineVertexBufferSize = std::max(m_lineVertexBufferSize, lineVertexCount);
    m_lineColorBufferSize = std::max(m_lineColorBufferSize, lineColorCount);

    m_pointVertexBufferSize = std::max(m_pointVertexBufferSize, pointVertexCount);
    m_pointColorBufferSize = std::max(m_pointColorBufferSize, pointColorCount);

    m_pLineProgram->bind();

    glUniformMatrix4fv(m_pLineProgram->getUniform("P"), 1, GL_FALSE, &P[0][0]);
    glUniformMatrix4fv(m_pLineProgram->getUniform("V"), 1, GL_FALSE, &V[0][0]);

    glBindVertexArray(m_lineVertexArray);

    glBindBuffer(GL_ARRAY_BUFFER, m_lineVertexBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_lineVertexBufferSize, NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * lineVertexCount, m_lineVertices.data());

    glBindBuffer(GL_ARRAY_BUFFER, m_lineColorBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_lineColorBufferSize, NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * lineColorCount, m_lineColors.data());

    glDrawArrays(GL_LINES, 0, lineVertexCount);

    glBindVertexArray(m_pointVertexArray);

    glBindBuffer(GL_ARRAY_BUFFER, m_pointVertexBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_pointVertexBufferSize, NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * pointVertexCount, m_pointVertices.data());

    glBindBuffer(GL_ARRAY_BUFFER, m_pointColorBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_pointColorBufferSize, NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * pointColorCount, m_pointColors.data());

    glPointSize(contactPointSize);
    glDrawArrays(GL_POINTS, 0, pointVertexCount);

    glBindVertexArray(0);

    m_pLineProgram->unbind();

    m_lineVertices.clear();
    m_lineColors.clear();

    m_pointVertices.clear();
    m_pointColors.clear();
}
