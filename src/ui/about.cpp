#include "ui.h"

#include <raylib.h>
#include <fmt/core.h>
#include <imgui.h>
#include <nlohmann/json.hpp>

namespace UI
{
    void helpScenes()
    {
        if (ImGui::CollapsingHeader("Scenes"))
        {
            ImGui::TextWrapped("Scenes are stored in the JSON format");
            ImGui::TextWrapped("Feel free to take a look, but please do not edit them youselves unless you know what you are doing.  I did not code in that much error handling lol.");
        }
    }

    void helpCamera()
    {
        if (ImGui::CollapsingHeader("Camera"))
        {
            ImGui::TextWrapped("The camera can be manipulated using the mouse and keyboard, or using the camera editor.");
            ImGui::TextWrapped("Controls:\n\
   - Dragging the mouse while holding alt will look around the scene\n\
   - Dragging the mouse while holding control will rotate the camera around the point it is observing\n\
   - Scrolling in and out will zoom in and out, respectively\n\
   - W,A,S,D move the camera forwards, left, backwards and right\n\
   - Space will raise the camera\n\
   - Shift will lower the camera");
        }
    }

    void helpSceneTree()
    {
        if (ImGui::CollapsingHeader("Scene Tree"))
        {
            ImGui::TextWrapped("The scene tree shows you all of the nodes (transformations, matrices and primitives) in your scene.\
Nodes can be selected individually, or in groups by holding down Control or Command.  Selected nodes have yellow titles.  Nodes can\
also be enabled or disabled by using the checkbox.");

            ImGui::TextWrapped("Double clicking a list node will allow you to view its children.  Double clicking a leaf node will bring up the node editor.");

            ImGui::TextWrapped("Nodes can be re-arranged using drag and drop.  You can only move one node at a time.  Child nodes will be moved with the parent.");

            ImGui::TextWrapped("When pasting nodes, they will appear either as the first child of a selected list node, or after the selected node.");

            ImGui::TextWrapped("Right click a node (or a node in a selected group) to view more options.");
        }
    }

    void about()
    {
        if (bAboutOpen && ImGui::Begin("About", &bAboutOpen))
        {
            ImGui::SetWindowSize(ImVec2{600.0, 500.0}, ImGuiCond_FirstUseEver);

            ImGui::SetWindowFontScale(1.5);
            ImGui::Text("Transform Playground by Zac Cleveland");
            ImGui::SetWindowFontScale(1);
            ImGui::Text("Written in C++ using Raylib (including RLGL) and ImGui.");
            ImGui::Text("Developed for UoA Course COMPSCI 373.");
            ImGui::Separator();
            ImGui::SetWindowFontScale(1.5);
            ImGui::Text("Help");
            ImGui::SetWindowFontScale(1.0);
            helpScenes();
            helpCamera();
            helpSceneTree();
            ImGui::Separator();
            ImGui::SetWindowFontScale(1.5);
            ImGui::Text("Libraries used");
            ImGui::SetWindowFontScale(1);

            ImGui::Text("Raylib (version %s)", RAYLIB_VERSION);
            ImGui::Text("rlImGui");
            ImGui::Text("Dear ImGui (version %s)", ImGui::GetVersion());
            ImGui::Text("fmt (version %d.%d.%d)", FMT_VERSION / 10000, FMT_VERSION / 100 % 100, FMT_VERSION % 100);
            ImGui::Text("nlohmann json (version %d.%d.%d)", NLOHMANN_JSON_VERSION_MAJOR, NLOHMANN_JSON_VERSION_MINOR, NLOHMANN_JSON_VERSION_PATCH);

            ImGui::End();
        }
    }
}