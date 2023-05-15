#include "app.h"

#include "camera.h"
#include "ui.h"

#include <iostream>
#include <nlohmann/json.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#else
#include <nfd.h>
#endif

#ifdef __APPLE__
#define HOTKEY_LEFT KEY_LEFT_SUPER
#define HOTKEY_RIGHT KEY_RIGHT_SUPER
#else
#define HOTKEY_LEFT KEY_LEFT_CONTROL
#define HOTKEY_RIGHT KEY_RIGHT_CONTROL
#endif

App *app = nullptr;

#ifndef __EMSCRIPTEN__
nfdfilteritem_t filterItem[1] = {{"Scene files", "json"}};
#endif

App::App()
{
    app = this;

    const int screenWidth = 1280;
    const int screenHeight = 800;

#ifndef __EMSCRIPTEN__
    NFD_Init();
#endif

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "transform playground");

    SetTargetFPS(60);

    UI::init();
    mScene = new Scene();

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
#ifndef __EMSCRIPTEN__
    NFD_Quit();
#endif
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
    mScene->render();

    UI::render();

    EndDrawing();
}

bool App::update()
{
    mRunning &= !WindowShouldClose();
    mScene->update();

    CameraUtil::update();

#ifndef __EMSCRIPTEN__
    // a bit janky, but emscripten doesn't send key events for control, alt, or command
    // instead, the HTML page listens for these events and updates the booleans.  i mimick
    // this here for desktop.
    if (IsKeyPressed(HOTKEY_LEFT) || IsKeyReleased(HOTKEY_RIGHT))
    {
        mHotkeyDown = true;
        CameraUtil::bControlHeld = true;
    }
    else if (IsKeyReleased(HOTKEY_LEFT) || IsKeyReleased(HOTKEY_RIGHT))
    {
        mHotkeyDown = false;
        CameraUtil::bControlHeld = false;
    }
    if (IsKeyPressed(KEY_LEFT_ALT) || IsKeyPressed(KEY_RIGHT_ALT))
    {
        CameraUtil::bAltHeld = true;
    }
    else if (IsKeyReleased(KEY_LEFT_ALT) || IsKeyReleased(KEY_RIGHT_ALT))
    {
        CameraUtil::bAltHeld = false;
    }
#ifdef __APPLE__
    if (IsKeyPressed(KEY_LEFT_CONTROL) || IsKeyPressed(KEY_RIGHT_CONTROL))
    {
        CameraUtil::bControlHeld = true;
    }
    else if (IsKeyReleased(KEY_LEFT_CONTROL) || IsKeyReleased(KEY_RIGHT_CONTROL))
    {
        CameraUtil::bControlHeld = false;
    }
#endif // __APPLE__
#endif // __EMSCRIPTEN__

    if (mHotkeyDown && IsKeyPressed(KEY_N))
    {
        deselectNode();
        resetScene();
    }
    if (mHotkeyDown && IsKeyPressed(KEY_O))
    {
        deselectNode();
        openFile();
    }
    if (mHotkeyDown && IsKeyPressed(KEY_S))
    {
        deselectNode();
        saveFile();
    }

    if (mHotkeyDown && IsKeyPressed(KEY_Z))
    {
        deselectNode();
        mScene->undo();
    }
    if (mHotkeyDown && IsKeyPressed(KEY_Y))
    {
        deselectNode();
        mScene->redo();
    }
    if (mHotkeyDown && IsKeyPressed(KEY_X))
    {
        cut();
        deselectNode();
    }
    if (mHotkeyDown && IsKeyPressed(KEY_C))
    {
        copy();
    }
    if (mHotkeyDown && IsKeyPressed(KEY_V))
    {
        paste();
    }

    if (mHotkeyDown && IsKeyPressed(KEY_ONE))
    {
        UI::bSceneTreeOpen = true;
    }
    if (mHotkeyDown && IsKeyPressed(KEY_TWO))
    {
        UI::bCameraOpen = true;
    }

    return mRunning;
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
#ifndef __EMSCRIPTEN__
    nfdchar_t *outPath;
    nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, NULL);
    if (result == NFD_OKAY)
    {
        delete mScene;
        char *content = LoadFileText(outPath);
        mScene = new Scene(content);
        free(content);
        NFD_FreePath(outPath);
    }
    else if (result != NFD_CANCEL)
    {
        printf("Error opening file: %s\n", NFD_GetError());
    }
#endif
}

void App::saveFile()
{
#ifndef __EMSCRIPTEN__
    nfdchar_t *outPath;
    nfdresult_t result = NFD_SaveDialog(&outPath, filterItem, 1, NULL, "scene.json");
    if (result == NFD_OKAY)
    {
        std::string content = mScene->save();
        SaveFileText(outPath, (char *)content.c_str());
        NFD_FreePath(outPath);
    }
    else if (result != NFD_CANCEL)
    {
        printf("Error saving file: %s\n", NFD_GetError());
    }
#endif
}

void App::resetScene()
{
    delete mScene;
    mScene = new Scene();
}

void App::selectNode(Node *node)
{
    if (mHotkeyDown)
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
        if (!mHotkeyDown)
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
    }

    void hotkey_down(int hotkey)
    {
        if (hotkey == 0)
        {
            // control
            app->setHotkey(true);
            CameraUtil::bControlHeld = true;
        }
        else if (hotkey == 1)
        {
            // alt
            CameraUtil::bAltHeld = true;
        }
        else if (hotkey == 2)
        {
            // super (command on macOS)
            app->setHotkey(true);
        }
    }

    void hotkey_up(int hotkey)
    {
        if (hotkey == 0)
        {
            // control
            app->setHotkey(false);
            CameraUtil::bControlHeld = false;
        }
        else if (hotkey == 1)
        {
            // alt
            CameraUtil::bAltHeld = false;
        }
        else if (hotkey == 2)
        {
            // super (command on macOS)
            app->setHotkey(false);
        }
    }
}
#endif