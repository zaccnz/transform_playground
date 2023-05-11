#include "nodes/nodes.h"

#include <raylib.h>

#define MAX_SELECTED_NODES 128

class App
{
private:
    bool mRunning = true;
    Camera mCamera = {0};
    MatrixFrameNode *mSceneRoot = nullptr;
    Node *selected_nodes[MAX_SELECTED_NODES] = {nullptr};

public:
    App();
    ~App();

    void render();
    bool update();

    void quit() { mRunning = false; }
    void cameraPos(float *x, float *y, float *z);

    void selectNode(Node *node, bool multiple = false);
    void deselectNode(Node *node = nullptr);
    bool isNodeSelected(Node *node);

    MatrixFrameNode *getSceneRoot() { return mSceneRoot; }
};

extern App *app;