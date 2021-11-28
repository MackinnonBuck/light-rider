#pragma once

#include <vector>
#include <string>

#include "Program.h"
#include "Scene.h"

// Contains common scene functionality to scenes in LightRider.
class LightRiderScene : public Scene
{
protected:

    // Loads common LightRider game assets.
    virtual void loadAssets();

private:
    Program* loadShaderProgramWithDynamicOutput(const std::string& id, const std::string& vertexShaderFileName,
        const std::string& fragmentShaderFileName, ShaderUniform defaultUniforms = ShaderUniform::DEFAULT);
};

