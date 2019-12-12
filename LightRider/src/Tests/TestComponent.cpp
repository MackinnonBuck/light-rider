#include "Tests\TestComponent.h"

#include <iostream>

void TestComponent::update(float deltaTime)
{
    std::cout << "UPDATE: " << getGameObject()->getName() << std::endl;
}

void TestComponent::postUpdate(float deltaTime)
{
    std::cout << "POST UPDATE: " << getGameObject()->getName() << std::endl;
}

void TestComponent::prePhysicsTick(float physicsTimeStep)
{
    std::cout << "PRE PHYSICS: " << getGameObject()->getName() << std::endl;
}

void TestComponent::physicsTick(float physicsTimeStep)
{
    std::cout << "PHYSICS: " << getGameObject()->getName() << std::endl;
}
