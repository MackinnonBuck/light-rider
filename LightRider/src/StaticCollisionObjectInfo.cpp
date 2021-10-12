#include "StaticCollisionObjectInfo.h"

static StaticCollisionObjectInfo s_groundStaticInfo = StaticCollisionObjectInfo(StaticCollisionObjectType::GROUND);
static StaticCollisionObjectInfo s_rampStaticInfo = StaticCollisionObjectInfo(StaticCollisionObjectType::RAMP);
static StaticCollisionObjectInfo s_particleStaticInfo = StaticCollisionObjectInfo(StaticCollisionObjectType::PARTICLE);

CollisionObjectInfo StaticCollisionObjectInfo::s_groundInfo = CollisionObjectInfo(nullptr, &s_groundStaticInfo);
CollisionObjectInfo StaticCollisionObjectInfo::s_rampInfo = CollisionObjectInfo(nullptr, &s_rampStaticInfo);
CollisionObjectInfo StaticCollisionObjectInfo::s_particleInfo = CollisionObjectInfo(nullptr, &s_particleStaticInfo);
