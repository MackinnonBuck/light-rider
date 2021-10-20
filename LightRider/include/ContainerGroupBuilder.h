#pragma once

#include <vector>

#include <glm/gtc/type_ptr.hpp>

#include <btBulletCollisionCommon.h>

#include "Component.h"
#include "GameObject.h"

class ContainerGroupBuilder
{
public:
    ContainerGroupBuilder(float scale, float spacing);

    void addGroup(const glm::vec2& position, const glm::ivec3& dimensions);
    GameObject* build(const std::string& shaderProgramId, const std::string& primaryTextureId, const std::string& shapeId);

private:
    float m_scale;
    float m_spacing;
    std::vector<glm::vec4> m_positions;
    btCompoundShape* m_pCompoundShape;
};

