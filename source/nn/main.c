
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

int main1(int argc, char* argv[]){
	double value[] ={0.5, 1, 0.5};
	struct CGNode* var = cg_newVectorNode(3, value);
	struct CGraph* graph = sigmoid();
	cg_setVar(graph, "Z", var);

	struct CGResultNode* res = cg_evalGraph(graph);
	
	printf("%f\n", cg_getResultVectorVal(res)->data[0]);
	
	//cg_freeGraph(graph);
	//free(graph);
	
	return 0;
}

int main2(int argc, char* argv[]){
	struct CGNode* x = cg_newVariable("x");
	struct CGNode* y = cg_newVariable("y");
	struct CGNode* z = cg_newVariable("z");
	
	struct CGNode* add = cg_newBinOp(CGBOT_ADD, x, y);
	struct CGNode* mult = cg_newBinOp(CGBOT_MULT, add, z);
	
	struct CGraph* graph = cg_newGraph("test", mult);
	
	struct CGNode* X = cg_newDoubleNode(-2.0);
	struct CGNode* Y = cg_newDoubleNode( 5.0);
	struct CGNode* Z = cg_newDoubleNode(-4.0);
	
	cg_setVar(graph, "x", X);
	cg_setVar(graph, "y", Y);
	cg_setVar(graph, "z", Z);
	
	
	struct CGResultNode* res = cg_evalGraph(graph);
	
	printf("%f\n", cg_getResultDoubleVal(res)->value);
	
	cg_autoDiffGraph(graph);
	
	//cg_freeGraph(graph);
	//free(graph);
	
	return 0;
}

int main3(int argc, char* argv[]){
	struct CGNode* x = cg_newVariable("x");
	
	struct CGNode* add = cg_newBinOp(CGBOT_ADD, x, x);
	struct CGNode* add2 = cg_newBinOp(CGBOT_DIV, add, cg_newDoubleNode(3.0));
	
	struct CGraph* graph = cg_newGraph("test", add2);
	
	struct CGNode* X = cg_newDoubleNode(-2.0);
	
	cg_setVar(graph, "x", X);
	
	
	struct CGResultNode* res = cg_evalGraph(graph);
	
	printf("%f\n", cg_getResultDoubleVal(res)->value);
	
	cg_autoDiffGraph(graph);
	
	//cg_freeGraph(graph);
	//free(graph);
	
	return 0;
}

int main4(int argc, char* argv[]){
	struct CGNode* x = cg_newVariable("x");
	
	struct CGNode* add = cg_newBinOp(CGBOT_ADD, cg_newBinOp(CGBOT_MULT, x, cg_newDoubleNode(3)), cg_newBinOp(CGBOT_POW, cg_newDoubleNode(3), x));
	
	struct CGraph* graph = cg_newGraph("test", add);
	
	struct CGNode* X = cg_newDoubleNode(2.0);
	
	cg_setVar(graph, "x", X);
	
	struct CGResultNode* res = cg_evalGraph(graph);
	
	printf("%f\n", cg_getResultDoubleVal(res)->value);
	
	cg_autoDiffGraph(graph);
	
	//cg_freeGraph(graph);
	//free(graph);
	
	return 0;
}
int main(int argc, char* argv[]){
	struct CGNode* x = cg_newVariable("x");
	
	struct CGNode* add = cg_newBinOp(CGBOT_ADD, cg_newBinOp(CGBOT_MULT, cg_newUnOp(CGUOT_MINUS, x), cg_newDoubleNode(3)), cg_newBinOp(CGBOT_POW, cg_newDoubleNode(3), x));
	
	struct CGraph* graph = cg_newGraph("test", add);
	
	struct CGNode* X = cg_newDoubleNode(2.0);
	
	cg_setVar(graph, "x", X);
	
	struct CGResultNode* res = cg_evalGraph(graph);
	
	printf("%f\n", cg_getResultDoubleVal(res)->value);
	
	cg_autoDiffGraph(graph);
	
	//cg_freeGraph(graph);
	//free(graph);
	
	return 0;
}
