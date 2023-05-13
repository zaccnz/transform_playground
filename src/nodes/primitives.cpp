#include <nodes/primitives.h>

#include <imgui.h>
#include <raylib.h>
#include <rlgl.h>
#include <nlohmann/json.hpp>

#include <iostream>

Color colorFromFloat(float r, float g, float b)
{
    auto c = Color{
        (unsigned char)(r * 255.0f),
        (unsigned char)(g * 255.0f),
        (unsigned char)(b * 255.0f),
        255,
    };

    return c;
}

bool PrimitiveNode::uiEditor()
{
    bool changed = false;
    changed |= ImGui::SliderFloat("Red", &r, 0.0, 1.0, "%.2f");
    changed |= ImGui::SliderFloat("Green", &g, 0.0, 1.0, "%.2f");
    changed |= ImGui::SliderFloat("Blue", &b, 0.0, 1.0, "%.2f");
    return changed;
}

void CubeNode::apply()
{
    if (!mEnabled)
        return;
    DrawCube({0, 0, 0}, 1.0f, 1.0f, 1.0f, colorFromFloat(r, g, b));
}

bool CubeNode::uiEditor()
{
    return PrimitiveNode::uiEditor();
}

void CubeNode::setData(void *data)
{
    float *floatData = (float *)data;
    r = floatData[0];
    g = floatData[1];
    b = floatData[2];
}

void *CubeNode::toData(int *size)
{
    int dataSize = sizeof(float) * 3;
    float *data = (float *)malloc(dataSize);
    data[0] = r;
    data[1] = g;
    data[2] = b;

    if (size)
    {
        *size = dataSize;
    }

    return data;
}

void CubeNode::setJson(nlohmann::json &json)
{
    r = json["r"].get<float>();
    g = json["g"].get<float>();
    b = json["b"].get<float>();
}

nlohmann::json CubeNode::toJson()
{
    nlohmann::json result;

    result["type"] = mName;
    result["r"] = r;
    result["g"] = g;
    result["b"] = b;

    return result;
}