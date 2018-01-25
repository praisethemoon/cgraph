
#include <stdlib.h>
#include <stdio.h>

#include "cg_diff.h"
#include "cgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"
#include "cg_factory.h"
#include "cg_constants.h"

#include "memory.h"


CGNode* diff_node(CGNode* node, CGraph* graph){
	// TODO
	
	return NULL;
}

CGraph* graph_diff(CGraph* graph, char* newName, char* rtNode){
	CGraph* diff = makeGraph(newName);
	diff->root = node_diff(graph->root, graph);
	
	return diff;
}
