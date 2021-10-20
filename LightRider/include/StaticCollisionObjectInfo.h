#pragma once

#include "CollisionObjectInfo.h"

enum class StaticCollisionObjectType
{
    UNKNOWN,
    GROUND,
    PARTICLE,
    TRAIL,
};

class StaticCollisionObjectInfo
{
public:
    StaticCollisionObjectInfo(StaticCollisionObjectType type) :
        m_type(type)
    {
    }

    StaticCollisionObjectType getType() const { return m_type; }

    static StaticCollisionObjectType getType(void* pUserPointer);
    static CollisionObjectInfo& getGroundInfo() { return s_groundInfo; }
    static CollisionObjectInfo& getParticleInfo() { return s_particleInfo; }
    static CollisionObjectInfo& getTrailInfo() { return s_trailInfo; }

private:
    static CollisionObjectInfo s_groundInfo;
    static CollisionObjectInfo s_particleInfo;
    static CollisionObjectInfo s_trailInfo;

    StaticCollisionObjectType m_type;
};

