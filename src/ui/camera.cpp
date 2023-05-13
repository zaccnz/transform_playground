#include "ui.h"

#include "app.h"

#include <imgui.h>

namespace UI
{
    void camera()
    {
        if (bCameraOpen && ImGui::Begin("Camera Editor", &bCameraOpen))
        {
            Scene *scene = app->getScene();
            Camera *camera = scene->getCameraPtr();
            float pos[3] = {camera->position.x, camera->position.y, camera->position.z};
            if (ImGui::InputFloat3("Position", pos, "%.2f", 0))
            {
                camera->position.x = pos[0];
                camera->position.y = pos[1];
                camera->position.z = pos[2];
            }

            float target[3] = {camera->target.x, camera->target.y, camera->target.z};
            if (ImGui::InputFloat3("Target", target, "%.2f", 0))
            {
                camera->target.x = pos[0];
                camera->target.y = pos[1];
                camera->target.z = pos[2];
            }
            ImGui::SliderFloat("FOV", &camera->fovy, 1.0, 90.0, "%.2f");

            const char *items[] = {"Perspective", "Orthographic"};
            const char *current_item = items[camera->projection];

            if (ImGui::BeginCombo("Projection", current_item))
            {
                for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                {
                    bool is_selected = (current_item == items[n]);
                    if (ImGui::Selectable(items[n], is_selected))
                    {
                        current_item = items[n];
                        camera->projection = n;
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::Button("Reset"))
            {
                camera->position = (Vector3){5.0f, 5.0f, 5.0f};
                camera->target = (Vector3){0.0f, 0.0f, 0.0f};
                camera->up = (Vector3){0.0f, 1.0f, 0.0f};
                camera->fovy = 45.0f;
                camera->projection = CAMERA_PERSPECTIVE;
            }

            ImGui::End();
        }
    }
}