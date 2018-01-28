#ifndef CGRAPH_DIFF_H
#define CGRAPH_DIFF_H

#include "cgraph.h"

CGNode* diff_node(CGNode* node, CGraph* graph, const char*);
CGraph* graph_diff(CGraph* graph, char* newName, const char* wrtNode);

#endif
