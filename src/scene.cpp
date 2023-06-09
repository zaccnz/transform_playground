#include "scene.h"

#include "app.h"
#include "camera.h"

#include <raylib.h>
#include <rlgl.h>
#include <sstream>
#include <nlohmann/json.hpp>

void (*cFree)(void *) = free;

void Action::free(Action *action)
{
    if (action->childNodeIds)
    {
        cFree(action->childNodeIds);
    }
    if (action->data)
    {
        cFree(action->data);
    }
    if (action->oldData)
    {
        cFree(action->oldData);
    }
    if (action->next)
    {
        free(action->next);
        cFree(action->next);
    }
}

Scene::Scene()
{
    // mSceneRoot.pushChild(new CubeNode(1.0, 1.0, 1.0));

    mSceneRoot = new ListNode();

    int id = mNextId++;
    mIds.insert({id, mSceneRoot});
    mIdResolver.insert({mSceneRoot, id});

    CubeNode *cube = new CubeNode();
    mSceneRoot->pushChild(cube);

    id = mNextId++;
    mIds.insert({id, cube});
    mIdResolver.insert({cube, id});

    mCamera.position = (Vector3){5.0f, 5.0f, 5.0f};
    mCamera.target = (Vector3){0.0f, 0.0f, 0.0f};
    mCamera.up = (Vector3){0.0f, 1.0f, 0.0f};
    mCamera.fovy = 45.0f;
    mCamera.projection = CAMERA_PERSPECTIVE;
}

Scene::Scene(char *sceneJson)
{
    nlohmann::json scene = nlohmann::json::parse(sceneJson);

    CameraUtil::fromJson(scene["camera"], &mCamera);
    mSceneRoot = (ListNode *)nodeFromJson(scene["root"]);

    mCamera.up = (Vector3){0.0f, 1.0f, 0.0f};

    int id = mNextId++;
    mIds.insert({id, mSceneRoot});
    mIdResolver.insert({mSceneRoot, id});

    registerChildren(mSceneRoot, nullptr);
}

Scene::~Scene()
{
    for (int i = 0; i < mActionCount; i++)
    {
        Action::free(&mActions[i]);
    }
}

bool Scene::pushAction(Action &action)
{
    if (mActionPointer < mActionCount)
    {
        for (int i = mActionPointer; i < mActionCount; i++)
        {
            Action::free(&mActions[i]);
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
        int id = childNodeIds ? *childNodeIds : mNextId++;
        mIds.insert({id, child});
        mIdResolver.insert({child, id});
        if (childNodeIds)
        {
            childNodeIds++;
        }
        if (!child->isLeaf())
        {
            int count = registerChildren((ListNode *)child, childNodeIds);
            if (childNodeIds)
            {
                childNodeIds += count;
            }
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
    BeginMode3D(mCamera);
    rlPushMatrix();
    mSceneRoot->apply();
    rlPopMatrix();

    if (*app->getGridPtr())
    {
        DrawGrid(10, 1.0f);
    }
    EndMode3D();
}

void Scene::update()
{
    if (mActionChanges)
    {
        while (mActionChangePointer < mActionPointer)
        {
            redo(true);
            mActionChangePointer++;
        }
        mActionChanges = false;
    }
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

void Scene::cutNodes(Node **nodes, int count)
{
    if (count == 0)
    {
        return;
    }

    Action firstAction = {
        .type = ACTION_NOP,
    };
    Action *lastAction = nullptr;
    for (int i = 0; i < count; i++)
    {
        Node *target = nodes[i];
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

        if (firstAction.type == ACTION_NOP)
        {
            firstAction = deleteNodeAction;
            lastAction = &firstAction;
        }
        else
        {
            lastAction->next = (Action *)malloc(sizeof(Action));
            memcpy(lastAction->next, &deleteNodeAction, sizeof(Action));
            lastAction = lastAction->next;
        }
    }

    pushAction(firstAction);
}

void Scene::pasteStart()
{
    mPasteFirst.type = ACTION_NOP;
    mPasteLast = nullptr;
}

void Scene::pasteAddNode(const char *type, void *data, Node *parent, int index)
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

    if (mPasteFirst.type == ACTION_NOP)
    {
        mPasteFirst = newNodeAction;
        mPasteLast = &mPasteFirst;
    }
    else
    {
        mPasteLast->next = (Action *)malloc(sizeof(Action));
        memcpy(mPasteLast->next, &newNodeAction, sizeof(Action));
        mPasteLast = mPasteLast->next;
    }
}

void Scene::pasteCommit()
{
    pushAction(mPasteFirst);
}

bool Scene::canUndo()
{
    return mActionPointer > 0;
}

bool Scene::canRedo()
{
    return mActionPointer < mActionCount;
}

std::string Scene::save()
{
    nlohmann::json scene;
    scene["camera"] = CameraUtil::toJson();
    scene["root"] = mSceneRoot->toJson();
    std::stringstream stream;
    stream << std::setw(4) << scene << std::endl;
    mActionPointerSaved = mActionPointer;
    return stream.str();
}

bool Scene::areChangesUnsaved()
{
    return mActionPointerSaved != mActionPointer;
}