#include "ui.h"

#include "app.h"

#include <imgui.h>
#include <rlImGui.h>

namespace UI
{
    bool bAboutOpen = false;
    bool bCameraOpen = false;
    bool bDebugFps = true;
    bool bDebugPosition = true;
    bool bEditorOpen = false;
    bool bOssOpen = false;
    bool bPromptUnsavedChanges = false;
    bool bSceneTreeOpen = true;

    std::string sPromptUnsavedChanges = "";

    void init()
    {
        rlImGuiSetup(true);
    }

    void deinit()
    {
        rlImGuiShutdown();
    }

    void render()
    {
        rlImGuiBegin();

        menubar();
        sceneTree();
        camera();
        editor();

        oss();
        about();

        promptUnsavedChanges();
        debug();

        rlImGuiEnd();
    }

    void debug()
    {
        int infoWidth = 180, infoHeight = 30, border = 10, vpad = 20;
        ImVec2 pos = ImGui::GetWindowSize();
        pos.x -= border + infoWidth;
        pos.y = border;

        ImGuiIO &io = ImGui::GetIO();
        if (bDebugPosition)
        {
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - (border + infoWidth), vpad + border));
            ImGui::SetNextWindowSize(ImVec2(infoWidth, infoHeight));
            ImGui::Begin("InfoPos", &bDebugPosition,
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
            float x, y, z;
            Camera *cam = app->getScene()->getCameraPtr();
            x = cam->position.x;
            y = cam->position.y;
            z = cam->position.z;
            ImGui::Text("Pos (%.2f,%.2f,%.2f)", x, y, z);
            ImGui::End();
            vpad += border + infoHeight;
        }

        if (bDebugFps)
        {
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - (border + infoWidth), vpad + border));
            ImGui::SetNextWindowSize(ImVec2(infoWidth, infoHeight));
            ImGui::Begin("InfoFPS", &bDebugFps,
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
            ImGui::Text("%.1f fps (%.3f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
            ImGui::End();
        }
    }

    void promptUnsavedChanges()
    {
        if (bPromptUnsavedChanges && ImGui::Begin("Confirm", &bPromptUnsavedChanges, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings))
        {
            float width = 300.0, height = 120.0;
            float pos_x = (GetScreenWidth() - width) / 2.0, pos_y = (GetScreenHeight() - height) / 2.0;

            ImGui::SetWindowSize(ImVec2{width, height}, ImGuiCond_Always);
            ImGui::SetWindowPos(ImVec2(pos_x, pos_y), ImGuiCond_Once);

            ImGui::TextWrapped("You have unsaved changes, which will be lost.  Are you sure you would like to %s?", sPromptUnsavedChanges.c_str());
            if (ImGui::Button("Cancel"))
            {
                bPromptUnsavedChanges = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Continue"))
            {
                app->continueUnsavedChanges();
            }
            ImGui::End();
        }
    }

}