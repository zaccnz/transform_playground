/*
 * Scene Class
 * Should handle operation such as
 *  - scene export and import
 *  - undo, redo, node cut, copy and psste
 */

#pragma once

#include "nodes/nodes.h"

#include <vector>
#include <raylib.h>
#include <nlohmann/json.hpp>
#include <unordered_map>

#define MAX_ACTIONS 2048
#define NODE_ID_FIRST 1

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
    int nodeId;
    // delete
    int *childNodeIds;
    // create and delete
    const char *nodeType;
    // create and edit
    void *data;
    // edit and delete
    void *oldData;
    // create, delete, move
    int parentId;
    int nodeIndex;
    // move
    int newParentId;
    int newNodeIndex;
    // for chaining actions together (e.g., pasting 3 nodes)
    Action *next;

    static void freeAction(Action *action);
};

class Scene
{
private:
    Action mActions[MAX_ACTIONS];
    ListNode mSceneRoot;
    Camera mCamera = {0};
    std::unordered_map<int, Node *> mIds;
    std::unordered_map<Node *, int> mIdResolver;
    int mNextId = NODE_ID_FIRST;
    int mActionPointer = 0;
    int mActionCount = 0;
    int mActionChangePointer = 0;
    bool mActionChanges = false;
    int mActionPointerSaved = 0;
    Action mPasteFirst = {.type = ACTION_NOP};
    Action *mPasteLast = nullptr;

    void tempRegisterNode(Node *node, Node *parent);

    bool pushAction(Action &action);
    void createNodeInternal(const char *type, void *data, int nodeId, int parentId, int index, int *childNodeIds);
    void deleteNodeInternal(int nodeId);
    void moveNodeInternal(int nodeId, int newParentId, int newParentIndex);

    int registerChildren(ListNode *node, int *childNodeIds);
    void unregisterChildren(ListNode *node);
    void collectChildNodeIds(ListNode *node, std::vector<int> &store);

    void undoAction(Action *action);
    void redoAction(Action *action);

public:
    Scene();
    ~Scene();

    void undo();
    void redo(bool changePointer = false);

    void render();
    void update();

    void createNode(const char *type, void *data, Node *parent, int index);
    void deleteNode(Node *target);
    void editNode(Node *target, void *data, void *newData);
    void moveNode(Node *target, ListNode *newParent, int index);

    void cutNodes(Node **nodes, int count);
    void pasteStart();
    void pasteAddNode(const char *type, void *data, Node *parent, int index);
    void pasteCommit();

    bool canUndo();
    bool canRedo();

    bool load(char *path);
    bool save(char *path);

    bool areChangesUnsaved();

    ListNode *getSceneRoot() { return &mSceneRoot; }
    Camera *getCameraPtr() { return &mCamera; }
};