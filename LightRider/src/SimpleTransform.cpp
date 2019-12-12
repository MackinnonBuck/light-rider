#include "SimpleTransform.h"

SimpleTransform::SimpleTransform() : m_transformMatrix(1.0f), m_origin(0.0f), m_basis(1.0f)
{
}

SimpleTransform::SimpleTransform(glm::vec3 position, glm::quat rotation) : m_origin(position), m_basis(rotation)
{
    updateTransformMatrix();
}

glm::mat4& SimpleTransform::getTransformMatrix()
{
    return m_transformMatrix;
}

glm::vec3 SimpleTransform::getPosition() const
{
    return m_origin;
}

glm::quat SimpleTransform::getRotation() const
{
    return m_basis;
}

void SimpleTransform::setTransformMatrix(glm::mat4 matrix)
{
    m_origin = matrix[3];
    m_basis = glm::mat3(matrix);

    updateTransformMatrix();
}

void SimpleTransform::setPosition(glm::vec3 position)
{
    m_origin = position;

    updateTransformMatrix();
}

void SimpleTransform::setRotation(glm::quat rotation)
{
    m_basis = glm::mat3_cast(rotation);

    updateTransformMatrix();
}

void SimpleTransform::multiply(const glm::mat4& matrix)
{
    m_origin += m_basis * glm::vec3(matrix[3]);
    m_basis *= glm::mat3(matrix);

    updateTransformMatrix();
}

void SimpleTransform::updateTransformMatrix()
{
    m_transformMatrix = glm::mat4(m_basis);
    m_transformMatrix[3] = glm::vec4(m_origin, 1.0f);
}
