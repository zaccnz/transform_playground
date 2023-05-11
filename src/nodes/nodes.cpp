#include "nodes/nodes.h"

#include <nlohmann/json.hpp>
#include <string>

#define COMPARE_CONST_CHAR(a, b) (((void *)a) == ((void *)b))
#define COMPARE_STD_STRING(a, b) (!strcmp(a, b))

#define CONSTRUCT_NODE(type, compare, data)         \
    if (compare(type, NODE_TYPE_ROTATE))            \
    {                                               \
        return new RotateNode(data);                \
    }                                               \
    else if (compare(type, NODE_TYPE_SCALE))        \
    {                                               \
        return new ScaleNode(data);                 \
    }                                               \
    else if (compare(type, NODE_TYPE_TRANSLATE))    \
    {                                               \
        return new TranslateNode(data);             \
    }                                               \
    else if (compare(type, NODE_TYPE_LIST))         \
    {                                               \
        return new ListNode(data);                  \
    }                                               \
    else if (compare(type, NODE_TYPE_MATRIX_FRAME)) \
    {                                               \
        return new MatrixFrameNode(data);           \
    }                                               \
    else if (compare(type, NODE_TYPE_CUBE))         \
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
    CONSTRUCT_NODE(type, COMPARE_CONST_CHAR, data);
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

    CONSTRUCT_NODE(typeName.c_str(), COMPARE_STD_STRING, data)
}