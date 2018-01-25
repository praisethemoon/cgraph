#ifndef CGRAPH_DIFF_H
#define CGRAPH_DIFF_H

#include "cgraph.h"

CGNode* diff_node(CGNode* node, CGraph* graph);
CGraph* graph_diff(CGraph* graph, char* newName, char* rtNode);

#endif
