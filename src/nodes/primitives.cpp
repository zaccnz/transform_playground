#include <nodes/primitives.h>

#include <raylib.h>
#include <rlgl.h>

#include <iostream>

Color colorFromFloat(float r, float g, float b)
{
    auto c = Color{
        (unsigned char)(r * 255.0f),
        (unsigned char)(g * 255.0f),
        (unsigned char)(b * 255.0f),
        255,
    };

    return c;
}

void PrimitiveNode::uiEditor()
{
    // TODO: render colour picker
}

void CubeNode::apply()
{
    if (!mEnabled)
        return;
    DrawCube({0, 0, 0}, 1.0f, 1.0f, 1.0f, colorFromFloat(r, g, b));
}

void CubeNode::uiEditor()
{
    PrimitiveNode::uiEditor();
}