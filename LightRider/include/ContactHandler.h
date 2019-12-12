#pragma once

#include <btBulletDynamicsCommon.h>

#include "ContactInfo.h"

// An interface for types that contain contact handlers for RigidBodyComponents.
class ContactHandler
{
public:

    // Called when a contact occurs between this body and another body.
    virtual void handleContact(const ContactInfo& contactInfo, btCollisionObject* pBodySelf, btCollisionObject* pBodyOther) = 0;
};
