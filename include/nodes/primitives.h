#pragma once

#include "node.h"

#include <fmt/core.h>
#include <nlohmann/json.hpp>

#define NODE_TYPE_CUBE "Cube"

class PrimitiveNode : public Node
{
protected:
    float r, g, b;

public:
    PrimitiveNode(const char *name, float r, float g, float b) : Node(name), r(r), g(g), b(b) {}

    PrimitiveNode(const PrimitiveNode &other) : Node(other.mName)
    {
        r = other.r;
        g = other.g;
        b = other.b;
    }

    virtual void apply() = 0;
    virtual std::string getLabel() = 0;
    virtual void setData(void *data) = 0;
    virtual void *toData(int *size) = 0;
    virtual void setJson(nlohmann::json &json) = 0;
    virtual nlohmann::json toJson() = 0;

    bool editable() { return true; }
    void uiEditor();
};

class CubeNode : public PrimitiveNode
{
private:
public:
    CubeNode(float r, float g, float b) : PrimitiveNode(NODE_TYPE_CUBE, r, g, b) {}

    CubeNode(const CubeNode &other) : PrimitiveNode(NODE_TYPE_CUBE, other.r, other.g, other.b) {}
    CubeNode(void *data) : PrimitiveNode(NODE_TYPE_CUBE, 0.0, 0.0, 0.0)
    {
        setData(data);
    }
    CubeNode(nlohmann::json &json) : PrimitiveNode(NODE_TYPE_CUBE, 0.0, 0.0, 0.0)
    {
        setJson(json);
    }

    void apply();
    std::string getLabel()
    {
        return fmt::format("Cube[r={:.2f},g={:.2f},b={:.2f}]", r, g, b);
    };

    void uiEditor();

    void setData(void *data)
    {
        float *floatData = (float *)data;
        r = floatData[0];
        g = floatData[1];
        b = floatData[2];
    }

    void *toData(int *size)
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

    void setJson(nlohmann::json &json)
    {
        r = json["r"].get<float>();
        g = json["g"].get<float>();
        b = json["b"].get<float>();
    }

    nlohmann::json toJson()
    {
        nlohmann::json result;

        result["type"] = mName;
        result["r"] = r;
        result["g"] = g;
        result["b"] = b;

        return result;
    }
};