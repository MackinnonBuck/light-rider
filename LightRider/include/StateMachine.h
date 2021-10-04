#pragma once

#include <stack>

#include "Component.h"
#include "State.h"

class State;

// Represents a component capable of exhibiting finite state behavior.
class StateMachine : public Component
{
public:

    // Initializes a new state machine instance.
    StateMachine();

    // Destroys the state machine.
    virtual ~StateMachine();

    // Pushes a new state on the state stack.
    void pushState(State* pState);

    // Pops the current state off the state stack.
    void popState();

    // Updates the state machine.
    virtual void update(float deltaTime);

    // Updates the state machine in the post-update stage.
    virtual void postUpdate(float deltaTime);

    // Called just before the physics simulation is advanced.
    virtual void prePhysicsTick(float physicsTimeStep);

    // Called just after the physics simulation is advanced.
    virtual void physicsTick(float physicsTimeStep);

private:

    // The state stack.
    std::stack<State*> m_states;

    // The active state.
    State* m_pState;
};

