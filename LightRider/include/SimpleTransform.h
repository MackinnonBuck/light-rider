#pragma once

#include "Transform.h"

class SimpleTransform : public Transform
{
public:

    // Creates a default SimpleTransform instance.
    SimpleTransform();

    // Creates a SimpleTransform instance with the provided position and rotation.
    SimpleTransform(glm::vec3 position, glm::quat rotation);

    // Returns a reference to the transform matrix representing this transform.
    virtual glm::mat4& getTransformMatrix();

    // Returns the position of this transform.
    virtual glm::vec3 getPosition() const;

    // Returns the rotation of this transform.
    virtual glm::quat getRotation() const;

    // Sets this transform's matrix directly.
    virtual void setTransformMatrix(glm::mat4 matrix);

    // Sets the postion of this transform.
    virtual void setPosition(glm::vec3 position);

    // Sets the rotation of this transform.
    virtual void setRotation(glm::quat rotation);

    // Multiplies this transform's matrix with the given matrix.
    virtual void multiply(const glm::mat4& matrix);

private:

    // The rotation matrix representing this transform.
    glm::mat4 m_transformMatrix;

    // The transform's origin.
    glm::vec3 m_origin;

    // The transform's basis matrix containing rotation information.
    glm::mat3 m_basis;

    // Updates the transform's matrix to reflect updated origin and basis values.
    void updateTransformMatrix();
};

