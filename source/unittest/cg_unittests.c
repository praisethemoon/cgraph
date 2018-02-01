
/** \file cg_unittests.h
 *  \brief Unit tests of every functionality offered by the library
 */


#include <stdio.h>
#include <stdint.h>

#include "cg_api.h"
#include "cg_enums.h"
#include "cg_types.h"

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

MU_TEST(runMult_MvM){
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

MU_TEST(runCross_VV){
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
	
	struct CGraph* graph = cg_newGraph("runCross_VV", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_VECTOR(result);
	
	CGVector* v = cg_getResultVectorVal(result);
	ASSERT_VECTOR_DIM(v, 3);
	
	double gt[] = {-9, -1, 2.5};
	
	ASSERT_VECTOR_EQ(gt, v);
}

MU_TEST(runDot_VV){
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
	
	struct CGraph* graph = cg_newGraph("runDot_VV", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_DOUBLE(result);
	
	CGDouble* d = cg_getResultDoubleVal(result);
	
	mu_assert_double_eq(3*-3+1*-1+5*0.5, d->value);
}


MU_TEST(runDiv_dd){
	double value1 = 1;
	
	double value2 = 2;
	
	
	struct CGNode* lhsNode = cg_newDoubleNode(value1);
	struct CGNode* rhsNode = cg_newDoubleNode(value2);
	struct CGNode* node = cg_newBinOp(CGBOT_DIV, lhsNode, rhsNode);
	
	struct CGraph* graph = cg_newGraph("runDiv_dd", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_DOUBLE(result);
	
	CGDouble* Y = cg_getResultDoubleVal(result);

	double gt = 0.5;
	
	mu_assert_double_eq(gt, Y->value);
}

MU_TEST(runDiv_Vd){
	double value1[] = {
		3,
		1, 
		5
	};
	
	double value2 = 2;
	
	
	struct CGNode* lhsNode = cg_newVectorNode(3, value1);
	struct CGNode* rhsNode = cg_newDoubleNode(value2);
	struct CGNode* node = cg_newBinOp(CGBOT_DIV, lhsNode, rhsNode);
	
	struct CGraph* graph = cg_newGraph("runDiv_Vd", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_VECTOR(result);
	
	CGVector* Y = cg_getResultVectorVal(result);

	double gt[] = {1.5, 0.5, 2.5};
	
	ASSERT_VECTOR_EQ(gt, Y);
}


MU_TEST(runDiv_Md){
	double value1[] = {
		3, 1, 3,
		1, 5, 9, 
		2, 6, 5,
		1, 1, 1
	};
	
	double value2 = 0.5;
	
	struct CGNode* lhsNode = cg_newMatrixNode(4, 3, value1);
	struct CGNode* rhsNode = cg_newDoubleNode(value2);

	struct CGNode* node = cg_newBinOp(CGBOT_DIV, lhsNode, rhsNode);
	
	struct CGraph* graph = cg_newGraph("runDiv_Md", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_MATRIX(result);
	
	CGMatrix* M = cg_getResultMatrixVal(result);

	double gt[] = {6, 2, 6, 2, 10, 18, 4, 12, 10, 2, 2, 2};
	
	ASSERT_MATRIX_EQ(gt, M);
}


MU_TEST(runAdd_dd){
	double value1 = 3;
	double value2 = 2;
	
	
	struct CGNode* lhsNode = cg_newDoubleNode(value1);
	struct CGNode* rhsNode = cg_newDoubleNode(value2);
	struct CGNode* node = cg_newBinOp(CGBOT_ADD, lhsNode, rhsNode);
	
	struct CGraph* graph = cg_newGraph("runAdd_dd", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_DOUBLE(result);
	
	CGDouble* Y = cg_getResultDoubleVal(result);
	
	double gt = 5;
	
	mu_assert_double_eq(5, Y->value);
}

MU_TEST(runAdd_Vd){
	double value1[] = {
		3,
		1, 
		5
	};
	
	double value2 = 2;
	
	
	struct CGNode* lhsNode = cg_newVectorNode(3, value1);
	struct CGNode* rhsNode = cg_newDoubleNode(value2);
	struct CGNode* node = cg_newBinOp(CGBOT_ADD, lhsNode, rhsNode);
	
	struct CGraph* graph = cg_newGraph("runAdd_Vd", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_VECTOR(result);
	
	CGVector* Y = cg_getResultVectorVal(result);
	
	double gt[] = {5, 3, 7};
	
	ASSERT_VECTOR_EQ(gt, Y);
}

MU_TEST(runAdd_Md){
	double value1[] = {
		3, 1, 3,
		1, 5, 9, 
		2, 6, 5,
		1, 1, 1
	};
	
	double value2 = 0.5;
	
	struct CGNode* lhsNode = cg_newMatrixNode(4, 3, value1);
	struct CGNode* rhsNode = cg_newDoubleNode(value2);

	struct CGNode* node = cg_newBinOp(CGBOT_ADD, lhsNode, rhsNode);
	
	
	struct CGraph* graph = cg_newGraph("runDiv_Md", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	

	CHECK_ERROR(result);
	ASSERT_MATRIX(result);
	
	CGMatrix* M = cg_getResultMatrixVal(result);
	
	double gt[] = {3.5, 1.5, 3.5, 1.5, 5.5, 9.5, 2.5, 6.5, 5.5, 1.5, 1.5, 1.5};
	
	ASSERT_MATRIX_EQ(gt, M);
}


MU_TEST(runAdd_MV){
	double value1[] = {
		3, 1, 3,
		1, 5, 9, 
		2, 6, 5,
		1, 1, 1
	};
	
	double value2 []= {
		-1.0, 0.0, 1.0, 0.5
	};
	
	struct CGNode* lhsNode = cg_newMatrixNode(4, 3, value1);
	struct CGNode* rhsNode = cg_newVectorNode(4, value2);
	struct CGNode* node = cg_newBinOp(CGBOT_ADD, lhsNode, rhsNode);
	
	struct CGraph* graph = cg_newGraph("runAdd_MV", node);
	struct CGResultNode* result = cg_evalGraph(graph);

	CHECK_ERROR(result);
	ASSERT_MATRIX(result);
	
	CGMatrix* M = cg_getResultMatrixVal(result);
	
	double gt[] = {2, 0, 2, 1, 5, 9, 3, 7, 6, 1.5, 1.5, 1.5};
	
	ASSERT_MATRIX_EQ(gt, M);
}

MU_TEST(runAdd_VV){
	double value1[] = {
		3,
		1, 
		5
	};
	
	double value2[] = {
		-3,
		-1, 
		1
	};
	
	
	struct CGNode* lhsNode = cg_newVectorNode(3, value1);
	struct CGNode* rhsNode = cg_newVectorNode(3, value2);
	struct CGNode* node = cg_newBinOp(CGBOT_ADD, lhsNode, rhsNode);
	
	
	struct CGraph* graph = cg_newGraph("runAdd_VV", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_VECTOR(result);
	
	CGVector* V = cg_getResultVectorVal(result);
	
	double gt[] = {0, 0, 6};
	
	ASSERT_VECTOR_EQ(gt, V);
}

MU_TEST(runAdd_MM){
	double value1[] = {
		3, 1, 3, 1,
		1, 5, 9, 1,
		2, 6, 5, 1,
		1, 1, 1, 0,
	};
	
	double value2[] = {
		-2, -1, -3, -1,
		-1, -4, -9, -1,
		-2, -6, -4, -1,
		-1, -1, -1, 1,
	};
	
	
	struct CGNode* lhsNode = cg_newMatrixNode(4, 4, value1);
	struct CGNode* rhsNode = cg_newMatrixNode(4, 4, value2);

	struct CGNode* node = cg_newBinOp(CGBOT_ADD, lhsNode, rhsNode);

	struct CGraph* graph = cg_newGraph("runAdd_MM", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_MATRIX(result);
	
	CGMatrix* M = cg_getResultMatrixVal(result);
	
	double gt[] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
	
	ASSERT_MATRIX_EQ(gt, M);
}

MU_TEST(runSub_dd){
	double value1 = 3;
	double value2 = 2;
	
	
	struct CGNode* lhsNode = cg_newDoubleNode(value1);
	struct CGNode* rhsNode = cg_newDoubleNode(value2);
	struct CGNode* node = cg_newBinOp(CGBOT_SUB, lhsNode, rhsNode);
	
	
	struct CGraph* graph = cg_newGraph("runSub_dd", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_DOUBLE(result);
	
	CGDouble* D = cg_getResultDoubleVal(result);
	
	double gt = 1;
	
	mu_assert_double_eq(gt, D->value);
}

MU_TEST(runSub_Vd){
	double value1[] = {
		3,
		1, 
		5
	};
	
	double value2 = 2;
	
	struct CGNode* lhsNode = cg_newVectorNode(3, value1);
	struct CGNode* rhsNode = cg_newDoubleNode(value2);
	struct CGNode* node = cg_newBinOp(CGBOT_SUB, lhsNode, rhsNode);
	
	struct CGraph* graph = cg_newGraph("runSub_Vd", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	
	CHECK_ERROR(result);
	ASSERT_VECTOR(result);
	
	CGVector* V = cg_getResultVectorVal(result);
	
	double gt[] = {1, -1, 3};
	
	ASSERT_VECTOR_EQ(gt, V);
}

MU_TEST(runSub_Md){
	double value1[] = {
		3, 1, 3,
		1, 5, 9, 
		2, 6, 5,
		1, 1, 1
	};
	
	double value2 = 0.5;
	
	struct CGNode* lhsNode = cg_newMatrixNode(4, 3, value1);
	struct CGNode* rhsNode = cg_newDoubleNode(value2);

	struct CGNode* node = cg_newBinOp(CGBOT_SUB, lhsNode, rhsNode);
	
	struct CGraph* graph = cg_newGraph("runSub_Md", node);
	struct CGResultNode* result = cg_evalGraph(graph);

	CHECK_ERROR(result);
	ASSERT_MATRIX(result);
	
	CGMatrix* M = cg_getResultMatrixVal(result);
	
	double gt[] = {2.5, 0.5, 2.5, 0.5, 4.5, 8.5, 1.5, 5.5, 4.5, 0.5, 0.5, 0.5};
	
	ASSERT_MATRIX_EQ(gt, M);
}


MU_TEST(runSub_VV){
	double value1[] = {
		3,
		1, 
		5
	};
	
	double value2[] = {
		-3,
		-1, 
		1
	};
	
	
	struct CGNode* lhsNode = cg_newVectorNode(3, value1);
	struct CGNode* rhsNode = cg_newVectorNode(3, value2);
	struct CGNode* node = cg_newBinOp(CGBOT_SUB, lhsNode, rhsNode);
	
	struct CGraph* graph = cg_newGraph("runSub_VV", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_VECTOR(result);
	
	CGVector* V = cg_getResultVectorVal(result);
	
	double gt[] = {6, 2, 4};
	
	ASSERT_VECTOR_EQ(gt, V);
}

MU_TEST(runSub_MM){
	double value1[] = {
		3, 1, 3, 1,
		1, 5, 9, 1,
		2, 6, 5, 1,
		1, 1, 1, 0,
	};
	
	double value2[] = {
		3, 1, 3, 1,
		1, 5, 9, 1,
		2, 6, 5, 1,
		1, 1, 1, 0,
	};
	
	
	struct CGNode* lhsNode = cg_newMatrixNode(4, 4, value1);
	struct CGNode* rhsNode = cg_newMatrixNode(4, 4, value2);

	struct CGNode* node = cg_newBinOp(CGBOT_SUB, lhsNode, rhsNode);
	
	
	struct CGraph* graph = cg_newGraph("runSub_MM", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_MATRIX(result);
	
	CGMatrix* M = cg_getResultMatrixVal(result);
	
	double gt[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	
	ASSERT_MATRIX_EQ(gt, M);
}

MU_TEST(runExp_M){
	double value1[] = {
		3, 1, 3, 1,
		1, 0, 9, 1,
		2, 6, 5, 1,
		1, 1, 1, 0,
	};
	
	
	struct CGNode* uhsNode = cg_newMatrixNode(4, 4, value1);
	struct CGNode* node = cg_newUnOp(CGUOT_EXP, uhsNode);
	
	struct CGraph* graph = cg_newGraph("runExp_M", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_MATRIX(result);
	
	CGMatrix* M = cg_getResultMatrixVal(result);
	
	double gt[16];
	
	uint64_t i = 0;
	for(; i < 16; i++){
		gt[i] = exp(value1[i]);
	}
	
	ASSERT_MATRIX_EQ(gt, M);
}


MU_TEST(runExpLog_M){
	double value1[] = {
		3, 1, 3, 1,
		1, 0, 9, 1,
		2, 6, 5, 1,
		1, 1, 1, 0,
	};
	
	
	struct CGNode* uhsNode = cg_newMatrixNode(4, 4, value1);

	struct CGNode* node1 = cg_newUnOp(CGUOT_EXP, uhsNode);
	struct CGNode* node2 = cg_newUnOp(CGUOT_LOG, node1);
	
	struct CGraph* graph = cg_newGraph("runExpLog_M", node2);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_MATRIX(result);
	
	CGMatrix* M = cg_getResultMatrixVal(result);
	
	ASSERT_MATRIX_EQ(value1, M);
}

MU_TEST(runT_M){
	double value1[] = {
		3, 1, 3, 1, 0,
		1, 0, 9, 1, 0.5,
		2, 6, 5, 1, 0.3,
		1, 1, 1, 0, 1.1
	};
	
	
	struct CGNode* uhsNode = cg_newMatrixNode(4, 5, value1);

	struct CGNode* node = cg_newUnOp(CGUOT_TRANSPOSE, uhsNode);
	
	
	struct CGraph* graph = cg_newGraph("runT_M", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_MATRIX(result);
	
	CGMatrix* M = cg_getResultMatrixVal(result);
	
	double gt[] = {3, 1, 2, 1, 1, 0, 6, 1, 3, 9, 5, 1, 1, 1, 1, 0, 0, 0.5, 0.3, 1.1};
	
	mu_assert_int_eq(5, M->rows);
	mu_assert_int_eq(4, M->cols);
	ASSERT_MATRIX_EQ(gt, M);
}

/*
 * the following function will fail on purpose,
 * it should fail with an error of invalid operation div for (matrix, vector)
 */
MU_TEST(runDiv_MV_FAIL){
	double value1[] = {
		3, 1, 3,
		1, 5, 9, 
		2, 6, 5,
		1, 1, 1
	};
	
	double value2[]= {
		-1.0, -1.0, 1.0,
	};
	
	struct CGNode* lhsNode = cg_newMatrixNode(4, 3, value1);
	struct CGNode* rhsNode = cg_newVectorNode(3, value2);

	struct CGNode* node = cg_newBinOp(CGBOT_DIV, lhsNode, rhsNode);
	
	struct CGraph* graph = cg_newGraph("runDiv_MV_FAIL", node);
	
	struct CGResultNode* result = cg_evalGraph(graph);
	struct CGError* error = cg_getResultError(result);
	
	mu_assert_int_eq(CGET_INCOMPATIBLE_ARGS_EXCEPTION, cg_getErrorType(error));
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
	MU_RUN_TEST(runDiv_dd);
	MU_RUN_TEST(runDiv_Vd);
	MU_RUN_TEST(runDiv_Md);
	
	MU_RUN_TEST(runAdd_dd);
	MU_RUN_TEST(runAdd_Vd);
	MU_RUN_TEST(runAdd_Md);
	MU_RUN_TEST(runAdd_MV);
	MU_RUN_TEST(runAdd_VV);
	MU_RUN_TEST(runAdd_MM);
	MU_RUN_TEST(runSub_dd);
	MU_RUN_TEST(runSub_Vd);
	MU_RUN_TEST(runSub_Md);
	MU_RUN_TEST(runSub_VV);
	MU_RUN_TEST(runSub_MM);
	MU_RUN_TEST(runExp_M);
	MU_RUN_TEST(runExpLog_M);
	MU_RUN_TEST(runT_M);
	
	MU_RUN_TEST(runDiv_MV_FAIL);
}

void runAllTests(){
	MU_RUN_SUITE(node_ops);
	MU_REPORT();
}
