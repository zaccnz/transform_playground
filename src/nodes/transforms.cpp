#include <nodes/transforms.h>

#include <imgui.h>
#include <rlgl.h>
#include <nlohmann/json.hpp>

void RotateNode::apply()
{
    if (!mEnabled)
        return;
    rlRotatef(angle, x, y, z);
}

bool RotateNode::uiEditor()
{
    bool changed = false;

    changed |= ImGui::SliderFloat("Angle", &angle, -180.0f, 180.0f,
                                  "%.3f", ImGuiSliderFlags_None);
    static float pos[3] = {x, y, z};
    if (ImGui::InputFloat3("Axis", pos, "%.2f", 0))
    {
        x = pos[0];
        y = pos[1];
        z = pos[2];
        changed = true;
    }
    return changed;
}

void RotateNode::setData(void *data)
{
    float *floatData = (float *)data;
    angle = floatData[0];
    x = floatData[1];
    y = floatData[2];
    z = floatData[3];
}

void *RotateNode::toData(int *size)
{
    int dataSize = sizeof(float) * 4;
    float *data = (float *)malloc(dataSize);
    data[0] = angle;
    data[1] = x;
    data[2] = y;
    data[3] = z;

    if (size)
    {
        *size = dataSize;
    }

    return data;
}

void RotateNode::setJson(nlohmann::json &json)
{
    angle = json["angle"].get<float>();
    x = json["x"].get<float>();
    y = json["y"].get<float>();
    z = json["z"].get<float>();
}

nlohmann::json RotateNode::toJson()
{
    nlohmann::json result;

    result["type"] = mName;
    result["angle"] = angle;
    result["x"] = x;
    result["y"] = y;
    result["z"] = z;

    return result;
}

void ScaleNode::apply()
{
    if (!mEnabled)
        return;
    rlScalef(x, y, z);
}

bool ScaleNode::uiEditor()
{
    static float pos[3] = {x, y, z};
    if (ImGui::InputFloat3("XYZ", pos, "%.2f", 0))
    {
        x = pos[0];
        y = pos[1];
        z = pos[2];
        return true;
    }
    return false;
}

void ScaleNode::setData(void *data)
{
    float *floatData = (float *)data;
    x = floatData[0];
    y = floatData[1];
    z = floatData[2];
}

void *ScaleNode::toData(int *size)
{
    int dataSize = sizeof(float) * 3;
    float *data = (float *)malloc(dataSize);
    data[0] = x;
    data[1] = y;
    data[2] = z;

    if (size)
    {
        *size = dataSize;
    }

    return data;
}

void ScaleNode::setJson(nlohmann::json &json)
{
    x = json["x"].get<float>();
    y = json["y"].get<float>();
    z = json["z"].get<float>();
}

nlohmann::json ScaleNode::toJson()
{
    nlohmann::json result;

    result["type"] = mName;
    result["x"] = x;
    result["y"] = y;
    result["z"] = z;

    return result;
}

void TranslateNode::apply()
{
    if (!mEnabled)
        return;
    rlTranslatef(x, y, z);
}

bool TranslateNode::uiEditor()
{
    static float pos[3] = {x, y, z};
    if (ImGui::InputFloat3("XYZ", pos, "%.2f", 0))
    {
        x = pos[0];
        y = pos[1];
        z = pos[2];
        return true;
    }
    return false;
}

void TranslateNode::setData(void *data)
{
    float *floatData = (float *)data;
    x = floatData[0];
    y = floatData[1];
    z = floatData[2];
}

void *TranslateNode::toData(int *size)
{
    int dataSize = sizeof(float) * 3;
    float *data = (float *)malloc(dataSize);
    data[0] = x;
    data[1] = y;
    data[2] = z;

    if (size)
    {
        *size = dataSize;
    }

    return data;
}

void TranslateNode::setJson(nlohmann::json &json)
{
    x = json["x"].get<float>();
    y = json["y"].get<float>();
    z = json["z"].get<float>();
}

nlohmann::json TranslateNode::toJson()
{
    nlohmann::json result;

    result["type"] = mName;
    result["x"] = x;
    result["y"] = y;
    result["z"] = z;

    return result;
}