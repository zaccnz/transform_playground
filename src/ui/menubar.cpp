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
                app->resetScene();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Load scene", SHORTCUT_KEY "+O"))
            {
                if (app->getScene()->areChangesUnsaved())
                {
                    app->promptUnsavedChanges(DO_LOAD_FILE);
                }
                else
                {
                    app->openFile();
                }
            }
            if (ImGui::MenuItem("Save scene", SHORTCUT_KEY "+S"))
            {
                app->saveFile();
            }
#ifndef __EMSCRIPTEN__
            ImGui::Separator();
            if (ImGui::MenuItem("Exit"))
            {
                if (app->getScene()->areChangesUnsaved())
                {
                    app->promptUnsavedChanges(DO_QUIT);
                }
                else
                {
                    app->quit();
                }
            }
#endif
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", SHORTCUT_KEY "+Z"))
            {
                app->getScene()->undo();
            }
            if (ImGui::MenuItem("Redo", SHORTCUT_KEY "+Y"))
            {
                app->getScene()->redo();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", SHORTCUT_KEY "+X"))
            {
                app->cut();
            }
            if (ImGui::MenuItem("Copy", SHORTCUT_KEY "+C"))
            {
                app->copy();
            }
            if (ImGui::MenuItem("Paste", SHORTCUT_KEY "+V", nullptr, app->clipboardFull()))
            {
                app->paste();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Scene"))
        {
            ImGui::MenuItem("Add", (const char *)NULL, (bool *)NULL, (bool)false);
            const char *type = nodeListUi();
            if (type)
            {
                app->createNode(type);
            }

            ImGui::Separator();
            ImGui::MenuItem("Editors", (const char *)NULL, (bool *)NULL, (bool)false);

            if (ImGui::MenuItem("Scene Tree", SHORTCUT_KEY "+1"))
            {
                bSceneTreeOpen = true;
            }
            if (ImGui::MenuItem("Camera", SHORTCUT_KEY "+2"))
            {
                bCameraOpen = true;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Debug"))
        {
            ImGui::MenuItem("Show grid", NULL, app->getGridPtr());
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