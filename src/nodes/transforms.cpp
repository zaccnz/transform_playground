#include <nodes/transforms.h>

#include <rlgl.h>
#include <nlohmann/json.hpp>

void RotateNode::apply()
{
    if (!mEnabled)
        return;
    rlRotatef(angle, x, y, z);
}

void RotateNode::uiEditor()
{
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

void ScaleNode::uiEditor()
{
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

void TranslateNode::uiEditor()
{
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