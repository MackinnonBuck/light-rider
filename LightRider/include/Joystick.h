#pragma once

// Represents a plugged-in joystick (game controller).
class Joystick
{
public:

    // Creates a new joystick instance.
    Joystick() : m_id(-1), m_axisCount(0), m_buttonCount(0), m_name(nullptr),
        m_axes(nullptr), m_buttons(nullptr) { }

    // Initializes the joystick, returning true if the joystick was found, otherwise false.
    bool initialize(int id);

    // Refreshes joystick values.
    void refresh();

    // Returns the number of axes on the joystick.
    int getAxisCount() const;

    // Returns the number of buttons on the joystick.
    int getButtonCount() const;

    // Returns the name of the joystick.
    const char* getName() const;

    // Returns the value of the given axis.
    float getAxis(int axis) const;

    // Returns the value of the given button.
    unsigned char getButton(int button) const;

private:

    // The id of the controller.
    int m_id;

    // The number of axes on the controller.
    int m_axisCount;

    // The number of buttons on the controller.
    int m_buttonCount;

    // The name of the controller.
    const char* m_name;

    // An array of all axis values.
    const float* m_axes;

    // An array of all button values.
    const unsigned char* m_buttons;
};

