
#include <stdio.h>
#include <inttypes.h>

#include "cg_api.h"
#include "cg_enums.h"
#include "cg_types.h"

struct CGraph* sigmoid(){
	// Z
	struct CGNode* Z = cg_newVariable("Z");
	
	// 1 / X
	struct CGNode* lhs = cg_newDoubleNode(1);
	
	// 1 + X
	struct CGNode* divLHS = cg_newDoubleNode(1);
	
	// -Z
	struct CGNode* divRHS1= cg_newUnOp(CGUOT_MINUS,  Z);
	struct CGNode* divRHS = cg_newUnOp(CGUOT_EXP , divRHS1);
	
	struct CGNode* addNode = cg_newBinOp(CGBOT_ADD, divLHS, divRHS);
	
	struct CGNode* node = cg_newBinOp(CGBOT_DIV, lhs, addNode);
	
	struct CGraph* graph = cg_newGraph("sigmoid", node);
	
	return graph;
}

int main(int argc, char* argv[]){
	double value =0;
	struct CGNode* var = cg_newDoubleNode(value);
	struct CGraph* graph = sigmoid();
	cg_setVar(graph, "Z", var);

	struct CGResultNode* res = cg_evalGraph(graph);
	
	printf("%f\n", cg_getResultDoubleVal(res)->value);
	
	cg_freeGraph(graph);
	
	return 0;
}
