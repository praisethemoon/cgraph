#ifndef CGRAPH_DIFF_H
#define CGRAPH_DIFF_H

#include "cgraph.h"
#include "cg_enums.h"

CGNode* optimizeNode(CGNode* node, CGraph* graph);
void optimizeGraph(CGraph* graph);
void autoDifferenciateGraph(CGraph* graph);
void printNode(CGNode* node);

#endif
