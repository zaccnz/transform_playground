#pragma once

#include "nodes/lists.h"
#include "nodes/primitives.h"
#include "nodes/transforms.h"

// draw the submenu list of nodes.
// returns nullptr if no node selected, otherwise type of node
const char *nodeListUi();
