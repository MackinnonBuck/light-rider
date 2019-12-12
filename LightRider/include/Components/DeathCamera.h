#pragma once

#include "GameObject.h"
#include "Components/ProcessedCamera.h"

// A camera that focuses on the "dead" player.
// Usage: addComponent(GameObject* pDeadObject, bool enabled = true, float layerDepth = 0.0f)
class DeathCamera : public ProcessedCamera
{
public:

    // Creates a new DeathCamera instance.
    DeathCamera(bool enabled = false, float layerDepth = 0.0f) :
        ProcessedCamera(enabled, layerDepth),
        m_pDeadObject(nullptr) { }

    // Sets the "dead" object that the camera should track.
    void setDeadObject(GameObject* pDeadObject);

    // Initializes the death camera.
    virtual bool initialize();

    // Updates the position of the death camera.
    virtual void postUpdate(float deltaTime);

private:

    // The "dead" GameObject to track.
    GameObject* m_pDeadObject;

    // Updates the position of the death camera.
    inline void updatePosition();
};
