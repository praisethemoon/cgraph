#ifndef CGRAPH_DIFF_H
#define CGRAPH_DIFF_H

#include "cgraph.h"
#include "cg_enums.h"

CGNode* differentiateNodeWRTVar(CGNode* node, CGraph* graph, const char*);
CGraph* differentiateGraphWRTVar(CGraph* graph, char* newName, const char* wrtNode);

CGNode* optimizeNode(CGNode* node, CGraph* graph);
void optimizeGraph(CGraph* graph);

#endif
