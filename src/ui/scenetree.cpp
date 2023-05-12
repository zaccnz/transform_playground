#include "ui.h"

#include "app.h"
#include "scene.h"
#include "node.h"
#include "nodes/nodes.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace UI
{
    void sceneTreeNodeContextMenu(Node *node)
    {
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem(node->isEnabled() ? "Disable" : "Enable"))
            {
                node->setEnabled(!node->isEnabled());
            }
            ImGui::Separator();
            if (node->editable())
            {
                if (ImGui::MenuItem("Edit"))
                {
                    // editedObject = node;
                    // editorOpen = true;
                }
                ImGui::Separator();
            }
            if (ImGui::MenuItem("Insert Above"))
            {
            }
            if (ImGui::MenuItem("Insert Below"))
            {
            }
            if (!node->isLeaf())
            {
                if (ImGui::MenuItem("Insert First Child"))
                {
                }
                if (ImGui::MenuItem("Insert Last Child"))
                {
                }
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Delete"))
            {
                app->getScene()->deleteNode(node);
            }
            ImGui::EndPopup();
        }
    }

    bool sceneTreeAllowDrop(Node *newParent)
    {
        const ImGuiPayload *payload = ImGui::GetDragDropPayload();
        if (payload == NULL || !payload->IsDataType("SCENE_TREE_DND"))
        {
            return true;
        }

        Node *dropPayload = *(Node **)payload->Data;
        if (newParent == dropPayload)
        {
            return false;
        }
        if (newParent->isDescendant(dropPayload))
        {
            return false;
        }

        return true;
    }

    // a bit hacky but w/e.
    float first_offset = 0.0f;

    void sceneTreeCustomTarget(ListNode *newParent, int index, bool first, bool last)
    {
        const ImVec2 start = ImGui::GetItemRectMin();
        const float offset = start.x - ImGui::GetWindowPos().x;
        if (first_offset == 0.0f)
        {
            first_offset = offset;
        }
        const ImVec2 end = ImVec2{start.x + ImGui::GetWindowSize().x - offset - first_offset, start.y + 1};

        if (!sceneTreeAllowDrop(newParent))
        {
            return;
        }

        ImVec2 dnd_bound_start = ImVec2{start.x, start.y - (first ? 0 : 10)};
        ImVec2 dnd_bound_end = ImVec2{end.x, start.y + (last ? 0 : 10)};

        ImRect dnd_bounds = ImRect(dnd_bound_start, dnd_bound_end);

        ImGui::PushID(newParent);
        if (ImGui::BeginDragDropTargetCustom(dnd_bounds, ImGui::GetID("custom_dnd_target")))
        {
            ImRect dnd_display = ImRect(start, end);
            ImGui::GetCurrentContext()->DragDropTargetRect = dnd_display;

            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SCENE_TREE_DND"))
            {
                IM_ASSERT(payload->DataSize == sizeof(Node *));
                Node *dropPayload = *(Node **)payload->Data;

                if (dropPayload->getParent() == newParent && index > newParent->indexOfChild(dropPayload))
                {
                    index--;
                }

                app->getScene()->moveNode(dropPayload, newParent, index);
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::PopID();
    }

    void sceneTreeNode(Node *node)
    {
        int flags = ImGuiTreeNodeFlags_AllowItemOverlap;
        flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
        flags |= ImGuiTreeNodeFlags_OpenOnArrow;
        if (node->isLeaf())
            flags |= ImGuiTreeNodeFlags_Leaf;

        ImGui::PushID(node);
        bool enabled = node->isEnabled();
        if (ImGui::Checkbox("##Enabled", &enabled))
        {
            node->setEnabled(enabled);
        }
        ImGui::PopID();
        ImGui::SameLine();
        bool selected = app->isNodeSelected(node);
        if (selected)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{1.0, 1.0, 0.5, 1.0});
        }
        bool open = ImGui::TreeNodeEx(node, flags, "%s", node->getLabel().c_str());
        if (selected)
        {
            ImGui::PopStyleColor(1);
        }

        ImGui::PushID(node);
        sceneTreeNodeContextMenu(node);
        ImGui::PopID();

        if (ImGui::IsItemClicked())
        {
            app->selectNode(node);
        }

        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            // even if holding down control, we dont want to select multiple
            app->deselectNode();
            app->selectNode(node);
            printf("hello world\n");
        }

        if (!node->isLeaf() && sceneTreeAllowDrop(node))
        {
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SCENE_TREE_DND"))
                {
                    IM_ASSERT(payload->DataSize == sizeof(Node *));
                    Node *dropPayload = *(Node **)payload->Data;

                    ListNode *thisNode = (ListNode *)node;
                    app->getScene()->moveNode(dropPayload, thisNode, thisNode->getChildCount());
                }
                ImGui::EndDragDropTarget();
            }
        }

        if (ImGui::BeginDragDropSource())
        {
            bool b = ImGui::SetDragDropPayload("SCENE_TREE_DND", &node, sizeof(Node *));
            ImGui::Text("moving %s (%d)", node->getLabel().c_str(), b);
            ImGui::EndDragDropSource();
        }

        if (open)
        {
            if (!node->isLeaf())
            {
                ListNode *list = (ListNode *)node;
                int child_count = list->getChildCount();
                for (int i = 0; i < child_count; i++)
                {
                    ImGui::Dummy(ImVec2{2.0f, 1.0f});
                    sceneTreeCustomTarget((ListNode *)node, i, i == 0, false);
                    sceneTreeNode(list->getChild(i));
                }
                ImGui::Dummy(ImVec2{2.0f, 2.0f});
                sceneTreeCustomTarget((ListNode *)node, child_count, false, true);
            }
            ImGui::TreePop();
        }
    }

    void sceneTree()
    {
        if (bSceneTreeOpen && ImGui::Begin("Scene Tree", &bSceneTreeOpen))
        {
            first_offset = 0.0f;

            sceneTreeNode(app->getSceneRoot());

            ImGui::End();
        }
    }
}