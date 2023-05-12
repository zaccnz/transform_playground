#include "nodes/lists.h"

#include <rlgl.h>
#include <nlohmann/json.hpp>

void ListNode::apply()
{
    if (!mEnabled)
        return;
    enter();
    for (auto &child : mChildren)
    {
        child->apply();
    }
    exit();
}

void ListNode::setData(void *data)
{
    releaseChildren();

    char *dataBuffer = (char *)data;
    int childCount = *(int *)data;
    dataBuffer += sizeof(int);
    for (int i = 0; i < childCount; i++)
    {
        int childSize = *(int *)dataBuffer;
        dataBuffer += sizeof(int);
        const char *childType = *(const char **)dataBuffer;
        dataBuffer += sizeof(const char *);
        Node *child = nodeFromData(childType, dataBuffer);
        dataBuffer += childSize;

        child->setParent(this);

        mChildren.push_back(child);
    }
}

void *ListNode::toData(int *size)
{
    int childCount = mChildren.size();
    void **childData = (void **)malloc(childCount * sizeof(void *));
    const char **childTypes = (const char **)malloc(childCount * sizeof(char *));
    int *childSizes = (int *)malloc(childCount * sizeof(int));
    int dataSize = sizeof(int);

    // load child data
    for (int i = 0; i < childCount; i++)
    {
        int childSize;
        childData[i] = mChildren[i]->toData(&childSize);
        childTypes[i] = mChildren[i]->getName();
        childSizes[i] = childSize;

        dataSize += sizeof(int) + sizeof(const char *) + childSize;
    }

    char *data = (char *)malloc(dataSize);
    int index = sizeof(int);
    *((int *)data) = childCount;
    // copy data into our buffer
    for (int i = 0; i < childCount; i++)
    {
        *((int *)(data + index)) = childSizes[i];
        index += sizeof(int);
        *((const char **)(data + index)) = childTypes[i];
        index += sizeof(const char *);
        memcpy(data + index, childData[i], childSizes[i]);
        index += childSizes[i];
    }

    if (size)
    {
        *size = dataSize;
    }
    // release temp memory
    for (int i = 0; i < childCount; i++)
    {
        free(childData[i]);
    }
    free(childData);
    free(childSizes);
    return data;
}

void ListNode::setJson(nlohmann::json &json)
{
    releaseChildren();
    for (auto child : json["children"])
    {
        mChildren.push_back(nodeFromJson(child));
    }
}

nlohmann::json ListNode::toJson()
{
    nlohmann::json result;

    result["type"] = mName;
    result["children"] = nlohmann::json::array();
    for (auto child : mChildren)
    {
        result["children"].push_back(child->toJson());
    }

    return result;
}

void MatrixFrameNode::enter()
{
    rlPushMatrix();
}

void MatrixFrameNode::exit()
{
    rlPopMatrix();
}