#pragma once

#include "ContactHandler.h"

class CollisionObjectInfo
{
public:
    // Constructs a new CollisionObjectInfo instance.
    CollisionObjectInfo(ContactHandler* pContactHandler, void* pUserData) :
        m_pContactHandler(pContactHandler),
        m_pUserData(pUserData)
    {
    }

    // Constructs a new CollisionObjectInfo instance.
    CollisionObjectInfo() : CollisionObjectInfo(nullptr, nullptr)
    {
    }

    // Gets the optional contact handler.
    ContactHandler* getContactHandler() const { return m_pContactHandler; }

    // Sets the optional contact handler.
    void setContactHandler(ContactHandler* pContactHandler) { m_pContactHandler = pContactHandler; }

    // Gets the additional user data attached to the collision object.
    void* getUserData() const { return m_pUserData; }

    // Sets the additional user data attached to the collision object.
    void setUserData(void* pUserData) { m_pUserData = pUserData; }

private:
    // The optional contact handler.
    ContactHandler* m_pContactHandler;

    // Additional user data attached to the collision object.
    void* m_pUserData;
};

