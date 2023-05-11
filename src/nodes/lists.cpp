#include "nodes/lists.h"

#include <rlgl.h>

void MatrixFrameNode::enter()
{
    rlPushMatrix();
}

void MatrixFrameNode::exit()
{
    rlPopMatrix();
}