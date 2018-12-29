
#include <stdio.h>
#include <inttypes.h>

#include "cg_api.h"
#include "cg_enums.h"
#include "cg_types.h"

struct CGNode* sigmoid_node(struct CGNode* x){
	return cg_newBinOp(CGBOT_DIV, cg_newDoubleNode(1.0), cg_newBinOp(CGBOT_ADD, cg_newDoubleNode(1.0), cg_newUnOp(CGUOT_EXP, cg_newUnOp(CGUOT_MINUS, x))));
}

struct CGNode* softmax_node(struct CGNode* x){
	return cg_newUnOp(CGUOT_TRANSPOSE, cg_newBinOp(CGBOT_DIV, cg_newUnOp(CGUOT_TRANSPOSE, cg_newUnOp(CGUOT_EXP, x)), cg_newAxisBoundOp(CGABOT_SUM, cg_newUnOp(CGUOT_EXP, x), 0)));
}

int main(int argc, char* argv[]){
	
	double x_val[] = {5.1,7.5,0.4,1.2, 0, 1, 5.2, 3.33, 0.01};
	double T1_val[] = {0.21, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
	double b1_val[] = {0.1, 0.1, 0.1, 0.1, 0.1};
	double T2_val[] = {0.3, 0.4, 0.12, 0.14, 0.1};
	double b2_val[] = {0.0};
	double y_val[] = {0, 1};
	
	
	
	struct CGNode* x = cg_newVariable("x");
	struct CGNode* y = cg_newVariable("y");
	struct CGNode* T_1 = cg_newVariable("T_1");
	struct CGNode* b_1 = cg_newVariable("b_1");
	struct CGNode* T_2 = cg_newVariable("T_2");
	struct CGNode* b_2 = cg_newVariable("b_2");
	
	struct CGNode* L1 = sigmoid_node(cg_newBinOp(CGBOT_ADD, cg_newBinOp(CGBOT_DOT, x, T_1), b_1));
	struct CGNode* L2 = sigmoid_node(cg_newBinOp(CGBOT_ADD, cg_newBinOp(CGBOT_DOT, L1, T_2), b_2));
	
	
	struct CGraph* graph = cg_newGraph("nn", softmax_node(L2));
	
	cg_setVar(graph, "x", cg_newMatrixNode(2, 4, x_val));
	cg_setVar(graph, "y", cg_newVectorNode(2, y_val));
	cg_setVar(graph, "T_1", cg_newMatrixNode(4, 5, T1_val));
	cg_setVar(graph, "b_1", cg_newVectorNode(5, b1_val));
	cg_setVar(graph, "T_2", cg_newMatrixNode(5, 1, T2_val));
	cg_setVar(graph, "b_2", cg_newDoubleNode(b2_val[0]));
	
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
	
	//struct CGNode* dx = cg_getVarDiff(graph, "T_2");
		
	return 0;
}

