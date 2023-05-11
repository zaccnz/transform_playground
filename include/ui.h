#pragma once

namespace UI
{
    extern bool bAboutOpen;
    extern bool bCameraOpen;
    extern bool bDebugFps;
    extern bool bDebugPosition;
    extern bool bOssOpen;
    extern bool bSceneTreeOpen;

    void init();
    void deinit();

    void render();

    // UI elements
    void debug();     // ui.cpp
    void menubar();   // menubar.cpp
    void sceneTree(); // scenetree.cpp
}