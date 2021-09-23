#pragma once

#include <string>
#include <glm/gtc/type_ptr.hpp>

#include "GameObject.h"
#include "Components/BikeController.h"

// Contains functions to create GameObjects with a predefiened set of components.
// Feel free to add more presets here.
namespace Presets
{
    // Creates a new LightRider ground object.
    GameObject* createLightRiderGround(const std::string& name);

    // Creates a new LightRider bike object.
    GameObject* createLightRiderBike(const std::string& name, int playerId, const BikeControls& bikeControls,
        const glm::vec3& position, float yaw);

    // Creates a new LightRider bike display object.
    GameObject* createLightRiderBikeDisplay(const std::string& name, int playerId,
        const glm::vec3& position, float yaw);
}
