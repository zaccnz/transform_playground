#pragma once

#include "node.h"

#include <vector>
#include <fmt/core.h>
#include <nlohmann/json.hpp>

#define NODE_TYPE_LIST "List"
#define NODE_TYPE_MATRIX_FRAME "MatrixFrame"

class ListNode : public Node
{
protected:
    std::vector<Node *> mChildren;

    virtual void enter(){};
    virtual void exit(){};

    ListNode(const char *name) : Node(name) {}

public:
    ListNode() : Node(NODE_TYPE_LIST) {}
    ListNode(void *data) : Node(NODE_TYPE_LIST)
    {
        setData(data);
    }
    ListNode(nlohmann::json &json) : Node(NODE_TYPE_LIST)
    {
        setJson(json);
    }

    void apply()
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

    std::string getLabel()
    {
        return fmt::format("List[child_count={}]", (int)mChildren.size());
    }

    bool isLeaf() { return false; }
    bool editable() { return false; }
    void uiEditor() {}

    void setData(void *data)
    {
        char *dataBuffer = (char *)data;
        mChildren.clear();
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

            mChildren.push_back(child);
        }
    }

    void *toData(int *size)
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

    void setJson(nlohmann::json &json)
    {
        mChildren.clear();
        for (auto child : json["children"])
        {
            mChildren.push_back(nodeFromJson(child));
        }
    }

    nlohmann::json toJson()
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

    // child accessor functions
    void pushChild(Node *node)
    {
        node->setParent(this);
        mChildren.push_back(node);
    }

    int indexOfChild(Node *child)
    {
        auto res = std::find(mChildren.begin(), mChildren.end(), child);
        return res == mChildren.end() ? -1 : (int)(res - mChildren.begin());
    }

    void removeChild(int index)
    {
        Node *child = mChildren.at(index);
        child->setParent(NULL);
        mChildren.erase(mChildren.begin() + index);
    }

    void insertChild(int index, Node *child)
    {
        if (index < 0)
        {
            return;
        }
        printf("inserting child into index %d (size=%d)\n", index, mChildren.size());

        child->setParent(this);
        if (index >= mChildren.size())
        {
            printf("we are pushing back\n");
            mChildren.push_back(child);
            return;
        }
        mChildren.insert(mChildren.begin() + index, child);
    }

    int getChildCount()
    {
        return mChildren.size();
    }

    Node *getChild(int index)
    {
        return mChildren.at(index);
    }
};

class MatrixFrameNode : public ListNode
{
protected:
    void enter();
    void exit();
    void uiEditor() {}
    std::string getLabel()
    {
        return fmt::format("MatrixFrame[child_count={}]", (int)mChildren.size());
    }

public:
    MatrixFrameNode() : ListNode(NODE_TYPE_MATRIX_FRAME) {}
    MatrixFrameNode(nlohmann::json &json) : MatrixFrameNode()
    {
        setJson(json);
    }
    MatrixFrameNode(void *data) : MatrixFrameNode()
    {
        setData(data);
    }
};