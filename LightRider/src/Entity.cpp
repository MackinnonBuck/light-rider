#include "Entity.h"

#include <iostream>

void Entity::stamp(EntityContainer *pContainer, int id, long frameHash)
{
    if (m_pContainer)
    {
        std::cerr << "Cannot stamp an entity that has already been stamped!" << std::endl;
        return;
    }

    m_pContainer = pContainer;
    m_id = id;
    m_frameHash = frameHash;
}
