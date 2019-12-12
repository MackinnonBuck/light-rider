#include "ConversionUtils.h"

glm::vec3 toGlm(const btVector3& v)
{
    return glm::vec3(v.x(), v.y(), v.z());
}

glm::quat toGlm(const btQuaternion& q)
{
    return glm::quat(q.x(), q.y(), q.z(), q.w());
}

btVector3 toBullet(const glm::vec3& v)
{
    return btVector3(v.x, v.y, v.z);
}

btQuaternion toBullet(const glm::quat& q)
{
    return btQuaternion(q.x, q.y, q.z, q.w);
}
