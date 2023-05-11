#include <nodes/transforms.h>

#include <rlgl.h>

void RotateNode::apply()
{
    if (!mEnabled)
        return;
    rlRotatef(angle, x, y, z);
}

void RotateNode::uiEditor()
{
}

void ScaleNode::apply()
{
    if (!mEnabled)
        return;
    rlScalef(x, y, z);
}

void ScaleNode::uiEditor()
{
}

void TranslateNode::apply()
{
    if (!mEnabled)
        return;
    rlTranslatef(x, y, z);
}

void TranslateNode::uiEditor()
{
}