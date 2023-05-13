#pragma once

#include "node.h"

#include <vector>
#include <fmt/core.h>
#include <nlohmann/json_fwd.hpp>

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
    ~ListNode()
    {
        releaseChildren();
    }

    void apply();

    std::string getLabel()
    {
        return fmt::format("List[child_count={}]", (int)mChildren.size());
    }

    bool isLeaf() { return false; }
    bool editable() { return false; }
    bool uiEditor() { return false; }

    void setData(void *data);
    void *toData(int *size);
    void setJson(nlohmann::json &json);
    nlohmann::json toJson();

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

        child->setParent(this);
        if (index >= mChildren.size())
        {
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

    void releaseChildren()
    {
        for (auto child : mChildren)
        {
            delete child;
        }
        mChildren.clear();
    }
};

class MatrixFrameNode : public ListNode
{
protected:
    void enter();
    void exit();
    bool uiEditor() { return false; }
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