#include "scene.h"

#include "app.h"

#include <raylib.h>
#include <rlgl.h>

void Action::freeAction(Action *action)
{
    if (action->childNodeIds)
    {
        free(action->childNodeIds);
    }
    if (action->data)
    {
        free(action->data);
    }
    if (action->oldData)
    {
        free(action->oldData);
    }
    if (action->next)
    {
        freeAction(action->next);
    }
}

Scene::Scene()
{
    // mSceneRoot.pushChild(new CubeNode(1.0, 1.0, 1.0));

    int id = mNextId++;
    mIds.insert({id, &mSceneRoot});
    mIdResolver.insert({&mSceneRoot, id});

    MatrixFrameNode *frame = new MatrixFrameNode();
    tempRegisterNode(new RotateNode(45, 1, 0, 0), frame);
    tempRegisterNode(new TranslateNode(2, 0, 0), frame);
    tempRegisterNode(new ScaleNode(0.5, 2, 1), &mSceneRoot);
    tempRegisterNode(frame, &mSceneRoot);
    tempRegisterNode(new CubeNode(1.0, 0.5, 1.0), &mSceneRoot);
    tempRegisterNode(new CubeNode(1.0, 1.0, 1.0), frame);

    mCamera.position = (Vector3){5.0f, 5.0f, 5.0f};
    mCamera.target = (Vector3){0.0f, 0.0f, 0.0f};
    mCamera.up = (Vector3){0.0f, 1.0f, 0.0f};
    mCamera.fovy = 45.0f;
    mCamera.projection = CAMERA_PERSPECTIVE;
}

Scene::~Scene()
{
    for (int i = 0; i < mActionCount; i++)
    {
        Action::freeAction(&mActions[i]);
    }
}

void Scene::tempRegisterNode(Node *node, Node *parent)
{
    int id = mNextId++;
    mIds.insert({id, node});
    mIdResolver.insert({node, id});
    ((ListNode *)parent)->pushChild(node);
}

bool Scene::pushAction(Action &action)
{
    if (mActionPointer < mActionCount)
    {
        for (int i = mActionPointer; i < mActionCount; i++)
        {
            Action::freeAction(&mActions[i]);
        }
    }
    if (mActionCount >= MAX_ACTIONS)
    {
        printf("out of actions\n");
        return false;
    }
    mActionCount = mActionPointer + 1;
    mActions[mActionPointer] = action;
    if (!mActionChanges)
    {
        mActionChangePointer = mActionPointer++;
    }
    mActionChanges = true;

    return true;
}

void Scene::createNodeInternal(const char *type, void *data, int nodeId, int parentId, int index, int *childNodeIds)
{
    Node *node;
    if (data)
    {
        node = nodeFromData(type, data);
    }
    else
    {
        node = nodeFromType(type);
    }
    mIds.insert({nodeId, node});
    mIdResolver.insert({node, nodeId});

    if (!node->isLeaf())
    {
        registerChildren((ListNode *)node, childNodeIds);
    }

    ListNode *parent = (ListNode *)mIds.at(parentId);
    parent->insertChild(index, node);
}

void Scene::deleteNodeInternal(int nodeId)
{
    Node *node = mIds.at(nodeId);
    mIds.erase(nodeId);
    mIdResolver.erase(node);

    if (!node->isLeaf())
    {
        unregisterChildren((ListNode *)node);
    }

    ListNode *parent = (ListNode *)node->getParent();
    int index = parent->indexOfChild(node);
    parent->removeChild(index);
    delete node;
}

void Scene::moveNodeInternal(int nodeId, int newParentId, int newParentIndex)
{
    Node *node = mIds.at(nodeId);

    ListNode *parent = (ListNode *)node->getParent();
    int oldIndex = parent->indexOfChild(node);
    parent->removeChild(oldIndex);

    ListNode *newParent = (ListNode *)mIds.at(newParentId);
    newParent->insertChild(newParentIndex, node);
}

int Scene::registerChildren(ListNode *node, int *childNodeIds)
{
    int childCount = node->getChildCount();
    int totalCount = childCount;
    for (int i = 0; i < childCount; i++)
    {
        Node *child = node->getChild(i);
        mIds.insert({*childNodeIds, child});
        mIdResolver.insert({child, *childNodeIds});
        childNodeIds++;
        if (!child->isLeaf())
        {
            int count = registerChildren((ListNode *)child, childNodeIds);
            childNodeIds += count;
            totalCount += count;
        }
    }
    return totalCount;
}

void Scene::unregisterChildren(ListNode *node)
{
    int childCount = node->getChildCount();
    for (int i = 0; i < childCount; i++)
    {
        Node *child = node->getChild(i);
        mIds.erase(mIdResolver.at(child));
        mIdResolver.erase(child);
        if (!child->isLeaf())
        {
            unregisterChildren((ListNode *)child);
        }
    }
}

void Scene::collectChildNodeIds(ListNode *node, std::vector<int> &store)
{
    int childCount = node->getChildCount();
    for (int i = 0; i < childCount; i++)
    {
        Node *child = node->getChild(i);
        int id = mIdResolver.at(child);
        store.push_back(id);
        if (!child->isLeaf())
        {
            collectChildNodeIds((ListNode *)child, store);
        }
    }
}

void Scene::undoAction(Action *action)
{
    if (action->next)
    {
        undoAction(action->next);
    }

    switch (action->type)
    {
    case ACTION_CREATE_NODE:
    {
        app->deselectNode();
        deleteNodeInternal(action->nodeId);
        break;
    }
    case ACTION_DELETE_NODE:
    {
        createNodeInternal(action->nodeType, action->oldData, action->nodeId, action->parentId, action->nodeIndex, action->childNodeIds);
        break;
    }
    case ACTION_EDIT_NODE:
    {
        Node *node = mIds.at(action->nodeId);
        node->setData(action->oldData);
        break;
    }
    case ACTION_MOVE_NODE:
    {
        moveNodeInternal(action->nodeId, action->parentId, action->nodeIndex);
        break;
    }
    default:
    {
        printf("invalid mode type %d\n", action->type);
        break;
    }
    }
}

void Scene::redoAction(Action *action)
{
    switch (action->type)
    {
    case ACTION_CREATE_NODE:
    {
        createNodeInternal(action->nodeType, action->data, action->nodeId, action->parentId, action->nodeIndex, nullptr);
        break;
    }
    case ACTION_DELETE_NODE:
    {
        app->deselectNode();
        deleteNodeInternal(action->nodeId);
        break;
    }
    case ACTION_EDIT_NODE:
    {
        Node *node = mIds.at(action->nodeId);
        node->setData(action->data);
        break;
    }
    case ACTION_MOVE_NODE:
    {
        moveNodeInternal(action->nodeId, action->newParentId, action->newNodeIndex);
        break;
    }
    default:
    {
        printf("invalid mode type %d\n", action->type);
        break;
    }
    }

    if (action->next)
    {
        redoAction(action->next);
    }
}

void Scene::undo()
{
    if (!canUndo())
    {
        return;
    }

    Action *action = &mActions[--mActionPointer];
    undoAction(action);
}

void Scene::redo(bool changePointer)
{

    if (!(changePointer || canRedo()))
    {
        return;
    }

    Action *action = &mActions[(changePointer ? mActionChangePointer : mActionPointer)++];
    redoAction(action);
}

void Scene::render()
{
    rlPushMatrix();
    mSceneRoot.apply();
    rlPopMatrix();
}

void Scene::update()
{
    while (mActionChangePointer < mActionPointer)
    {
        redo(true);
        mActionChangePointer++;
    }
    mActionChanges = false;
}

void Scene::createNode(const char *type, void *data, Node *parent, int index)
{
    Action newNodeAction = {
        .type = ACTION_CREATE_NODE,
        .nodeId = mNextId++,
        .childNodeIds = nullptr,
        .nodeType = type,
        .data = data,
        .oldData = nullptr,
        .parentId = mIdResolver.at(parent),
        .nodeIndex = index,
        .newParentId = 0,
        .newNodeIndex = 0,
        .next = nullptr,
    };

    pushAction(newNodeAction);
}

void Scene::deleteNode(Node *target)
{
    int *childNodeIds = nullptr;
    if (!target->isLeaf())
    {
        std::vector<int> childNodeIdVec;

        collectChildNodeIds((ListNode *)target, childNodeIdVec);

        if (childNodeIdVec.size() > 0)
        {
            int bufferSize = childNodeIdVec.size() * sizeof(int);
            childNodeIds = (int *)malloc(bufferSize);
            memcpy(childNodeIds, childNodeIdVec.data(), bufferSize);
        }
    }
    ListNode *parent = (ListNode *)target->getParent();
    Action deleteNodeAction = {
        .type = ACTION_DELETE_NODE,
        .nodeId = mIdResolver.at(target),
        .childNodeIds = childNodeIds,
        .nodeType = target->getName(),
        .data = nullptr,
        .oldData = target->toData(nullptr),
        .parentId = mIdResolver.at(parent),
        .nodeIndex = parent->indexOfChild(target),
        .newParentId = 0,
        .newNodeIndex = 0,
        .next = nullptr,
    };

    pushAction(deleteNodeAction);
}

void Scene::editNode(Node *target, void *data, void *newData)
{
    Action editNodeAction = {
        .type = ACTION_EDIT_NODE,
        .nodeId = mIdResolver.at(target),
        .childNodeIds = nullptr,
        .nodeType = nullptr,
        .data = newData,
        .oldData = data,
        .parentId = 0,
        .nodeIndex = 0,
        .newParentId = 0,
        .newNodeIndex = 0,
        .next = nullptr,
    };

    pushAction(editNodeAction);
}

void Scene::moveNode(Node *target, ListNode *newParent, int index)
{
    ListNode *parent = (ListNode *)target->getParent();
    Action moveNodeAction = {
        .type = ACTION_MOVE_NODE,
        .nodeId = mIdResolver.at(target),
        .childNodeIds = nullptr,
        .nodeType = nullptr,
        .data = nullptr,
        .oldData = nullptr,
        .parentId = mIdResolver.at(parent),
        .nodeIndex = parent->indexOfChild(target),
        .newParentId = mIdResolver.at(newParent),
        .newNodeIndex = index,
        .next = nullptr,
    };

    pushAction(moveNodeAction);
}

bool Scene::canUndo()
{
    return mActionPointer > 0;
}

bool Scene::canRedo()
{
    return mActionPointer < mActionCount;
}

bool Scene::load(char *path)
{
    return false;
}

bool Scene::save(char *path)
{
    mActionPointerSaved = mActionPointer;
    return false;
}

bool Scene::areChangesUnsaved()
{
    return mActionPointerSaved != mActionPointer;
}