#pragma once

#include <nlohmann/json_fwd.hpp>
#include <raylib.h>

// stupid c++.... i want to call this namespace Camera but it would conflict with Raylib
namespace CameraUtil
{
    extern bool bControlHeld;
    extern bool bAltHeld;

    void fromJson(nlohmann::json &camJson, Camera *camera);
    nlohmann::json toJson();
    void update();
}