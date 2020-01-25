#include "StateMachine.h"

#include <iostream>

StateMachine::StateMachine() :
    m_states(),
    m_pState(nullptr)
{
}

StateMachine::~StateMachine()
{
    while (!m_states.empty())
        popState();
}

void StateMachine::pushState(State* pState)
{
    if (!pState)
    {
        std::cerr << "Can only push valid state instances." << std::endl;
        return;
    }

    if (pState->getStateMachine())
    {
        std::cerr << "Cannot push a state that has already been added to another state machine!" << std::endl;
        return;
    }

    if (m_pState)
        m_pState->pause();

    m_pState = pState;

    m_states.push(m_pState);

    m_pState->_setStateMachine(this);
    m_pState->initialize();
    m_pState->resume();
}

void StateMachine::popState()
{
    if (!m_pState)
        return;

    m_pState->pause();
    delete m_pState;

    m_states.pop();

    if (m_states.empty())
    {
        m_pState = nullptr;
        return;
    }

    m_pState = m_states.top();
    m_pState->resume();
}

void StateMachine::update(float deltaTime)
{
    if (m_pState)
        m_pState->update(deltaTime);
}

void StateMachine::postUpdate(float deltaTime)
{
    if (m_pState)
        m_pState->postUpdate(deltaTime);
}

void StateMachine::prePhysicsTick(float physicsTimeStep)
{
    if (m_pState)
        m_pState->prePhysicsTick(physicsTimeStep);
}

void StateMachine::physicsTick(float physicsTimeStep)
{
    if (m_pState)
        m_pState->physicsTick(physicsTimeStep);
}
