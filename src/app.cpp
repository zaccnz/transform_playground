#include "app.h"

#include "ui.h"

#include <iostream>
#include <nlohmann/json.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#ifdef __APPLE__
#define HOTKEY_LEFT KEY_LEFT_SUPER
#define HOTKEY_RIGHT KEY_RIGHT_SUPER
#else
#define HOTKEY_LEFT KEY_LEFT_CONTROL
#define HOTKEY_RIGHT KEY_RIGHT_CONTROL
#endif

App *app = nullptr;

App::App()
{
    app = this;

    const int screenWidth = 1280;
    const int screenHeight = 800;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "transform playground");

    SetTargetFPS(60);

    UI::init();
    mScene = new Scene();

    mCamera.position = (Vector3){5.0f, 5.0f, 5.0f};
    mCamera.target = (Vector3){0.0f, 0.0f, 0.0f};
    mCamera.up = (Vector3){0.0f, 1.0f, 0.0f};
    mCamera.fovy = 45.0f;
    mCamera.projection = CAMERA_PERSPECTIVE;

#ifdef __EMSCRIPTEN__
    emscripten_run_script("setApplicationReady();");
#endif
}

App::~App()
{
    freeClipboard();
    delete mScene;
    UI::deinit();
    CloseWindow();
}

void App::freeClipboard()
{
    if (!mClipboard)
    {
        return;
    }

    for (int i = 0; i < mClipboardCount; i++)
    {
        free(mClipboard[i]);
    }
    free(mClipboard);
    mClipboard = nullptr;
}

void App::render()
{

    BeginDrawing();
    ClearBackground(DARKGRAY);
    BeginMode3D(mCamera);
    mScene->render();
    if (mGrid)
    {
        DrawGrid(10, 1.0f);
    }
    EndMode3D();

    UI::render();

    EndDrawing();
}

bool App::update()
{
    mScene->update();

    bool isHotkeyDown = IsKeyDown(HOTKEY_LEFT) || IsKeyDown(HOTKEY_RIGHT);

    if (isHotkeyDown && IsKeyPressed(KEY_N))
    {
        deselectNode();
        resetScene();
    }
    if (isHotkeyDown && IsKeyPressed(KEY_O))
    {
        deselectNode();
        openFile();
    }
    if (isHotkeyDown && IsKeyPressed(KEY_S))
    {
        deselectNode();
        saveFile();
    }

    if (isHotkeyDown && IsKeyPressed(KEY_Z))
    {
        deselectNode();
        mScene->undo();
    }
    if (isHotkeyDown && IsKeyPressed(KEY_Y))
    {
        deselectNode();
        mScene->redo();
    }
    if (isHotkeyDown && IsKeyPressed(KEY_X))
    {
        cut();
        deselectNode();
    }
    if (isHotkeyDown && IsKeyPressed(KEY_C))
    {
        copy();
    }
    if (isHotkeyDown && IsKeyPressed(KEY_V))
    {
        paste();
    }

    if (isHotkeyDown && IsKeyPressed(KEY_ONE))
    {
        UI::bSceneTreeOpen = true;
    }
    if (isHotkeyDown && IsKeyPressed(KEY_TWO))
    {
        UI::bCameraOpen = true;
    }

    return !WindowShouldClose() && mRunning;
}

void App::cut()
{
    copy();
    if (!*mSelectedNodes || !mClipboardCount)
    {
        return;
    }
    mScene->cutNodes(mSelectedNodes, mClipboardCount);
}

void App::copy()
{
    if (!*mSelectedNodes)
    {
        return;
    }
    freeClipboard();
    int i = 0;
    while (mSelectedNodes[i] && i < MAX_SELECTED_NODES)
    {
        i++;
    }
    mClipboard = (void **)malloc(sizeof(void *) * i);
    mClipboardCount = i;

    for (int i = 0; i < mClipboardCount; i++)
    {
        Node *node = mSelectedNodes[i];
        int size;
        void *data = node->toData(&size);
        mClipboard[i] = malloc(sizeof(const char *) + sizeof(int) + size);
        *(const char **)mClipboard[i] = node->getName();
        *(int *)(((char *)mClipboard[i]) + sizeof(const char *)) = size;
        memcpy(((char *)mClipboard[i]) + sizeof(const char *) + sizeof(int), data, size);
        free(data);
    }
}

void App::paste()
{
    if (!mClipboard || mClipboardCount == 0)
    {
        return;
    }
    mScene->pasteStart();
    int i = 0;
    if (!*mSelectedNodes)
    {
        for (int j = 0; j < mClipboardCount; j++)
        {
            const char *type = *(const char **)mClipboard[j];
            int size = *(int *)(((char *)mClipboard[j]) + sizeof(const char *));
            void *data = malloc(size);
            memcpy(data, (((char *)mClipboard[j]) + sizeof(const char *) + sizeof(int)), size);
            mScene->pasteAddNode(type, data, mScene->getSceneRoot(), mScene->getSceneRoot()->getChildCount() + j);
        }
        mScene->pasteCommit();
        return;
    }
    while (mSelectedNodes[i] && i < MAX_SELECTED_NODES)
    {
        Node *pasteTarget = mSelectedNodes[i];
        ListNode *parent = nullptr;
        int index = 0;
        if (pasteTarget->isLeaf())
        {
            parent = (ListNode *)pasteTarget->getParent();
            index = parent->indexOfChild(pasteTarget) + 1;
        }
        else
        {
            parent = (ListNode *)pasteTarget;
            index = parent->getChildCount();
        }
        for (int j = 0; j < mClipboardCount; j++)
        {
            const char *type = *(const char **)mClipboard[j];
            int size = *(int *)(((char *)mClipboard[j]) + sizeof(const char *));
            void *data = malloc(size);
            memcpy(data, (((char *)mClipboard[j]) + sizeof(const char *) + sizeof(int)), size);
            mScene->pasteAddNode(type, data, parent, index);
        }
        i++;
    }
    mScene->pasteCommit();
}

void App::promptUnsavedChanges(AfterUnsavedChanges after)
{
    mAfterUnsavedChanges = after;
    UI::bPromptUnsavedChanges = true;
    switch (after)
    {
    case DO_LOAD_FILE:
    {
        UI::sPromptUnsavedChanges = "load a new scene";
        break;
    }
    case DO_QUIT:
    {
        UI::sPromptUnsavedChanges = "quit";
        break;
    }
    default:
    {
        printf("invalid unsaved change action %d\n", mAfterUnsavedChanges);
        break;
    }
    }
}

void App::continueUnsavedChanges()
{
    switch (mAfterUnsavedChanges)
    {
    case DO_LOAD_FILE:
    {
        openFile();
        break;
    }
    case DO_QUIT:
    {
        quit();
        break;
    }
    default:
    {
        printf("invalid unsaved change action %d\n", mAfterUnsavedChanges);
        break;
    }
    }
}

void App::openFile()
{
}

void App::saveFile()
{
}

void App::cameraPos(float *x, float *y, float *z)
{
    Vector3 pos = mCamera.position;
    *x = pos.x;
    *y = pos.y;
    *z = pos.z;
}

void App::resetScene()
{
    delete mScene;
    mScene = new Scene();
}

void App::selectNode(Node *node)
{
    bool isHotkeyDown = IsKeyDown(HOTKEY_LEFT) || IsKeyDown(HOTKEY_RIGHT);

    if (isHotkeyDown)
    {
        int i = 0;
        while (mSelectedNodes[i] && i < MAX_SELECTED_NODES - 1)
        {
            i++;
        }
        if (i == MAX_SELECTED_NODES - 1)
        {
            return;
        }
        mSelectedNodes[i] = node;
        mSelectedNodes[i + 1] = nullptr;
    }
    else
    {
        mSelectedNodes[0] = node;
        mSelectedNodes[1] = nullptr;
    }
}

void App::deselectNode(Node *node)
{
    if (node)
    {
        // if we are not holding control or command, we want to change the
        // selection to be this node (unless we are already selecting it).  otherwise, deselect it
        bool isHotkeyDown = IsKeyDown(HOTKEY_LEFT) || IsKeyDown(HOTKEY_RIGHT);
        if (!isHotkeyDown)
        {
            if (mSelectedNodes[0] == node)
            {
                node = nullptr;
            }
            mSelectedNodes[0] = node;
            mSelectedNodes[1] = nullptr;
            return;
        }
        if (!isNodeSelected(node))
        {
            return;
        }
        int i;
        for (i = 0; mSelectedNodes[i] != node; i++)
        {
        }
        while (mSelectedNodes[i] && i < MAX_SELECTED_NODES)
        {
            mSelectedNodes[i] = mSelectedNodes[i + 1];
            i++;
        }
    }
    else
    {
        mSelectedNodes[0] = nullptr;
    }
}

bool App::isNodeSelected(Node *node)
{
    int i = 0;
    while (mSelectedNodes[i] && i < MAX_SELECTED_NODES)
    {
        if (mSelectedNodes[i] == node)
        {
            return true;
        }
        i++;
    }
    return false;
}

int App::getSelectedNodeCount()
{
    int i = 0;
    while (mSelectedNodes[i] && i < MAX_SELECTED_NODES)
    {
        i++;
    }
    return i;
}

void App::createNode(const char *type)
{
    if (!*mSelectedNodes)
    {
        ListNode *root = mScene->getSceneRoot();
        mScene->createNode(type, nullptr, root, root->getChildCount());
    }
    else
    {
        int i = 0;
        while (mSelectedNodes[i] && i < MAX_SELECTED_NODES)
        {
            Node *selected = mSelectedNodes[i];
            if (selected->isLeaf())
            {
                ListNode *parent = (ListNode *)selected->getParent();
                int index = parent->indexOfChild(selected);
                mScene->createNode(type, nullptr, parent, index + 1);
            }
            else
            {
                ListNode *selectedList = (ListNode *)selected;
                mScene->createNode(type, nullptr, selectedList, selectedList->getChildCount());
            }
        }
    }
}

#ifdef __EMSCRIPTEN__
extern "C"
{
    void resize_event(int width, int height)
    {
        SetWindowSize(width, height);
        printf("resized window to %d %d\n", width, height);
    }
}
#endif