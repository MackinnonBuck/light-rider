#pragma once

#include <functional>

#include "Program.h"

struct RenderConfiguration
{
    std::function<bool(Program*)> canUseShader;
    std::function<void(Program*)> configureShader;
};
