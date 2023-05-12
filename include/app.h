#include "node.h"
#include "scene.h"

#include <raylib.h>

#define MAX_SELECTED_NODES 128

enum AfterUnsavedChanges
{
    DO_NOP = 0,
    DO_LOAD_FILE,
    DO_QUIT,
    DO_MAX,
};

class App
{
private:
    bool mRunning = true;
    Camera mCamera = {0};
    Scene *mScene = nullptr;
    Node *mSelectedNodes[MAX_SELECTED_NODES] = {nullptr};
    void **mClipboard = nullptr;
    int mClipboardCount = 0;
    AfterUnsavedChanges mAfterUnsavedChanges;
    bool mGrid = true;

    void freeClipboard();

public:
    App();
    ~App();

    void render();
    bool update();

    void cut();
    void copy();
    void paste();

    void promptUnsavedChanges(AfterUnsavedChanges after);
    void continueUnsavedChanges();
    void openFile();
    void saveFile();

    void cameraPos(float *x, float *y, float *z);
    void resetScene();

    void selectNode(Node *node);
    void deselectNode(Node *node = nullptr);
    bool isNodeSelected(Node *node);

    void quit() { mRunning = false; }
    Scene *getScene() { return mScene; }
    ListNode *getSceneRoot() { return mScene->getSceneRoot(); }
    bool *getGridPtr() { return &mGrid; }
};

extern App *app;