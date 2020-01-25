#include "State.h"

#include <iostream>

StateMachine* State::getStateMachine() const
{
    return m_pStateMachine;
}

void State::_setStateMachine(StateMachine* pParent)
{
    if (m_pStateMachine)
    {
        std::cerr << "Cannot set the state machine of a state that already has a parent assigned!" << std::endl;
        return;
    }

    m_pStateMachine = pParent;
}