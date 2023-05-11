#pragma once

#include "node.h"

#include <fmt/core.h>
#include <nlohmann/json.hpp>

#define NODE_TYPE_ROTATE "Rotate"
#define NODE_TYPE_SCALE "Scale"
#define NODE_TYPE_TRANSLATE "Translate"

class RotateNode : public Node
{
private:
    float angle, x, y, z;

public:
    RotateNode(float angle, float x, float y, float z) : Node(NODE_TYPE_ROTATE), angle(angle), x(x), y(y), z(z) {}

    RotateNode(const RotateNode &other) : Node(NODE_TYPE_ROTATE)
    {
        angle = other.angle;
        x = other.x;
        y = other.y;
        z = other.z;
    }

    RotateNode(void *data) : Node(NODE_TYPE_ROTATE)
    {
        setData(data);
    }

    RotateNode(nlohmann::json &json) : Node(NODE_TYPE_ROTATE)
    {
        setJson(json);
    }

    void apply();

    std::string getLabel()
    {
        return fmt::format("Rotate[angle={:.2f},x={:.2f},y={:.2f},z={:.2f}]", angle, x, y, z);
    }

    bool editable() { return true; }
    void uiEditor();

    void setData(void *data)
    {
        float *floatData = (float *)data;
        angle = floatData[0];
        x = floatData[1];
        y = floatData[2];
        z = floatData[3];
    }

    void *toData(int *size)
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

    void setJson(nlohmann::json &json)
    {
        angle = json["angle"].get<float>();
        x = json["x"].get<float>();
        y = json["y"].get<float>();
        z = json["z"].get<float>();
    }

    nlohmann::json toJson()
    {
        nlohmann::json result;

        result["type"] = mName;
        result["angle"] = angle;
        result["x"] = x;
        result["y"] = y;
        result["z"] = z;

        return result;
    }
};

class ScaleNode : public Node
{
private:
    float x, y, z;

public:
    ScaleNode(float x, float y, float z) : Node(NODE_TYPE_SCALE), x(x), y(y), z(z) {}

    ScaleNode(const ScaleNode &other) : Node(NODE_TYPE_SCALE)
    {
        x = other.x;
        y = other.y;
        z = other.z;
    }

    ScaleNode(void *data) : Node(NODE_TYPE_SCALE)
    {
        setData(data);
    }

    ScaleNode(nlohmann::json &json) : Node(NODE_TYPE_SCALE)
    {
        setJson(json);
    }

    void apply();

    std::string getLabel()
    {
        return fmt::format("Scale[x={:.2f},y={:.2f},z={:.2f}]", x, y, z);
    }

    bool editable() { return true; }
    void uiEditor();

    void setData(void *data)
    {
        float *floatData = (float *)data;
        x = floatData[0];
        y = floatData[1];
        z = floatData[2];
    }

    void *toData(int *size)
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

    void setJson(nlohmann::json &json)
    {
        x = json["x"].get<float>();
        y = json["y"].get<float>();
        z = json["z"].get<float>();
    }

    nlohmann::json toJson()
    {
        nlohmann::json result;

        result["type"] = mName;
        result["x"] = x;
        result["y"] = y;
        result["z"] = z;

        return result;
    }
};

class TranslateNode : public Node
{
private:
    float x, y, z;

public:
    TranslateNode(float x, float y, float z) : Node(NODE_TYPE_TRANSLATE), x(x), y(y), z(z) {}

    TranslateNode(const TranslateNode &other) : Node(NODE_TYPE_TRANSLATE)
    {
        x = other.x;
        y = other.y;
        z = other.z;
    }

    TranslateNode(void *data) : Node(NODE_TYPE_TRANSLATE)
    {
        setData(data);
    }

    TranslateNode(nlohmann::json &json) : Node(NODE_TYPE_TRANSLATE)
    {
        setJson(json);
    }

    void apply();

    std::string getLabel()
    {
        return fmt::format("Translate[x={:.2f},y={:.2f},z={:.2f}]", x, y, z);
    }

    bool editable() { return true; }
    void uiEditor();

    void setData(void *data)
    {
        float *floatData = (float *)data;
        x = floatData[0];
        y = floatData[1];
        z = floatData[2];
    }

    void *toData(int *size)
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

    void setJson(nlohmann::json &json)
    {
        x = json["x"].get<float>();
        y = json["y"].get<float>();
        z = json["z"].get<float>();
    }

    nlohmann::json toJson()
    {
        nlohmann::json result;

        result["type"] = mName;
        result["x"] = x;
        result["y"] = y;
        result["z"] = z;

        return result;
    }
};