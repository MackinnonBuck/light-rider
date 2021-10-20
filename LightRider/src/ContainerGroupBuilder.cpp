#include "ContainerGroupBuilder.h"

#include <iostream>

#include <btBulletDynamicsCommon.h>

#include "ConversionUtils.h"
#include "Shape.h"
#include "StaticCollisionObjectInfo.h"
#include "Components/MeshRenderer.h"
#include "Components/RigidBodyComponent.h"

ContainerGroupBuilder::ContainerGroupBuilder(float scale, float spacing) :
    m_scale(scale),
    m_spacing(spacing)
{
    m_pCompoundShape = new btCompoundShape(); // TODO: Disable dynamic AABB tree?
}

void ContainerGroupBuilder::addGroup(const glm::vec2& position, const glm::ivec3& dimensions)
{
    float scaleWithSpacing = m_scale + m_spacing;
    glm::vec3 halfExtents = glm::vec3(dimensions.x, 0.0f, dimensions.z) * scaleWithSpacing;
    glm::vec3 positionOffset = glm::one<glm::vec3>() * m_scale;

    for (int x = 0; x < dimensions.x; x++)
    {
        for (int y = 0; y < dimensions.y; y++)
        {
            for (int z = 0; z < dimensions.z; z++)
            {
                m_positions.push_back(glm::vec4(-halfExtents + glm::vec3(x * scaleWithSpacing, y * m_scale, z * scaleWithSpacing) * 2.0f + positionOffset, m_scale));
            }
        }
    }

    btVector3 spacingOffset = btVector3(m_spacing, 0.0f, m_spacing);
    btVector3 dimensionsWithSpacing = btVector3(dimensions.x * scaleWithSpacing, dimensions.y * m_scale, dimensions.z * scaleWithSpacing);
    btBoxShape* pBoxShape = new btBoxShape(dimensionsWithSpacing - spacingOffset);
    btTransform localTransform;
    localTransform.setIdentity();
    localTransform.setOrigin(btVector3(position.x, dimensions.y * m_scale, -position.y * 2) - spacingOffset);
    m_pCompoundShape->addChildShape(localTransform, pBoxShape);
}

GameObject* ContainerGroupBuilder::build(const std::string& shaderProgramId, const std::string& primaryTextureId, const std::string& shapeId)
{
    GameObject* pContainerGroupsObject = GameObject::create("ContainerGroups");
    // TODO: Would be nice if we didn't have to use detailed shadows. We would need a separate vertex shader for shadow rendering
    // that uses instancing. Maybe add an option to provide a custom shadow shader?
    MeshRenderer* pMeshRenderer = pContainerGroupsObject->addComponent<MeshRenderer>(shaderProgramId, primaryTextureId, shapeId, false, true);
    Shape* pShape = pMeshRenderer->getShape();

    if (pShape->usesInstancing())
    {
        std::cerr << "Instancing already configured for the provided shape." << std::endl;
        return nullptr;
    }

    pShape->generateInstanceData(m_positions.size(), 4, GL_FLOAT, m_positions.data());

    btTransform transform;
    transform.setIdentity();

    btDefaultMotionState* pMotionState = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, pMotionState, m_pCompoundShape);
    RigidBodyComponent* pRigidBodyComponent = pContainerGroupsObject->addComponent<RigidBodyComponent>(rbInfo);
    pRigidBodyComponent->setInfo(&StaticCollisionObjectInfo::getGroundInfo());
    btRigidBody* pRigidBody = pRigidBodyComponent->getRigidBody();
    pRigidBody->setFriction(1.0f);
    pRigidBody->setRestitution(1.0f);

    return pContainerGroupsObject;
}

