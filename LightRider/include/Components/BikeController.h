#pragma once

#include "Component.h"
#include "DebugDrawer.h"
#include "ContactHandler.h"
#include "Components/RigidBodyComponent.h"
#include "Game.h"
#include "Joystick.h"

// Represents the control mode of a bike.
enum class BikeControlMode
{
    LOCKED,
    AUTO_ACCELERATE,
    MANUAL_ACCELERATE
};

// Represents the possible actions the player can peform on a bike.
enum class BikeActions
{
    ACCELERATE,
    JUMP,
    TURN
};

// Holds joystick info and key codes which describe which keys correspond with certain parts of bike functionality.
struct BikeControls
{
    // The ID of the joystick to be used.
    int joystickId;

    // The accelerate key code (fallback if no joystick found).
    int accelerate;

    // The jump key code (fallback if no joystick found).
    int jump;

    // The turn left key code (fallback if no joystick found).
    int turnLeft;

    // The turn right key code (fallback if no joystick found).
    int turnRight;
};

// Controls bike logic on the parent GameObject. If there is no RigidBodyComponent attached to the parent GameObject
// when a BikeController is added, the BikeController will fail to initialize, and won't be added to the GameObject.
// Usage: addComponent()
class BikeController : public Component, public ContactHandler
{
public:

    // Creates a new BikeController instance.
    BikeController(const BikeControls& bikeControls) :
        m_bikeControls(bikeControls),
        m_controlMode(BikeControlMode::LOCKED),
        m_pJoystick(Game::getInstance().getJoystick(bikeControls.joystickId)),
        m_pRigidBodyComponent(nullptr),
        m_localFrontWheelContactPoint(0.0f, 0.0f, 0.0f),
        m_localRearWheelContactPoint(0.0f, 0.0f, 0.0f),
        m_frontWheelGroundNormal(0.0f, 0.0f, 0.0f),
        m_rearWheelGroundNormal(0.0f, 0.0f, 0.0f),
        m_frontWheelAngle(0.0f),
        m_jumpTimer(0.0f) { }

    // Returns the control mode for this bike controller.
    BikeControlMode getControlMode() const { return m_controlMode; }

    // Sets the control mode for this bike controller.
    void setControlMode(BikeControlMode controlMode) { m_controlMode = controlMode; }

    // Destroys the BikeController instance.
    virtual ~BikeController();

    // Draws physics data in the scene.
    void debugDrawPhysicsData(btIDebugDraw* pDebugDrawer, const btTransform& transform, const btVector3& localForward, const btVector3& frontWheelLeft,
        const btVector3& rearWheelLeft, const btVector3& linearVelocity, const btVector3& frontImpulse, const btVector3& rearImpulse);

    // Initializes this BikeController instsance, returning true if initialization was successful.
    virtual bool initialize();

    // Called just before the physics simulation is advanced.
    virtual void prePhysicsTick(float physicsTimeStep);

    // Handles bike collisions.
    virtual void handleContact(const ContactInfo& contactInfo, btCollisionObject* pBodySelf, btCollisionObject* pBodyOther);

private:

    // Specifies the controls for the bike.
    BikeControls m_bikeControls;

    // The control mode of this bike controller.
    BikeControlMode m_controlMode;

    // The joystick controlling this bike controller.
    Joystick* m_pJoystick; 

    // The sibling RigidBodyComponent.
    RigidBodyComponent* m_pRigidBodyComponent;

    // The best-determined front wheel contact point.
    glm::vec3 m_localFrontWheelContactPoint;

    // The best-determined rear wheel contact point.
    glm::vec3 m_localRearWheelContactPoint;

    // The best-determined front wheel normal with the ground.
    glm::vec3 m_frontWheelGroundNormal;

    // The best-determined rear wheel normal with the ground.
    glm::vec3 m_rearWheelGroundNormal;

    // The turning angle of the front wheel.
    float m_frontWheelAngle;

    // The time remaining until a jump is available.
    float m_jumpTimer;

    // Updates driving physics for the bike.
    void updateDrivingPhysics();

    // Updates aerial physics for the bike.
    void updateAerialPhysics();

    // Gets the input for a specific bike action.
    float getActionInput(BikeActions bikeAction);

    // Gets the input for a specific bike action from the keyboard.
    float getKeyboardActionInput(BikeActions bikeAction);

    // Gets the input for a specific bike action from the joystick.
    float getJoystickActionInput(BikeActions bikeAction);

    // "Kills" the bike, destroying this component and modifying the rigid body properties to reflect
    // a crashed bike.
    void kill();
};
