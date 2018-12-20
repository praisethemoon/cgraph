
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


struct CGNode* sigmoid_node(struct CGNode* Z){
	// Z
	
	// 1 / X
	struct CGNode* lhs = cg_newDoubleNode(1);
	
	// 1 + X
	struct CGNode* divLHS = cg_newDoubleNode(1);
	
	// -Z
	struct CGNode* divRHS1= cg_newUnOp(CGUOT_MINUS,  Z);
	struct CGNode* divRHS = cg_newUnOp(CGUOT_EXP , divRHS1);
	
	struct CGNode* addNode = cg_newBinOp(CGBOT_ADD, divLHS, divRHS);
	
	struct CGNode* node = cg_newBinOp(CGBOT_DIV, lhs, addNode);
	
	return node;
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

int main5(int argc, char* argv[]){
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

int main6(int argc, char* argv[]){
	struct CGNode* x = cg_newVariable("x");
	
	struct CGNode* add = cg_newBinOp(CGBOT_POW, cg_newUnOp(CGUOT_TRANSPOSE, x), cg_newDoubleNode(3));
	
	struct CGraph* graph = cg_newGraph("test", add);
	
	double value[] = {1.0, 2.0, 3.0, 4.0};
	
	struct CGNode* X = cg_newMatrixNode(2, 2, value);
	
	cg_setVar(graph, "x", X);
	
	struct CGResultNode* res = cg_evalGraph(graph);
	
	printf("%f, %f\n%f, %f\n", cg_getResultMatrixVal(res)->data[0], cg_getResultMatrixVal(res)->data[1], cg_getResultMatrixVal(res)->data[2], cg_getResultMatrixVal(res)->data[3]);
	
	cg_autoDiffGraph(graph);
	
	//cg_freeGraph(graph);
	//free(graph);
	
	return 0;
}


int main7(int argc, char* argv[]){
	
	double value1[] = {
		3, 1, 3,
		1, 5, 9, 
		2, 6, 5,
		1, 1, 1
	};
	
	double value2 []= {
		-1.0, -1.0, 1.0,
	};
	
	double value3[] = {
		1.0, 1.0, 0.0
	};
	
	
	struct CGNode* x = cg_newVariable("x");
	struct CGNode* rhsNode = cg_newVectorNode(3, value2);
	struct CGNode* b = cg_newVectorNode(3, value3);

	struct CGNode* node = cg_newBinOp(CGBOT_MULT, cg_newBinOp(CGBOT_POW, cg_newBinOp(CGBOT_MULT, x, cg_newDoubleNode(4)), cg_newDoubleNode(2)), rhsNode);
	struct CGNode* addNnode = cg_newBinOp(CGBOT_ADD, node, b);
	struct CGraph* graph = cg_newGraph("runMult_MV", addNnode);
	
	struct CGNode* lhsNode = cg_newMatrixNode(4, 3, value1);
		
	struct CGNode* X = lhsNode;
	
	cg_setVar(graph, "x", X);
	
	struct CGResultNode* res = cg_evalGraph(graph);
	
	printf("Result type: %d\n", cg_getResultType(res));
	
	switch(cg_getResultType(res)){
		case CGVT_DOUBLE:
		{
			CGDouble* d = cg_getResultDoubleVal(res);
			printf("current diff %f\n",  d->value);
			break;
		}
		
		case CGVT_VECTOR:
		{
			CGVector* vec = cg_getResultVectorVal(res);
			uint64_t i = 0;
			printf("(");
			for(; i < vec->len; i++){
				printf("%f, ", vec->data[i]);
			}
			printf(")\n");
			break;
		}
		
		case CGVT_MATRIX:
		{
			CGMatrix* m = cg_getResultMatrixVal(res);
			uint64_t i = 0;
			uint64_t j = 0;
			printf("(");
			for(; i < m->rows; i++){
				printf("\n\t");
				for(j = 0; j < m->cols; j++){
					printf("%f, ", m->data[i*m->cols+j]);
				}
			}
			printf(")\n");
			break;
		}
	}
	
	cg_autoDiffGraph(graph);
	
	//cg_freeGraph(graph);
	//free(graph);
	
	return 0;
}


int main8(int argc, char* argv[]){
	
	double value1[] = {
		3, 1, 3,
		1, 5, 9, 
		2, 6, 5,
		1, 1, 1
	};
	
	double value2 []= {
		-1.0, -1.0, 1.0,
		 0.0,  0.0, 0.0,
		 1.0,  1.0, 1.0
	};
	
	double value3[] = {
		1.0, 1.0, 0.0
	};
	
	
	struct CGNode* x = cg_newVariable("x");
	struct CGNode* rhsNode = cg_newMatrixNode(3, 3, value2);
	struct CGNode* b = cg_newVectorNode(3, value3);

	struct CGNode* node = cg_newBinOp(CGBOT_DOT, x, rhsNode);
	//struct CGNode* addNnode = cg_newBinOp(CGBOT_MULT, node, b);
	struct CGraph* graph = cg_newGraph("runMult_MV", node);
	
	struct CGNode* lhsNode = cg_newMatrixNode(4, 3, value1);
		
	struct CGNode* X = lhsNode;
	
	cg_setVar(graph, "x", X);
	
	struct CGResultNode* res = cg_evalGraph(graph);
	
	printf("Result type: %d\n", cg_getResultType(res));
	
	switch(cg_getResultType(res)){
		case CGVT_DOUBLE:
		{
			CGDouble* d = cg_getResultDoubleVal(res);
			printf("current diff %f\n",  d->value);
			break;
		}
		
		case CGVT_VECTOR:
		{
			CGVector* vec = cg_getResultVectorVal(res);
			uint64_t i = 0;
			printf("(");
			for(; i < vec->len; i++){
				printf("%f, ", vec->data[i]);
			}
			printf(")\n");
			break;
		}
		
		case CGVT_MATRIX:
		{
			CGMatrix* m = cg_getResultMatrixVal(res);
			uint64_t i = 0;
			uint64_t j = 0;
			printf("(");
			for(; i < m->rows; i++){
				printf("\n\t");
				for(j = 0; j < m->cols; j++){
					printf("%f, ", m->data[i*m->cols+j]);
				}
			}
			printf(")\n");
			break;
		}
	}
	
	cg_autoDiffGraph(graph);
	
	
	return 0;
}


int main9(int argc, char* argv[]){
	
	double value1[] = {
		1, 2, 3,
		4, 5, 6, 
	};
	
	double value2 []= {
		1.0, 2.0,
		3.0, 4.0,
	};
	
	double value3[] = {
		1.0, 0.0
	};
	
	
	struct CGNode* x = cg_newVariable("x");
	struct CGNode* rhsNode = cg_newMatrixNode(2, 2, value2);
	struct CGNode* b = cg_newVectorNode(2, value3);

	struct CGNode* node = cg_newBinOp(CGBOT_DOT, cg_newUnOp(CGUOT_TRANSPOSE, x), rhsNode);
	struct CGNode* addNnode = cg_newBinOp(CGBOT_ADD, node, b);
	struct CGraph* graph = cg_newGraph("runMult_MV", addNnode);
	
	struct CGNode* lhsNode = cg_newMatrixNode(2, 3, value1);
		
	struct CGNode* X = lhsNode;
	
	cg_setVar(graph, "x", X);
	
	struct CGResultNode* res = cg_evalGraph(graph);
	
	printf("Result type: %d\n", cg_getResultType(res));
	
	switch(cg_getResultType(res)){
		case CGVT_DOUBLE:
		{
			CGDouble* d = cg_getResultDoubleVal(res);
			printf("current diff %f\n",  d->value);
			break;
		}
		
		case CGVT_VECTOR:
		{
			CGVector* vec = cg_getResultVectorVal(res);
			uint64_t i = 0;
			printf("(");
			for(; i < vec->len; i++){
				printf("%f, ", vec->data[i]);
			}
			printf(")\n");
			break;
		}
		
		case CGVT_MATRIX:
		{
			CGMatrix* m = cg_getResultMatrixVal(res);
			uint64_t i = 0;
			uint64_t j = 0;
			printf("(");
			for(; i < m->rows; i++){
				printf("\n\t");
				for(j = 0; j < m->cols; j++){
					printf("%f, ", m->data[i*m->cols+j]);
				}
			}
			printf(")\n");
			break;
		}
	}
	
	cg_autoDiffGraph(graph);
	
	
	return 0;
}


int main10(int argc, char* argv[]){
	
	double value1[] = {
		1, 2, 3,
		4, 5, 6, 
	};
	
	double value2 []= {
		1.0, 2.0,
		3.0, 4.0,
	};
	
	double value3[] = {
		1.0, 0.0
	};
	
	
	struct CGNode* x = cg_newVariable("x");

	struct CGraph* graph = cg_newGraph("runMult_MV", cg_newSumOp(cg_newBinOp(CGBOT_POW, x, cg_newDoubleNode(2)), 1));
	
	struct CGNode* lhsNode = cg_newMatrixNode(2, 3, value1);
		
	struct CGNode* X = lhsNode;
	
	cg_setVar(graph, "x", X);
	
	struct CGResultNode* res = cg_evalGraph(graph);
	
	printf("Result type: %d\n", cg_getResultType(res));
	
	switch(cg_getResultType(res)){
		case CGVT_DOUBLE:
		{
			CGDouble* d = cg_getResultDoubleVal(res);
			printf("current diff %f\n",  d->value);
			break;
		}
		
		case CGVT_VECTOR:
		{
			CGVector* vec = cg_getResultVectorVal(res);
			uint64_t i = 0;
			printf("(");
			for(; i < vec->len; i++){
				printf("%f, ", vec->data[i]);
			}
			printf(")\n");
			break;
		}
		
		case CGVT_MATRIX:
		{
			CGMatrix* m = cg_getResultMatrixVal(res);
			uint64_t i = 0;
			uint64_t j = 0;
			printf("(");
			for(; i < m->rows; i++){
				printf("\n\t");
				for(j = 0; j < m->cols; j++){
					printf("%f, ", m->data[i*m->cols+j]);
				}
			}
			printf(")\n");
			break;
		}
	}
	
	cg_autoDiffGraph(graph);
	
	struct CGNode* dx = cg_getVarDiff(graph, "x");
		
	return 0;
}


int main(int argc, char* argv[]){
	
	double value1[] = {
		1, 2,
		3, 4,
	};
	
	double value2[] = {1, 2};
	
	
	struct CGNode* x = cg_newVariable("x");

	struct CGraph* graph = cg_newGraph("runMult_MV", cg_newBinOp(CGBOT_DOT, x, cg_newVectorNode(2, value2)));
	
	struct CGNode* lhsNode = cg_newMatrixNode(2, 2, value1);
		
	struct CGNode* X = lhsNode;
	
	cg_setVar(graph, "x", X);
	
	struct CGResultNode* res = cg_evalGraph(graph);
	
	printf("Result type: %d\n", cg_getResultType(res));
	
	switch(cg_getResultType(res)){
		case CGVT_DOUBLE:
		{
			CGDouble* d = cg_getResultDoubleVal(res);
			printf("current diff %f\n",  d->value);
			break;
		}
		
		case CGVT_VECTOR:
		{
			CGVector* vec = cg_getResultVectorVal(res);
			uint64_t i = 0;
			printf("(");
			for(; i < vec->len; i++){
				printf("%f, ", vec->data[i]);
			}
			printf(")\n");
			break;
		}
		
		case CGVT_MATRIX:
		{
			CGMatrix* m = cg_getResultMatrixVal(res);
			uint64_t i = 0;
			uint64_t j = 0;
			printf("(");
			for(; i < m->rows; i++){
				printf("\n\t");
				for(j = 0; j < m->cols; j++){
					printf("%f, ", m->data[i*m->cols+j]);
				}
			}
			printf(")\n");
			break;
		}
	}
	
	//cg_autoDiffGraph(graph);
	
	//struct CGNode* dx = cg_getVarDiff(graph, "x");
		
	return 0;
}
