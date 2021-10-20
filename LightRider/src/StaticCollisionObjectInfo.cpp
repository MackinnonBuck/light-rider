#include "StaticCollisionObjectInfo.h"

static StaticCollisionObjectInfo s_groundStaticInfo = StaticCollisionObjectInfo(StaticCollisionObjectType::GROUND);
static StaticCollisionObjectInfo s_particleStaticInfo = StaticCollisionObjectInfo(StaticCollisionObjectType::PARTICLE);
static StaticCollisionObjectInfo s_trailInfo = StaticCollisionObjectInfo(StaticCollisionObjectType::TRAIL);

CollisionObjectInfo StaticCollisionObjectInfo::s_groundInfo = CollisionObjectInfo(nullptr, &s_groundStaticInfo);
CollisionObjectInfo StaticCollisionObjectInfo::s_particleInfo = CollisionObjectInfo(nullptr, &s_particleStaticInfo);
CollisionObjectInfo StaticCollisionObjectInfo::s_trailInfo = CollisionObjectInfo(nullptr, &s_trailInfo);

StaticCollisionObjectType StaticCollisionObjectInfo::getType(void* pUserPointer)
{
    if (pUserPointer == nullptr)
    {
        return StaticCollisionObjectType::UNKNOWN;
    }

    CollisionObjectInfo* pInfo = static_cast<CollisionObjectInfo*>(pUserPointer);
    void* pUserData = pInfo->getUserData();

    if (pUserData == nullptr)
    {
        return StaticCollisionObjectType::UNKNOWN;
    }

    StaticCollisionObjectInfo* pStaticInfo = static_cast<StaticCollisionObjectInfo*>(pUserData);
    return pStaticInfo->getType();
}
