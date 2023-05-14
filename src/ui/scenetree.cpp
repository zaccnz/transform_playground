#include "ui.h"

#include "app.h"
#include "scene.h"
#include "node.h"
#include "nodes/nodes.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace UI
{
    void sceneTreeContextMenu()
    {
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::BeginMenu("Create"))
            {
                const char *type = nodeListUi();
                if (type)
                {
                    ListNode *parent = app->getSceneRoot();
                    app->getScene()->createNode(type, nullptr, parent, parent->getChildCount());
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Paste", nullptr, nullptr, app->clipboardFull()))
            {
                app->paste();
            }
            ImGui::EndPopup();
        }
    }

    void sceneTreeNodeContextMenu(Node *node)
    {
        int selectionSize = app->getSelectedNodeCount();
        if (ImGui::BeginPopupContextItem())
        {
            if (!app->isNodeSelected(node))
            {
                app->selectNode(node);
            }
            if (ImGui::MenuItem(node->isEnabled() ? "Disable" : "Enable"))
            {
                if (selectionSize > 1)
                {
                    for (int i = 0; i < selectionSize; i++)
                    {
                        Node *selectedNode = app->getSelectedNode(i);
                        selectedNode->setEnabled(!selectedNode->isEnabled());
                    }
                }
                else
                {
                    node->setEnabled(!node->isEnabled());
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut"))
            {
                app->cut();
            }
            if (ImGui::MenuItem("Copy"))
            {
                app->copy();
            }
            if (ImGui::MenuItem("Paste", nullptr, nullptr, app->clipboardFull()))
            {
                app->paste();
            }
            ImGui::Separator();
            if (node->editable() && selectionSize <= 1)
            {
                if (ImGui::MenuItem("Edit"))
                {
                    app->selectNode(node);
                    bEditorOpen = true;
                    // editedObject = node;
                    // editorOpen = true;
                }
                ImGui::Separator();
            }
            if (selectionSize <= 1)
            {
                if (ImGui::BeginMenu("Insert Above"))
                {
                    const char *type = nodeListUi();
                    if (type)
                    {
                        ListNode *parent = (ListNode *)node->getParent();
                        int index = parent->indexOfChild(node);
                        app->getScene()->createNode(type, nullptr, parent, index);
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Insert Below"))
                {
                    const char *type = nodeListUi();
                    if (type)
                    {
                        ListNode *parent = (ListNode *)node->getParent();
                        int index = parent->indexOfChild(node);
                        app->getScene()->createNode(type, nullptr, parent, index + 1);
                    }
                    ImGui::EndMenu();
                }
                if (!node->isLeaf())
                {
                    if (ImGui::BeginMenu("Insert First Child"))
                    {
                        const char *type = nodeListUi();
                        if (type)
                        {
                            ListNode *self = (ListNode *)node;
                            app->getScene()->createNode(type, nullptr, self, 0);
                        }
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Insert Last Child"))
                    {
                        const char *type = nodeListUi();
                        if (type)
                        {
                            ListNode *self = (ListNode *)node;
                            app->getScene()->createNode(type, nullptr, self, self->getChildCount());
                        }
                        ImGui::EndMenu();
                    }
                }
                ImGui::Separator();
            }
            if (ImGui::MenuItem("Delete"))
            {
                if (selectionSize > 1)
                {
                    Node **nodes = (Node **)malloc(selectionSize * sizeof(Node *));
                    for (int i = 0; i < selectionSize; i++)
                    {
                        nodes[i] = app->getSelectedNode(i);
                    }
                    app->getScene()->cutNodes(nodes, selectionSize);
                    free(nodes);
                }
                else
                {
                    app->getScene()->deleteNode(node);
                }
            }
            ImGui::EndPopup();
        }
    }

    bool sceneTreeAllowDrop(Node *newParent, int newIndex)
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
        if (newParent == dropPayload->getParent())
        {
            ListNode *parentList = (ListNode *)newParent;
            int oldIndex = parentList->indexOfChild(dropPayload);

            if (newIndex == oldIndex)
            {
                return false;
            }

            if (newIndex == oldIndex + 1)
            {
                return false;
            }
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

        if (!sceneTreeAllowDrop(newParent, index))
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

        if (ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            // even if holding down control, we dont want to select multiple
            app->deselectNode();
            if (node->isLeaf())
            {
                app->selectNode(node);
                UI::bEditorOpen = true;
            }
        }
        else if (ImGui::IsItemClicked())
        {
            if (selected)
            {
                app->deselectNode(node);
            }
            else
            {
                app->selectNode(node);
            }
        }

        if (!node->isLeaf() && sceneTreeAllowDrop(node, ((ListNode *)node)->getChildCount()))
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
                int childCount = list->getChildCount();
                for (int i = 0; i < childCount; i++)
                {
                    ImGui::Dummy(ImVec2{2.0f, 1.0f});
                    sceneTreeCustomTarget((ListNode *)node, i, i == 0, false);
                    sceneTreeNode(list->getChild(i));
                }
                ImGui::Dummy(ImVec2{2.0f, 2.0f});
                sceneTreeCustomTarget((ListNode *)node, childCount, false, true);
            }
            ImGui::TreePop();
        }
    }

    void sceneTree()
    {
        if (bSceneTreeOpen && ImGui::Begin("Scene Tree", &bSceneTreeOpen))
        {
            ImGui::SetWindowSize(ImVec2{500.0, 350.0}, ImGuiCond_FirstUseEver);

            first_offset = 0.0f;
            sceneTreeContextMenu();

            ListNode *root = app->getSceneRoot();
            int childCount = root->getChildCount();
            for (int i = 0; i < childCount; i++)
            {
                ImGui::Dummy(ImVec2{2.0f, 1.0f});
                sceneTreeCustomTarget((ListNode *)root, i, i == 0, false);

                sceneTreeNode(root->getChild(i));
            }
            ImGui::Dummy(ImVec2{2.0f, 2.0f});
            sceneTreeCustomTarget((ListNode *)root, childCount, false, true);

            ImGui::End();
        }
    }
}