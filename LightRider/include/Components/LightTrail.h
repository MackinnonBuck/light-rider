#pragma once

#include <vector>
#include <btBulletDynamicsCommon.h>

#include "Renderable.h"

// Updates and renders a light trail on the attached GameObject.
// Usage: addComponent(const glm::vec3& color)
class LightTrail : public Renderable
{
public:

    // Creates a new LightTrail instance.
    LightTrail(int playerId);

    // Destroys the LightTrail instance.
    virtual ~LightTrail();

    // Returns whether the light trail is enabled.
    bool getEnabled() const { return m_isEnabled; }

    // If set to true, the light trail is enabled and actively updated. Otherwise, the light trail will not be
    // updated, but will still be rendered.
    void setEnabled(bool enabled) { m_isEnabled = enabled; }

    // Initializes the light trail.
    virtual bool initialize();
    
    // Renders the light trail.
    virtual void render();

    // Updates the segments of the light trail.
    virtual void physicsTick(float physicsTimeStep);

    // Updates the trail visually to give the illusion of a continuous trail.
    virtual void postUpdate(float deltaTime);

private:

    // The ID of the player emitting the trail.
    int m_playerId;

    // The shader program used to render the trail.
    Program* m_pShaderProgram;

    // The rigid body of the light trail.
    btRigidBody* m_pRigidBody;

    // The compound shape containing each trail segment.
    btCompoundShape* m_pCompoundShape;

    // The next segment to add to the trail.
    btBvhTriangleMeshShape* m_pNextSegment;

    // If true, the first segment is ready to be created.
    bool m_isInitialized;

    // If true, the light trail is enabled and actively creating segments.
    bool m_isEnabled;

    // The amount of time the tail has existed. Used to seed the perlin noise algorithm
    // for the shader.
    float m_continuousTime;
    
    // The amount of time the tail has existed in physics time. Used to assign time stamps to
    // vertices in the trail.
    float m_physicsTime;

    // The top vertex of the leading edge of the trail.
    glm::vec3 m_lastVertex1;

    // The bottom vertex of the leading edge of the trail.
    glm::vec3 m_lastVertex2;

    // The game tick when the last segment was created.
    float m_lastTimeStamp;

    // The vertices in the trail.
    std::vector<glm::vec3> m_trailVertices;

    // The height values of the vertices in the trail.
    std::vector<float> m_trailHeights;

    // The time stamps of the vertices in the trail.
    std::vector<float> m_trailTimeStamps;

    // The vertex array object ID for the trail.
    GLuint m_trailVertexArray;

    // The vertex buffer object ID for the trail vertices.
    GLuint m_trailVertexBuffer;

    // The vertex buffer object ID for the trail heights.
    GLuint m_trailHeightBuffer;

    // The vertex buffer object ID for the trail vertex time stamps.
    GLuint m_trailTimeStampBuffer;

    // The trail vertex count.
    int m_trailVertexCount;
    
    // The trail buffer size.
    int m_trailBufferSize;

    // Calculates the positions of the latest trail edge.
    void calculateTrailEdge(glm::vec3& vertex1, glm::vec3& vertex2);

    // Generates segment vertex and height data from the given leading edge.
    void generateSegmentData(const glm::vec3& edgeVertex1, const glm::vec3& edgeVertex2,
        glm::vec3* pSegmentVertices, float* pSegmentHeights, float* pSegmentTimeStamps);

    // Writes the given segment data to the vertex and height buffers at the provided offset.
    void writeSegmentData(int offset, glm::vec3* pSegmentVertices, float* pSegmentHeights, float* pSegmentTimeStamps);

    // Copies the trail buffer to a new buffer of the provided size.
    void reallocateTrailBuffer(int size);
};
