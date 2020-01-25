#pragma once

#include "StateMachine.h"

class StateMachine;

class State
{
public:

    // Destroys this state.
    virtual ~State() { }

    // Gets the parent state machine.
    StateMachine* getStateMachine() const;

    // Called when the state initializes for the first time.
    virtual void initialize() { };

    // Called when the state resumes active behavior.
    virtual void resume() { };

    // Updates the State.
    virtual void update(float deltaTime) { };

    // Updates this state in the post-update stage.
    virtual void postUpdate(float deltaTime) { };

    // Called just before the scene's physics are updated.
    virtual void prePhysicsTick(float physicsTimeStep) { };

    // Called just after the scene's physics are updated.
    virtual void physicsTick(float physicsTimeStep) { };

    // Called when the state gets paused.
    virtual void pause() { };

    // Sets the parent state machine (internal use only).
    void _setStateMachine(StateMachine* pStateMachine);

private:

    // The parent state machine.
    StateMachine* m_pStateMachine;
};
