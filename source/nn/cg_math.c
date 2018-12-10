/*
 * Neural Networks Library
 */

#include <stdio.h>
#include <inttypes.h>

#include "cg_api.h"
#include "cg_enums.h"
#include "cg_types.h"

struct CGraph* sigmoid(){
	struct CGNode* Z = cg_newVariable("Z");
	struct CGNode* lhs = cg_newDoubleNode(1);
	struct CGNode* divLHS = cg_newDoubleNode(1);
	struct CGNode* divRHS1= cg_newUnOp(CGUOT_MINUS,  Z);
	struct CGNode* divRHS = cg_newUnOp(CGUOT_EXP , divRHS1);
	struct CGNode* addNode = cg_newBinOp(CGBOT_ADD, divLHS, divRHS);
	struct CGNode* node = cg_newBinOp(CGBOT_DIV, lhs, addNode);
	struct CGraph* graph = cg_newGraph("sigmoid", node);
	
	return graph;
}

