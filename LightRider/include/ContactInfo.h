#pragma once

#include <btBulletDynamicsCommon.h>

// Wraps a Bullet Physics manifold point with fields distinguishing between the associated body and the other body.
struct ContactInfo
{
    // The wrapped manifold point.
    btManifoldPoint& manifoldPoint;

    // The position in world coordinates on the assiciated body.
    btVector3& positionWorldOnSelf;

    // The position in world coordinates on the other body.
    btVector3& positionWorldOnOther;

    // The local contact point in world coordinates relative to the associated body.
    btVector3& localPointSelf;

    // The local contact point in world coordinates relative to the other body.
    btVector3& localPointOther;

    // True if this contact info is from the perspective of "body A".
    bool isBodyA;

    // Creates a new ContactInfo instance.
    ContactInfo(btManifoldPoint& manifoldPoint, bool isBodyA) :
        isBodyA(isBodyA),
        manifoldPoint(manifoldPoint),
        positionWorldOnSelf(isBodyA ? manifoldPoint.m_positionWorldOnA : manifoldPoint.m_positionWorldOnB),
        positionWorldOnOther(isBodyA ? manifoldPoint.m_positionWorldOnB : manifoldPoint.m_positionWorldOnA),
        localPointSelf(isBodyA ? manifoldPoint.m_localPointA : manifoldPoint.m_localPointB),
        localPointOther(isBodyA ? manifoldPoint.m_localPointB : manifoldPoint.m_localPointA) { }
};
