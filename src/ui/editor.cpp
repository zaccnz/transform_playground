#include "ui.h"

#include "app.h"

#include <imgui.h>

namespace UI
{
    Node *editorLastNode = nullptr;
    void *editorInitialData = nullptr;
    bool editorWasSaved = true;

    void stopLastEdits()
    {
        if (editorInitialData)
        {
            if (editorLastNode && !editorWasSaved)
            {
                editorLastNode->setData(editorInitialData);
            }
            free(editorInitialData);
        }
        editorInitialData = nullptr;
        editorLastNode = nullptr;
        editorWasSaved = true;
    }

    void editor()
    {
        if (bEditorOpen && ImGui::Begin("Node Editor", &bEditorOpen))
        {
            ImGui::SetWindowSize(ImVec2{280.0, 175.0}, ImGuiCond_FirstUseEver);

            int selectedNodeCount = app->getSelectedNodeCount();
            if (selectedNodeCount == 0)
            {
                stopLastEdits();
                ImGui::Text("No node selected");
            }
            else if (selectedNodeCount > 1)
            {
                stopLastEdits();
                ImGui::Text("You can only edit one node at a time");
            }
            else
            {
                Node *node = app->getSelectedNode();
                if (node != editorLastNode)
                {
                    stopLastEdits();
                    editorInitialData = node->toData(nullptr);
                    editorLastNode = node;
                }

                ImGui::Text("Editing %s", node->getName());
                if (node->uiEditor())
                {
                    editorWasSaved = false;
                }

                if (ImGui::Button("Cancel"))
                {
                    bEditorOpen = false;
                    node->setData(editorInitialData);
                    free(editorInitialData);
                    editorInitialData = nullptr;
                    editorLastNode = nullptr;
                }
                ImGui::SameLine();
                if (ImGui::Button("Save"))
                {
                    editorWasSaved = true;
                    bEditorOpen = false;
                    void *newData = node->toData(nullptr);
                    app->getScene()->editNode(node, editorInitialData, newData);
                    editorInitialData = nullptr;
                    editorLastNode = nullptr;
                }
            }

            ImGui::End();
        }
    }
}