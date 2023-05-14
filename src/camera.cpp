#include "camera.h"

#include "app.h"

#include <raymath.h>
#include <nlohmann/json.hpp>

namespace CameraUtil
{
    void jsonWriteVector3(nlohmann::json &json, Vector3 data)
    {
        json[0] = data.x;
        json[1] = data.y;
        json[2] = data.z;
    }

    void jsonReadVector3(nlohmann::json &json, Vector3 *data)
    {
        data->x = json[0].get<float>();
        data->y = json[1].get<float>();
        data->z = json[2].get<float>();
    }

    void jsonWriteProjection(nlohmann::json &json, int proj)
    {
        if (proj == 0)
        {
            json = "perspective";
        }
        else
        {
            json = "orthographic";
        }
    }

    void jsonReadProjection(nlohmann::json &json, int *proj)
    {
        std::string projection = json.get<std::string>();
        if (projection.compare("perspective") == 0)
        {
            *proj = 0;
        }
        else
        {
            *proj = 1;
        }
    }

    void fromJson(nlohmann::json &camJson, Camera *camera)
    {
        jsonReadVector3(camJson["position"], &camera->position);
        jsonReadVector3(camJson["target"], &camera->target);
        camera->fovy = camJson["fovy"].get<float>();
        jsonReadProjection(camJson["projection"], &camera->projection);
    }

    nlohmann::json toJson()
    {
        Camera *camera = app->getScene()->getCameraPtr();
        nlohmann::json camJson;

        jsonWriteVector3(camJson["position"], camera->position);
        jsonWriteVector3(camJson["target"], camera->target);
        camJson["fovy"] = camera->fovy;

        jsonWriteProjection(camJson["projection"], camera->projection);

        return camJson;
    }

    void update()
    {

        Vector3 movement = {0.0};
        Vector3 rotation = {0.0};
        if (IsKeyDown(KEY_W))
        {
            movement.x = 0.1;
        }
        if (IsKeyDown(KEY_S))
        {
            movement.x -= 0.1;
        }
        if (IsKeyDown(KEY_A))
        {
            movement.y -= 0.1;
        }
        if (IsKeyDown(KEY_D))
        {
            movement.y += 0.1;
        }
        if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
        {
            movement.z -= 0.1;
        }
        if (IsKeyDown(KEY_SPACE))
        {
            movement.z += 0.1;
        }

        Vector2 mouseDelta = GetMouseDelta();
        if (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT))
        {
            rotation.x = mouseDelta.x;
            rotation.y = mouseDelta.y;
        }

        if (IsKeyPressed(KEY_LEFT_ALT) || IsKeyPressed(KEY_RIGHT_ALT))
        {
            DisableCursor();
        }
        if (IsKeyReleased(KEY_LEFT_ALT) || IsKeyReleased(KEY_RIGHT_ALT))
        {
            EnableCursor();
        }

        int mouseWheel = GetMouseWheelMove();

        Camera *camera = app->getScene()->getCameraPtr();
        UpdateCameraPro(camera, movement, rotation, -mouseWheel);

        if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))
        {
            Vector3 focus = Vector3Subtract(camera->position, camera->target);
            Vector3 right = Vector3CrossProduct(Vector3Subtract(camera->position, camera->target),
                                                Vector3Subtract(camera->up, camera->position));

            float yaw = -mouseDelta.x * 0.05;
            float pitch = mouseDelta.y * 0.05;

            Matrix rotMatYaw = MatrixRotate(camera->up, yaw);
            Matrix rotMatPitch = MatrixRotate(right, pitch);
            Vector3 newFocus = Vector3Transform(focus, rotMatYaw);
            newFocus = Vector3Transform(newFocus, rotMatPitch);
            if (newFocus.x < 1.0 && newFocus.x > -1.0 && newFocus.z < 1.0 && newFocus.z > -1.0)
            {
                newFocus.x = newFocus.x < 0.0 ? -1.0 : 1.0;
                newFocus.z = newFocus.z < 0.0 ? -1.0 : 1.0;
                return;
            }
            camera->position = Vector3Add(camera->target, newFocus);
        }
    }
}