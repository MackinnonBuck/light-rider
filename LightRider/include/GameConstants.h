#pragma once

#include <btBulletDynamicsCommon.h>
#include <glm/gtc/type_ptr.hpp>

// Contains global game constants.
namespace GameConstants
{
    // World constants.
    constexpr float worldGravity = -15.0f;

    // Map constants.
    constexpr float mapHalfWidth = 200.0f;
    constexpr float mapHalfHeight = 5.0f;

    // Camera constants.
    constexpr float cameraTranslationStiffness = 16.0f;
    constexpr float cameraRotationStiffness = 12.0f;
    constexpr float cameraHeight = 3.0f;
    constexpr float cameraDistance = 10.0f;
    constexpr float cameraNudgeOffset = 0.05f;
    constexpr float cameraDeathAngle = -0.35f;
    constexpr float cameraDeathHeight = 5.0f;
    constexpr float cameraDeathDistance = 10.0f;

    // Intro sequence constants.
    constexpr int introLightTrailFrame = 60;
    constexpr int introCameraFocusFrame = 100;
    constexpr int introEndFrame = 130;
    constexpr glm::vec3 introCamera1Position = glm::vec3(-180.0f, 1.0f, -1.0f);
    constexpr glm::vec3 introCamera2Position = glm::vec3(180.0f, 1.0f, -1.0f);
    constexpr float introCameraSpeed = 20.0f;

    // Player constants.
    constexpr glm::vec3 player1Color = glm::vec3(0.0f, 0.75f, 1.0f);
    constexpr glm::vec3 player2Color = glm::vec3(1.0f, 0.25f, 0.0f);

    // Controls constants.
    constexpr float accelerationDeadzone = 0.1f;
    constexpr float turningDeadzone = 0.1f;
    constexpr int turnAxis = 0;
    constexpr int accelerateAxis = 5;
    constexpr int jumpButton = 0;
    constexpr int opponentViewButton = 1;

    // Bike constants.
    constexpr float bikeShapeVerticalOffset = 0.1f;
    constexpr float bikeMass = 200.0f;
    constexpr float bikeHalfWidth = 0.4f;
    constexpr float bikeHalfHeight = 0.5f;
    constexpr float bikeHalfLength = 2.0f;
    constexpr float bikeWheelRadius = 0.65f;
    constexpr float bikeWheelVerticalOffset = -0.15f;
    constexpr float bikeWheelBodyPadding = 0.1f;
    constexpr float bikeMaxSpeed = 50.0f;
    constexpr float bikeAccelerationForceFactor = 8.0f;
    constexpr float bikeDecelerationForceFactor = 2.0f;
    constexpr float bikeBalanceStability = 1.50f;
    constexpr float bikeBalanceSpeed = 7.5f;
    constexpr float bikeMaxLeanAngle = 0.5f;
    constexpr float bikeAerialCorrectionStability = 10.0f;
    constexpr float bikeAerialCorrectionSpeed = 12.5f;
    constexpr float bikeWheelTurningVelocity = 0.035f;
    constexpr float bikeWheelMaxTurningAngle = 0.25f;
    constexpr float bikeWheelForceFactor = 30.0f;
    constexpr float bikeWheelTorqueFactor = 225.0f;
    constexpr float bikeWheelTorqueSpeedEpsilon = 0.1f;
    constexpr float bikeDampingTorqueFactor = 50.0f;
    constexpr float bikeJumpImpulse = 2000.0f;
    constexpr float bikeJumpCooldown = 0.5f;
    constexpr float bikeMinTrailSegmentLength = 2.0f;
    constexpr float bikeTrailTopEdgeOffset = 0.55f;
    constexpr float bikeTrailBottomEdgeOffset = -0.625f;
    constexpr float bikeTrailEdgePenetration = 0.335f;
    constexpr float bikeDeathAngularDamping = 0.5f;
    constexpr float bikeDeathRestitution = 0.5f;
    constexpr float bikeDeathFriction = 0.6f;
}
