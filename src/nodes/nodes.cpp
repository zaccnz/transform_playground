#include "nodes/nodes.h"

#include <nlohmann/json.hpp>
#include <string>
#include <imgui.h>

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

Node *nodeFromType(const char *type)
{
    CONSTRUCT_NODE(type, );
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

const char *nodeListUi()
{
    const char *result = nullptr;
    if (ImGui::BeginMenu("Shape"))
    {
        if (ImGui::MenuItem("Cube"))
        {
            result = NODE_TYPE_CUBE;
        }
        if (ImGui::MenuItem("Sphere"))
        {
            result = nullptr;
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Transform"))
    {
        if (ImGui::MenuItem("Translate"))
        {
            result = NODE_TYPE_TRANSLATE;
        }
        if (ImGui::MenuItem("Rotate"))
        {
            result = NODE_TYPE_ROTATE;
        }
        if (ImGui::MenuItem("Scale"))
        {
            result = NODE_TYPE_SCALE;
        }
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Matrix Frame"))
    {
        result = NODE_TYPE_MATRIX_FRAME;
    }
    return result;
}