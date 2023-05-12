#include "nodes/nodes.h"

#include <nlohmann/json.hpp>
#include <string>

#define CONSTRUCT_NODE(type, data)                  \
    if (!strcmp(type, NODE_TYPE_ROTATE))            \
    {                                               \
        return new RotateNode(data);                \
    }                                               \
    else if (!strcmp(type, NODE_TYPE_SCALE))        \
    {                                               \
        return new ScaleNode(data);                 \
    }                                               \
    else if (!strcmp(type, NODE_TYPE_TRANSLATE))    \
    {                                               \
        return new TranslateNode(data);             \
    }                                               \
    else if (!strcmp(type, NODE_TYPE_LIST))         \
    {                                               \
        return new ListNode(data);                  \
    }                                               \
    else if (!strcmp(type, NODE_TYPE_MATRIX_FRAME)) \
    {                                               \
        return new MatrixFrameNode(data);           \
    }                                               \
    else if (!strcmp(type, NODE_TYPE_CUBE))         \
    {                                               \
        return new CubeNode(data);                  \
    }                                               \
    else                                            \
    {                                               \
        printf("unknown node type %s", type);       \
        return nullptr;                             \
    }

Node *nodeFromData(const char *type, void *data)
{
    CONSTRUCT_NODE(type, data);
}

Node *nodeFromJson(nlohmann::json &data)
{
    if (!data.contains("type"))
    {
        printf("nodeFromJson: json is missing 'type'\n");
        return nullptr;
    }
    if (!data["type"].is_string())
    {
        printf("nodeFromJson: json 'type' is not a string\n");
        return nullptr;
    }

    std::string typeName = data["type"].get<std::string>();

    CONSTRUCT_NODE(typeName.c_str(), data)
}