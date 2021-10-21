#include "Components/LightTrail.h"

#include "Game.h"
#include "GameConstants.h"
#include "ConversionUtils.h"
#include "StaticCollisionObjectInfo.h"

namespace GC = GameConstants;

LightTrail::LightTrail(int playerId) :
    Renderable("trailShader", "", true),
    m_playerId(playerId),
    m_pShaderProgram(Game::getInstance().getScene()->getAssetManager()->getShaderProgram(getShaderProgramId())),
    m_pRigidBody(nullptr),
    m_pCompoundShape(nullptr),
    m_pNextSegment(nullptr),
    m_isInitialized(false),
    m_isEnabled(true),
    m_continuousTime(0.0f),
    m_physicsTime(0.0f),
    m_lastVertex1(0.0f, 0.0f, 0.0f),
    m_lastVertex2(0.0f, 0.0f, 0.0f),
    m_lastTimeStamp(0.0f),
    m_trailVertices(),
    m_trailHeights(),
    m_trailTimeStamps(),
    m_trailVertexCount(6),
    m_trailBufferSize(1024 * 8)
{
    glGenVertexArrays(1, &m_trailVertexArray);
    glBindVertexArray(m_trailVertexArray);

    glGenBuffers(1, &m_trailVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_trailVertexBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_trailBufferSize, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glGenBuffers(1, &m_trailHeightBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_trailHeightBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_trailBufferSize, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glGenBuffers(1, &m_trailTimeStampBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_trailTimeStampBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_trailBufferSize, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);
}

LightTrail::~LightTrail()
{
    glDeleteVertexArrays(1, &m_trailVertexArray);
    glDeleteBuffers(1, &m_trailVertexBuffer);
    glDeleteBuffers(1, &m_trailHeightBuffer);
    glDeleteBuffers(1, &m_trailTimeStampBuffer);

    Game::getInstance().getScene()->getDynamicsWorld()->removeCollisionObject(m_pRigidBody);

    delete m_pRigidBody->getMotionState();

    while (m_pCompoundShape->getNumChildShapes() > 0)
        m_pCompoundShape->removeChildShapeByIndex(0);

    delete m_pCompoundShape;

    delete m_pRigidBody;
}

bool LightTrail::initialize()
{
    if (!Renderable::initialize())
        return false;

    m_pCompoundShape = new btCompoundShape();

    btTransform shapeTransform;
    shapeTransform.setFromOpenGLMatrix(&getGameObject()->getTransform()->getTransformMatrix()[0][0]);

    btTransform transform;
    transform.setIdentity();

    btDefaultMotionState* pMotionState = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, pMotionState, m_pCompoundShape);

    m_pRigidBody = new btRigidBody(rbInfo);
    m_pRigidBody->setRestitution(1.0f);
    m_pRigidBody->setCollisionFlags(m_pRigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
    m_pRigidBody->setUserPointer(&StaticCollisionObjectInfo::getTrailInfo());

    Game::getInstance().getScene()->getDynamicsWorld()->addRigidBody(m_pRigidBody);

    return true;
}

void LightTrail::physicsTick(float physicsTimeStep)
{
    m_physicsTime += physicsTimeStep;

    if (!m_isEnabled)
        return;

    glm::vec3 vertex1;
    glm::vec3 vertex2;

    calculateTrailEdge(vertex1, vertex2);

    if (!m_isInitialized)
    {
        m_isInitialized = true;
        m_lastVertex1 = vertex1;
        m_lastVertex2 = vertex2;
        m_lastTimeStamp = m_physicsTime;

        return;
    }

    if (glm::length(vertex1 - m_lastVertex1) < GC::bikeMinTrailSegmentLength ||
        glm::length(vertex2 - m_lastVertex2) < GC::bikeMinTrailSegmentLength)
        return;

    glm::vec3 segmentVertices[6];
    float segmentHeights[6];
    float segmentTimeStamps[6];

    generateSegmentData(vertex1, vertex2, segmentVertices, segmentHeights, segmentTimeStamps);

    int trailVertexOffset = m_trailVertexCount - 6;
    m_trailVertexCount += 6;

    m_trailVertices.resize(m_trailVertexCount);
    m_trailHeights.resize(m_trailVertexCount);
    m_trailTimeStamps.resize(m_trailVertexCount);

    m_trailVertices.insert(m_trailVertices.begin() + trailVertexOffset, segmentVertices, segmentVertices + 6);
    m_trailHeights.insert(m_trailHeights.begin() + trailVertexOffset, segmentHeights, segmentHeights + 6);
    m_trailTimeStamps.insert(m_trailTimeStamps.begin() + trailVertexOffset, segmentTimeStamps, segmentTimeStamps + 6);

    glBindVertexArray(m_trailVertexArray);

    if (m_trailVertexCount > m_trailBufferSize)
        reallocateTrailBuffer(m_trailBufferSize * 2);
    else
        writeSegmentData(trailVertexOffset, segmentVertices, segmentHeights, segmentTimeStamps);

    glBindVertexArray(0);

    if (m_pNextSegment)
        m_pCompoundShape->addChildShape(btTransform::getIdentity(), m_pNextSegment);

    btTriangleMesh* pTriangleMesh = new btTriangleMesh();
    pTriangleMesh->preallocateVertices(6);
    pTriangleMesh->addTriangle(toBullet(m_lastVertex1), toBullet(vertex2), toBullet(vertex1));
    pTriangleMesh->addTriangle(toBullet(m_lastVertex1), toBullet(m_lastVertex2), toBullet(vertex2));

    m_pNextSegment = new btBvhTriangleMeshShape(pTriangleMesh, false);

    m_lastVertex1 = vertex1;
    m_lastVertex2 = vertex2;
    m_lastTimeStamp = m_physicsTime;

    Renderable::physicsTick(physicsTimeStep);
}

void LightTrail::postUpdate(float deltaTime)
{
    m_continuousTime += deltaTime;

    if (!m_isEnabled)
        return;

    glBindVertexArray(m_trailVertexArray);

    glm::vec3 vertex1;
    glm::vec3 vertex2;
    float timeStamp = (float)Game::getInstance().getTick();

    calculateTrailEdge(vertex1, vertex2);

    glm::vec3 segmentVertices[6];
    float segmentHeights[6];
    float segmentTimeStamps[6];

    generateSegmentData(vertex1, vertex2, segmentVertices, segmentHeights, segmentTimeStamps);
    writeSegmentData(m_trailVertexCount - 6, segmentVertices, segmentHeights, segmentTimeStamps);

    glBindVertexArray(0);

    Renderable::postUpdate(deltaTime);
}

void LightTrail::render()
{
    if (!m_isInitialized)
        return;
    
    glUniform1i(m_pShaderProgram->getUniform("playerId"), m_playerId);
    glUniform1f(m_pShaderProgram->getUniform("noiseSeed"), m_continuousTime);
    glUniform1f(m_pShaderProgram->getUniform("currentTime"), m_physicsTime);

    glBindVertexArray(m_trailVertexArray);

    glBlendFunci(0, GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);//glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, m_trailVertexCount);
    glDepthMask(GL_TRUE);

    glBindVertexArray(0);
}

void LightTrail::calculateTrailEdge(glm::vec3& vertex1, glm::vec3& vertex2)
{
    glm::mat4 transformMatrix = getGameObject()->getTransform()->getTransformMatrix();
    glm::mat4 vertex1Transform = transformMatrix * glm::translate(glm::mat4(1.0f),
        glm::vec3(0.0f, GC::bikeTrailTopEdgeOffset, GC::bikeHalfLength - GC::bikeTrailEdgePenetration));
    glm::mat4 vertex2Transform = transformMatrix * glm::translate(glm::mat4(1.0f),
        glm::vec3(0.0f, GC::bikeTrailBottomEdgeOffset, GC::bikeHalfLength - GC::bikeTrailEdgePenetration));

    vertex1 = glm::vec3(vertex1Transform[3]);
    vertex2 = glm::vec3(vertex2Transform[3]);
}

void LightTrail::generateSegmentData(const glm::vec3& edgeVertex1, const glm::vec3& edgeVertex2,
    glm::vec3* pSegmentVertices, float* pSegmentHeights, float* pSegmentTimeStamps)
{
    pSegmentVertices[0] = m_lastVertex1;
    pSegmentVertices[1] = edgeVertex2;
    pSegmentVertices[2] = edgeVertex1;
    pSegmentVertices[3] = m_lastVertex1;
    pSegmentVertices[4] = m_lastVertex2;
    pSegmentVertices[5] = edgeVertex2;

    pSegmentHeights[0] = 1;
    pSegmentHeights[1] = -1;
    pSegmentHeights[2] = 1;
    pSegmentHeights[3] = 1;
    pSegmentHeights[4] = -1;
    pSegmentHeights[5] = -1;

    pSegmentTimeStamps[0] = m_lastTimeStamp;
    pSegmentTimeStamps[1] = m_physicsTime;
    pSegmentTimeStamps[2] = m_physicsTime;
    pSegmentTimeStamps[3] = m_lastTimeStamp;
    pSegmentTimeStamps[4] = m_lastTimeStamp;
    pSegmentTimeStamps[5] = m_physicsTime;
}

void LightTrail::writeSegmentData(int offset, glm::vec3* pSegmentVertices, float* pSegmentHeights, float* pSegmentTimeStamps)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_trailVertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * offset, sizeof(glm::vec3) * 6, pSegmentVertices);

    glBindBuffer(GL_ARRAY_BUFFER, m_trailHeightBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * offset, sizeof(float) * 6, pSegmentHeights);

    glBindBuffer(GL_ARRAY_BUFFER, m_trailTimeStampBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * offset, sizeof(float) * 6, pSegmentTimeStamps);
}

void LightTrail::reallocateTrailBuffer(int size)
{
    m_trailBufferSize = size;

    glBindBuffer(GL_ARRAY_BUFFER, m_trailVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_trailBufferSize, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * m_trailVertexCount, m_trailVertices.data());

    glBindBuffer(GL_ARRAY_BUFFER, m_trailHeightBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_trailBufferSize, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * m_trailVertexCount, m_trailHeights.data());

    glBindBuffer(GL_ARRAY_BUFFER, m_trailTimeStampBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_trailBufferSize, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * m_trailVertexCount, m_trailTimeStamps.data());
}
