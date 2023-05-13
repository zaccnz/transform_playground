#pragma once

#include "node.h"

#include <fmt/core.h>
#include <nlohmann/json_fwd.hpp>

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
    bool uiEditor();
};

class CubeNode : public PrimitiveNode
{
private:
public:
    CubeNode() : PrimitiveNode(NODE_TYPE_CUBE, 1.0, 1.0, 1.0) {}
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

    bool uiEditor();

    void setData(void *data);
    void *toData(int *size);
    void setJson(nlohmann::json &json);
    nlohmann::json toJson();
};