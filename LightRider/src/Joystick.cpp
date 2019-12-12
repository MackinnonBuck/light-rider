#include "Joystick.h"

#include <assert.h>
#include <GLFW/glfw3.h>

bool Joystick::initialize(int id)
{
    if (!glfwJoystickPresent(id))
        return false;

    m_id = id;
    m_name = glfwGetJoystickName(id);
    m_axes = glfwGetJoystickAxes(id, &m_axisCount);
    m_buttons = glfwGetJoystickButtons(id, &m_buttonCount);

    return true;
}

void Joystick::refresh()
{
    assert(m_id != -1);
    m_axes = glfwGetJoystickAxes(m_id, &m_axisCount);
    m_buttons = glfwGetJoystickButtons(m_id, &m_buttonCount);
}

int Joystick::getAxisCount() const
{
    assert(m_id != -1);
    return m_axisCount;
}

int Joystick::getButtonCount() const
{
    assert(m_id != -1);
    return m_buttonCount;
}

const char* Joystick::getName() const
{
    assert(m_id != -1);
    return m_name;
}

float Joystick::getAxis(int axis) const
{
    assert(m_id != -1);
    return axis >= m_axisCount ? 0.0f : m_axes[axis];
}

unsigned char Joystick::getButton(int button) const
{
    assert(m_id != -1);
    return button >= m_buttonCount ? 0.0f : m_buttons[button];
}
