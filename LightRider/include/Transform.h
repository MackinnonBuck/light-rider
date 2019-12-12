#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// An interface that outlines the basic functionality for a transform.
class Transform
{
public:

    // Returns a reference to the transform matrix representing this transform.
    virtual glm::mat4& getTransformMatrix() = 0;

    // Returns the position of this transform.
    virtual glm::vec3 getPosition() const = 0;

    // Returns the rotation of this transform.
    virtual glm::quat getRotation() const = 0;

    // Sets this transform's matrix directly.
    virtual void setTransformMatrix(glm::mat4 matrix) = 0;

    // Sets the postion of this transform.
    virtual void setPosition(glm::vec3 position) = 0;

    // Sets the rotation of this transform.
    virtual void setRotation(glm::quat rotation) = 0;

    // Multiplies this transform's matrix with the given matrix.
    virtual void multiply(const glm::mat4& matrix) = 0;
};
