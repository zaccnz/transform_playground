#pragma once

#include "node.h"

#include <fmt/core.h>
#include <nlohmann/json_fwd.hpp>

#define NODE_TYPE_ROTATE "Rotate"
#define NODE_TYPE_SCALE "Scale"
#define NODE_TYPE_TRANSLATE "Translate"

class RotateNode : public Node
{
private:
    float angle, x, y, z;

public:
    RotateNode() : Node(NODE_TYPE_ROTATE), angle(0.0), x(1.0), y(0.0), z(0.0) {}
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
    bool uiEditor();

    void setData(void *data);
    void *toData(int *size);
    void setJson(nlohmann::json &json);
    nlohmann::json toJson();
};

class ScaleNode : public Node
{
private:
    float x, y, z;

public:
    ScaleNode() : Node(NODE_TYPE_SCALE), x(1.0), y(1.0), z(1.0) {}
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
    bool uiEditor();

    void setData(void *data);
    void *toData(int *size);
    void setJson(nlohmann::json &json);
    nlohmann::json toJson();
};

class TranslateNode : public Node
{
private:
    float x, y, z;

public:
    TranslateNode() : Node(NODE_TYPE_TRANSLATE), x(0.0), y(0.0), z(0.0) {}
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
    bool uiEditor();

    void setData(void *data);
    void *toData(int *size);
    void setJson(nlohmann::json &json);
    nlohmann::json toJson();
};