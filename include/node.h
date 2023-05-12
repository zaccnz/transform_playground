#pragma once

#include <nlohmann/json_fwd.hpp>
#include <string>

class Node
{
protected:
    const char *mName = nullptr;
    Node *mParent = nullptr;
    bool mEnabled = true;

    Node() {}
    Node(const char *name) : mName(name) {}

public:
    virtual ~Node() {}

    // Virtual node functions
    virtual void apply() = 0;
    virtual std::string getLabel() = 0;
    virtual bool isLeaf() { return true; }
    virtual bool editable() = 0;
    virtual void uiEditor() = 0;
    virtual void setData(void *data) = 0;
    virtual void *toData(int *size) = 0;
    virtual void setJson(nlohmann::json &json) = 0;
    virtual nlohmann::json toJson() = 0;

    // Node helper functions
    const char *getName()
    {
        return mName;
    }

    void setParent(Node *parent)
    {
        mParent = parent;
    }

    Node *getParent()
    {
        return mParent;
    }

    bool isDescendant(Node *parent)
    {
        Node *node = mParent;
        while (node != nullptr)
        {
            if (node == parent)
            {
                return true;
            }
            node = node->getParent();
        }
        return false;
    }

    void setEnabled(bool enabled)
    {
        mEnabled = enabled;
    }

    bool isEnabled()
    {
        if (mEnabled && mParent)
        {
            return mParent->isEnabled();
        }
        return mEnabled;
    }
};

// nodes/nodes.cpp
Node *nodeFromData(const char *type, void *data);
Node *nodeFromJson(nlohmann::json &data);