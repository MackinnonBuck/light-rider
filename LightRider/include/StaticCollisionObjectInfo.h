#pragma once

#include "CollisionObjectInfo.h"

enum class StaticCollisionObjectType
{
    GROUND,
    RAMP,
    PARTICLE,
};

class StaticCollisionObjectInfo
{
public:
    StaticCollisionObjectInfo(StaticCollisionObjectType type) :
        m_type(type)
    {
    }

    StaticCollisionObjectType getType() const { return m_type; }

    static CollisionObjectInfo& getGroundInfo() { return s_groundInfo; }
    static CollisionObjectInfo& getRampInfo() { return s_rampInfo; }
    static CollisionObjectInfo& getParticleInfo() { return s_particleInfo; }

private:
    static CollisionObjectInfo s_groundInfo;
    static CollisionObjectInfo s_rampInfo;
    static CollisionObjectInfo s_particleInfo;

    StaticCollisionObjectType m_type;
};

