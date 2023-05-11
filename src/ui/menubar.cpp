#include "ui.h"

#include "app.h"

#include <imgui.h>

#ifdef __APPLE__
#define SHORTCUT_KEY "CMD"
#else
#define SHORTCUT_KEY "CTRL"
#endif

void UI::menubar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Scene", SHORTCUT_KEY "+N"))
            {
                // NOTE: should check for changes
                // (if scene action ptr > 0)
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Load scene", SHORTCUT_KEY "+O"))
            {
                // NOTE: should check for changes
                // (if scene action ptr > 0)
            }
            if (ImGui::MenuItem("Save scene", SHORTCUT_KEY "+S"))
            {
            }
#ifndef __EMSCRIPTEN__
            ImGui::Separator();
            if (ImGui::MenuItem("Exit"))
            {
                app->quit();
            }
#endif
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", SHORTCUT_KEY "+Z"))
            {
            }
            if (ImGui::MenuItem("Redo", SHORTCUT_KEY "+Y"))
            {
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", SHORTCUT_KEY "+X"))
            {
                printf("cut shortcut\n");
            }
            if (ImGui::MenuItem("Copy", SHORTCUT_KEY "+C"))
            {
            }
            if (ImGui::MenuItem("Paste", SHORTCUT_KEY "+V"))
            {
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Scene"))
        {
            ImGui::MenuItem("Add", (const char *)NULL, (bool *)NULL, (bool)false);
            if (ImGui::BeginMenu("Shape"))
            {

                if (ImGui::MenuItem("Cube"))
                {
                }
                if (ImGui::MenuItem("Sphere"))
                {
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Transform"))
            {
                if (ImGui::MenuItem("Translate"))
                {
                }
                if (ImGui::MenuItem("Rotate"))
                {
                }
                if (ImGui::MenuItem("Scale"))
                {
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Matrix Frame"))
            {
            }

            ImGui::Separator();
            ImGui::MenuItem("Editors", (const char *)NULL, (bool *)NULL, (bool)false);

            if (ImGui::MenuItem("Camera"))
            {
                bCameraOpen = true;
            }
            if (ImGui::MenuItem("Scene Tree"))
            {
                bSceneTreeOpen = true;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Debug"))
        {
            ImGui::MenuItem("Show grid", NULL, (bool *)NULL);
            ImGui::MenuItem("Show position", NULL, &bDebugPosition);
            ImGui::MenuItem("Show fps", NULL, &bDebugFps);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About Transform Playground"))
            {
                bAboutOpen = true;
            }
            if (ImGui::MenuItem("Open Source Licenses"))
            {
                bOssOpen = true;
            }
            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();
}