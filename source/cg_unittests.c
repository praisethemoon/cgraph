
/** \file cg_unittests.h
 *  \brief Unit tests of every functionality offered by the library
 */


#include <stdio.h>
#include <stdint.h>

#include "cg_api.h"
#include "cg_enums.h"
#include "cg_types.h"

#include "memory.h"
#include "minunit.h"

#define CHECK_ERROR(result) mu_check(cg_getResultError(result) == NULL);

#define ASSERT_DOUBLE(result) mu_check(cg_getResultType(result) == CGVT_DOUBLE);
#define ASSERT_VECTOR(result) mu_check(cg_getResultType(result) == CGVT_VECTOR);
#define ASSERT_MATRIX(result) mu_check(cg_getResultType(result) == CGVT_MATRIX);

#define ASSERT_VECTOR_DIM(v, l) mu_check(v->len == l);
#define ASSERT_MATRIX_DIM(m, r, c)\
	mu_check(m->rows == r);\
	mu_check(m->cols == c);

#define ASSERT_VECTOR_EQ(v1, v2)\
{\
	uint64_t i = 0;\
	\
	for(;i < v2->len;i++){\
		mu_assert_double_eq(v1[i], v2->data[i]);\
	}\
}

#define ASSERT_MATRIX_EQ(m1, m2)\
{\
	uint64_t i = 0;\
	\
	for(;i < m2->rows*m2->cols;i++){\
		mu_assert_double_eq(m1[i], m2->data[i]);\
	}\
}

MU_TEST(runMult_MV){
	double value1[] = {
		3, 1, 3,
		1, 5, 9, 
		2, 6, 5,
		1, 1, 1
	};
	
	double value2 []= {
		-1.0, -1.0, 1.0,
	};
	
	struct CGNode* lhsNode = cg_newMatrixNode(4, 3, value1);
	struct CGNode* rhsNode = cg_newVectorNode(3, value2);

	struct CGNode* node = cg_newBinOp(CGBOT_MULT, lhsNode, rhsNode);
	
	struct CGraph* graph = cg_newGraph("runMult_MV", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_VECTOR(result);
	
	CGVector* v = cg_getResultVectorVal(result);
	ASSERT_VECTOR_DIM(v, 4);
	
	double gt[] = {-1, 3, -3, -1};
	
	ASSERT_VECTOR_EQ(gt, v);
}


MU_TEST(runMult_MM){
	double value1[] = {
		1, 2, 3,
		4, 5, 6
	};
	
	double value2 []= {
		7, 8,
		9, 10,
		11, 12
	};
	
	struct CGNode* lhsNode = cg_newMatrixNode(2, 3, value1);
	struct CGNode* rhsNode = cg_newMatrixNode(3, 2, value2);

	struct CGNode* node = cg_newBinOp(CGBOT_MULT, lhsNode, rhsNode);
	struct CGraph* graph = cg_newGraph("runMult_MM", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_MATRIX(result);
	
	CGMatrix* m = cg_getResultMatrixVal(result);
	ASSERT_MATRIX_DIM(m, 2, 2);
	double gt[] = {58, 64, 139, 154};
	ASSERT_MATRIX_EQ(gt, m);
	
}

MU_TEST(runMult_Md){
	double value1[] = {
		1, 2, 3,
		4, 5, 6
	};
	
	double value2 = -0.5;
	
	struct CGNode* lhsNode = cg_newMatrixNode(2, 3, value1);
	struct CGNode* rhsNode = cg_newDoubleNode(value2);

	struct CGNode* node = cg_newBinOp(CGBOT_MULT, lhsNode, rhsNode);
	
	struct CGraph* graph = cg_newGraph("runMult_Md", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_MATRIX(result);
	
	CGMatrix* m = cg_getResultMatrixVal(result);
	ASSERT_MATRIX_DIM(m, 2, 3);
	double gt[] = {-0.5, -1, -1.5, -2, -2.5, -3};
	ASSERT_MATRIX_EQ(gt, m);
}


MU_TEST(runMult_dd){
	double value1 = 3.14;
	
	double value2 = 0.5;
	
	struct CGNode* lhsNode = cg_newDoubleNode(value1);
	struct CGNode* rhsNode = cg_newDoubleNode(value2);

	struct CGNode* node = cg_newBinOp(CGBOT_MULT, lhsNode, rhsNode);
	
	struct CGraph* graph = cg_newGraph("runMult_dd", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_DOUBLE(result);
	
	CGDouble* d = cg_getResultDoubleVal(result);
	
	mu_assert_double_eq(3.14*0.5, d->value);
}

MU_TEST(runMult_dV){
	double value1 = 3.14;
	
	double value2 []= {
		-1.0, -1.0, 1.0,
	};
	
	struct CGNode* lhsNode = cg_newDoubleNode(value1);
	struct CGNode* rhsNode = cg_newVectorNode(3, value2);

	struct CGNode* node = cg_newBinOp(CGBOT_MULT, lhsNode, rhsNode);
	
	struct CGraph* graph = cg_newGraph("runMult_dV", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_VECTOR(result);
	
	CGVector* v = cg_getResultVectorVal(result);
	ASSERT_VECTOR_DIM(v, 3);
	
	double gt[] = {-3.14, -3.14, 3.14};
	
	ASSERT_VECTOR_EQ(gt, v);
}

void runMult_MvM(){
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
		1, 2, 3, -1,
		4, 5, 6, 0.5
	};
	
	
	struct CGNode* lhsNode1 = cg_newMatrixNode(4, 3, value1);
	struct CGNode* rhsNode1 = cg_newVectorNode(3, value2);

	struct CGNode* rhsNode2 = cg_newBinOp(CGBOT_MULT, lhsNode1, rhsNode1);
	struct CGNode* lhsNode2 = cg_newMatrixNode(2, 4, value3);
	
	struct CGNode* node = cg_newBinOp(CGBOT_MULT, lhsNode2, rhsNode2);
	
	struct CGraph* graph = cg_newGraph("runMult_MvM", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_VECTOR(result);
	
	CGVector* v = cg_getResultVectorVal(result);
	ASSERT_VECTOR_DIM(v, 2);
	double gt[] = {-3.0, -7.5};
	
	ASSERT_VECTOR_EQ(gt, v);
}

void runCross_VV(){
	double value1[] = {
		3,
		1, 
		5
	};
	
	double value2[] = {
		-3,
		-1, 
		0.5
	};
	
	
	struct CGNode* lhsNode = cg_newVectorNode(3, value1);
	struct CGNode* rhsNode = cg_newVectorNode(3, value2);
	struct CGNode* node = cg_newBinOp(CGBOT_MULT, lhsNode, rhsNode);
	
	struct CGraph* graph = cg_newGraph("runMult_dd", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_VECTOR(result);
	
	CGVector* v = cg_getResultVectorVal(result);
	ASSERT_VECTOR_DIM(v, 3);
	
	double gt[] = {-9, -1, 2.5};
	
	ASSERT_VECTOR_EQ(gt, v);
}

void runDot_VV(){
	double value1[] = {
		3,
		1, 
		5
	};
	
	double value2[] = {
		-3,
		-1, 
		0.5
	};
	
	
	struct CGNode* lhsNode = cg_newVectorNode(3, value1);
	struct CGNode* rhsNode = cg_newVectorNode(3, value2);
	struct CGNode* node = cg_newBinOp(CGBOT_DOT, lhsNode, rhsNode);
	
	struct CGraph* graph = cg_newGraph("runMult_dd", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_DOUBLE(result);
	
	CGDouble* d = cg_getResultDoubleVal(result);
	
	mu_assert_double_eq(3*-3+1*-1+5*0.5, d->value);
}

MU_TEST_SUITE(node_ops) {
	MU_RUN_TEST(runMult_MV);
	MU_RUN_TEST(runMult_MM);
	MU_RUN_TEST(runMult_Md);
	MU_RUN_TEST(runMult_dd);
	MU_RUN_TEST(runMult_dV);
	MU_RUN_TEST(runMult_MvM);
	MU_RUN_TEST(runCross_VV);
	MU_RUN_TEST(runDot_VV);
}

void runAllTests(){
	MU_RUN_SUITE(node_ops);
	MU_REPORT();
}
