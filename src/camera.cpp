#include "camera.h"

#include "app.h"

void updateCamera()
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

    UpdateCameraPro(app->getScene()->getCameraPtr(), movement, rotation, -mouseWheel);

    if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))
    {
        // figure out orbit
    }
}