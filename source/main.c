/*
 * 
 * 
 */

#include <stdlib.h>
#include <stdio.h>

#include "ccgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"
#include "cg_factory.h"

#include "memory.h"

#define PROFILER_DEFINE
#include "profiler.h"

void runMult_MV(){
	printf("Running M.v example\n");
	double value1[] = {
		3, 1, 3,
		1, 5, 9, 
		2, 6, 5,
		1, 1, 1
	};
	
	double value2 []= {
		-1.0, -1.0, 1.0,
	};
	
	CGNode* lhsNode = makeMatrixConstantNode(4, 3, value1);
	CGNode* rhsNode = makeVectorConstantNode(3, value2);

	CGNode* node = makeBinaryOpNode(CGBOT_MULT, lhsNode, rhsNode);
	
	CGResultNode* result = computeCGNode(node);
	CGVector* Y = (CGVector*)result->value;

	printf("result: %d shape: %d\n", result->type, Y->len);
	uint64_t i = 0;
	
	for(;i<Y->len;i++){
		printf("\t%f\n", Y->data[i]);
	}
}

void runMult_MM(){
	printf("Running M.M example\n");
	double value1[] = {
		1, 2, 3,
		4, 5, 6
	};
	
	double value2 []= {
		7, 8,
		9, 10,
		11, 12
	};
	
	CGNode* lhsNode = makeMatrixConstantNode(2, 3, value1);
	CGNode* rhsNode = makeMatrixConstantNode(3, 2, value2);

	CGNode* node = makeBinaryOpNode(CGBOT_MULT, lhsNode, rhsNode);
	
	CGResultNode* result = computeCGNode(node);
	CGMatrix* Y = (CGMatrix*)result->value;

	printf("result: %d shape: (%d, %d)\n", result->type, Y->rows, Y->cols);
	uint64_t i = 0;
	uint64_t j = 0;
	
	for(;i<Y->rows;i++){
		for(j = 0;j<Y->cols;j++){
			printf("\t%f", Y->data[i*Y->cols +j]);
		}
		printf("\n");
	}
}

void runMult_Md(){
	printf("Running M.d example\n");
	double value1[] = {
		1, 2, 3,
		4, 5, 6
	};
	
	double value2 = -0.5;
	
	CGNode* lhsNode = makeMatrixConstantNode(2, 3, value1);
	CGNode* rhsNode = makeDoubleConstantNode(value2);

	CGNode* node = makeBinaryOpNode(CGBOT_MULT, lhsNode, rhsNode);
	
	CGResultNode* result = computeCGNode(node);
	CGMatrix* Y = (CGMatrix*)result->value;

	printf("result: %d shape: (%d, %d)\n", result->type, Y->rows, Y->cols);
	uint64_t i = 0;
	uint64_t j = 0;
	
	for(;i<Y->rows;i++){
		for(j = 0;j<Y->cols;j++){
			printf("\t%f", Y->data[i*Y->cols +j]);
		}
		printf("\n");
	}
}


void runMult_dd(){
	printf("Running d.d example\n");
	double value1 = 3.14;
	
	double value2 = 0.5;
	
	CGNode* lhsNode = makeDoubleConstantNode(value1);
	CGNode* rhsNode = makeDoubleConstantNode(value2);

	CGNode* node = makeBinaryOpNode(CGBOT_MULT, lhsNode, rhsNode);
	
	CGResultNode* result = computeCGNode(node);
	CGDouble* Y = (CGDouble*)result->value;

	printf("result: %d value: %f\n", result->type, Y->value);
}


void runMult_dV(){
	printf("Running d.v example\n");
	double value1 = 3.14;
	
	double value2 []= {
		-1.0, -1.0, 1.0,
	};
	
	CGNode* lhsNode = makeDoubleConstantNode(value1);
	CGNode* rhsNode = makeVectorConstantNode(3, value2);

	CGNode* node = makeBinaryOpNode(CGBOT_MULT, lhsNode, rhsNode);
	
	CGResultNode* result = computeCGNode(node);
	CGVector* Y = (CGVector*)result->value;

	printf("result: %d shape: %d\n", result->type, Y->len);
	uint64_t i = 0;
	
	for(;i<Y->len;i++){
		printf("\t%f\n", Y->data[i]);
	}
}

void runMult_MvM(){
	printf("Running M.(M.v) example\n");
	double value1[] = {
		3, 1, 3,
		1, 5, 9, 
		2, 6, 5,
		1, 1, 1
	};
	
	double value2[]= {
		-1.0, -1.0, 1.0,
	};
	double value3[] = {
		1, 2, 3,
		4, 5, 6
	};
	
	
	CGNode* lhsNode1 = makeMatrixConstantNode(4, 3, value1);
	CGNode* rhsNode1 = makeVectorConstantNode(3, value2);

	CGNode* rhsNode2 = makeBinaryOpNode(CGBOT_MULT, lhsNode1, rhsNode1);
	CGNode* lhsNode2 = makeMatrixConstantNode(2, 3, value3);
	
	CGNode* node = makeBinaryOpNode(CGBOT_MULT, lhsNode2, rhsNode2);
	
	CGResultNode* result = computeCGNode(node);
	CGVector* Y = (CGVector*)result->value;

	printf("result: %d shape: %d\n", result->type, Y->len);
	uint64_t i = 0;
	
	for(;i<Y->len;i++){
		printf("\t%f\n", Y->data[i]);
	}
}

int main(int argc, char *argv[]) {
	profiler_initialize();
	
	PROFILER_START(init_func);
	printf("Initializing\n");
	PROFILER_STOP(init_func);
	
	
	PROFILER_START(runMult_dd_func);
	runMult_dd();
	PROFILER_STOP(runMult_dd_func);
	
	
	PROFILER_START(runMult_dv_func);
	runMult_dV();
	PROFILER_STOP(runMult_dv_func);
	
	PROFILER_START(runMult_MV_func);
	runMult_MV();
	PROFILER_STOP(runMult_MV_func);
	
	PROFILER_START(runMult_MM_func);
	runMult_MM();
	PROFILER_STOP(runMult_MM_func);
	
	PROFILER_START(runMult_MvM_func);
	runMult_MvM();
	PROFILER_STOP(runMult_MvM_func);
	
	PROFILER_START(runMult_Md_func);
	runMult_Md();
	PROFILER_STOP(runMult_Md_func);
	
	profiler_dump_file("stats.txt");
}
