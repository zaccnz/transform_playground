#pragma once

#include <string>

namespace UI
{
    extern bool bAboutOpen;
    extern bool bCameraOpen;
    extern bool bDebugFps;
    extern bool bDebugPosition;
    extern bool bEditorOpen;
    extern bool bOssOpen;
    extern bool bPromptUnsavedChanges;
    extern bool bSceneTreeOpen;

    extern std::string sPromptUnsavedChanges;

    void init();
    void deinit();

    void render();

    // UI elements
    void debug();                // ui.cpp
    void promptUnsavedChanges(); // ui.cpp
    void about();                // about.cpp
    void camera();               // camera.cpp
    void editor();               // editor.cpp
    void menubar();              // menubar.cpp
    void oss();                  // oss.cpp
    void sceneTree();            // scenetree.cpp
}