#include "app.h"

#include "ui.h"

#include <iostream>
#include <nlohmann/json.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
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

    mSceneRoot = new MatrixFrameNode();
    MatrixFrameNode *frame = new MatrixFrameNode();
    frame->pushChild(new RotateNode(45, 1, 0, 0));
    frame->pushChild(new TranslateNode(2, 0, 0));
    mSceneRoot->pushChild(new ScaleNode(0.5, 2, 1));
    mSceneRoot->pushChild(frame);
    mSceneRoot->pushChild(new CubeNode(1.0, 0.5, 1.0));
    frame->pushChild(new CubeNode(1.0, 1.0, 1.0));

    nlohmann::json json = mSceneRoot->toJson();

    std::cout << std::setw(4) << json << std::endl;

    mSceneRoot->setJson(json);

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
    UI::deinit();
    CloseWindow();
}

void App::render()
{

    BeginDrawing();
    ClearBackground(DARKGRAY);
    BeginMode3D(mCamera);
    mSceneRoot->apply();
    DrawGrid(10, 1.0f);
    EndMode3D();

    UI::render();

    EndDrawing();
}

bool App::update()
{

    return !WindowShouldClose() && mRunning;
}

void App::cameraPos(float *x, float *y, float *z)
{
    Vector3 pos = mCamera.position;
    *x = pos.x;
    *y = pos.y;
    *z = pos.z;
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