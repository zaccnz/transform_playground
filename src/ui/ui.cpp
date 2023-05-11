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
    bool bOssOpen = true;
    bool bSceneTreeOpen = true;

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
        debug();
        sceneTree();

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
            app->cameraPos(&x, &y, &z);
            ImGui::Text("Pos (%.1f,%.1f,%.1f)", x, y, z);
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

}