#pragma once

#include "Scene.h"

class TestScene : public Scene
{
public:

    TestScene() : m_counter(0), m_pLast(nullptr) { }

    virtual void initialize();

    virtual void update(float deltaTime);

private:

    int m_counter;

    GameObject* m_pLast;

    void createSphere();
};

