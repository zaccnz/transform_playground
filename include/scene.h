/*
 * Scene Class
 * Should handle operation such as
 *  - scene export and import
 *  - undo, redo, node cut, copy and psste
 */

#pragma once

#include "nodes/nodes.h"

#include <unordered_map>

#define MAX_ACTIONS 2048
#define INDEX_FIRST 1

enum ActionType
{
    ACTION_NOP = 0,
    ACTION_CREATE_NODE,
    ACTION_DELETE_NODE,
    ACTION_EDIT_NODE,
    ACTION_MOVE_NODE,
    ACTION_MAX,
};

struct Action
{
    ActionType type;
    // all actions
    int node_id;
    // create
    const char *type;
    // create and edit
    void *data;
    // edit
    void *data_was;
    // create, delete, move
    int parent_id;
    int node_index;
    // move
    int new_parent_id;
    int new_node_index;
    // for chaining actions together (like pasting 3 nodes)
    Action *next;

    void freeAction(Action *action);
};

class Scene
{
private:
    Action mActions[MAX_ACTIONS];
    ListNode mSceneRoot;
    std::unordered_map<int, Node *> mIds;
    std::unordered_map<Node *, int> mIdResolver;
    int mNextIndex;
    int mActionPointer;
    int mActionCount;

    bool pushAction(Action &action);

public:
    Scene();
    ~Scene();

    void createNode();
    void deleteNode(Node *target);
    void editNode(Node *target, void *newData);
    void moveNode(Node *target, ListNode *newParent, int index);

    void undo();
    void redo();

    bool canUndo();
    bool canRedo();

    bool load();
    bool save();

    // TODO: figure out params
    void copyNode();
    void copyNodes();
    void pasteNodes();
};