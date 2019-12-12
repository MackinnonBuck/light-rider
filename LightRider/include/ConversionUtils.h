#pragma once

#include <btBulletDynamicsCommon.h>
#include <glm/gtc/type_ptr.hpp>

// Converts a Bullet vector3 to a GLM vector3.
glm::vec3 toGlm(const btVector3& v);

// Converts a Bullet quaternion to a GLM quaternion.
glm::quat toGlm(const btQuaternion& q);

// Converts a GLM vector3 to a Bullet vector3.
btVector3 toBullet(const glm::vec3& v);

// Converts a GLM quaternion to a Bullet quaternion.
btQuaternion toBullet(const glm::quat& q);
