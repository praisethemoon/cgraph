
/** \file cg_unittests.h
 *  \brief Unit tests of every functionality offered by the library
 */

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

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

/*
 * Test Matrix vector multiplication broadcast
 */
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
	ASSERT_MATRIX(result);
	
	CGMatrix* m = cg_getResultMatrixVal(result);
	ASSERT_MATRIX_DIM(m, 4, 3);
	
	double gt[] = {-3, -1,  3,
       -1, -5,  9,
       -2, -6,  5,
       -1, -1,  1
	};
	
	ASSERT_MATRIX_EQ(gt, m);
	
	cg_freeGraph(graph); free(graph);
}

/*
 * Test matrix-matrix multiplication broadcast
 */
MU_TEST(runMult_MM){
	double value1[] = {
		1, 2, 3,
		4, 5, 6
	};
	
	double value2 []= {
		1, 2, 3,
		4, 5, 6
	};
	
	struct CGNode* lhsNode = cg_newMatrixNode(2, 3, value1);
	struct CGNode* rhsNode = cg_newMatrixNode(2, 3, value2);

	struct CGNode* node = cg_newBinOp(CGBOT_MULT, lhsNode, rhsNode);
	struct CGraph* graph = cg_newGraph("runMult_MM", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_MATRIX(result);
	
	CGMatrix* m = cg_getResultMatrixVal(result);
	ASSERT_MATRIX_DIM(m, 2, 3);
	double gt[] = {1, 4, 9, 16, 25, 36};
	ASSERT_MATRIX_EQ(gt, m);
	
	cg_freeGraph(graph); free(graph);
}


MU_TEST(runDot_MM){
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

	struct CGNode* node = cg_newBinOp(CGBOT_DOT, lhsNode, rhsNode);
	struct CGraph* graph = cg_newGraph("runDot_MM", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_MATRIX(result);
	
	CGMatrix* m = cg_getResultMatrixVal(result);
	ASSERT_MATRIX_DIM(m, 2, 2);
	double gt[] = {58, 64, 139, 154};
	ASSERT_MATRIX_EQ(gt, m);
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
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
		4, 5, 6, 0.5,
		4, 5, 6, 0.5
	};
	
	
	struct CGNode* lhsNode1 = cg_newMatrixNode(4, 3, value1);
	struct CGNode* rhsNode1 = cg_newVectorNode(3, value2);

	struct CGNode* rhsNode2 = cg_newBinOp(CGBOT_MULT, lhsNode1, rhsNode1);
	struct CGNode* lhsNode2 = cg_newMatrixNode(3, 4, value3);
	
	struct CGNode* node = cg_newBinOp(CGBOT_DOT, lhsNode2, rhsNode2);
	
	struct CGraph* graph = cg_newGraph("runMult_MvM", node);
	struct CGResultNode* result = cg_evalGraph(graph);
	
	CHECK_ERROR(result);
	ASSERT_MATRIX(result);
	
	CGMatrix* m = cg_getResultMatrixVal(result);
	ASSERT_MATRIX_DIM(m, 3, 3);
	double gt[] = { -10. , -28. ,  35. ,
       -29.5, -65.5,  87.5,
       -29.5, -65.5,  87.5 };
	
	ASSERT_MATRIX_EQ(gt, m);
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
}


MU_TEST(runAdd_MV){
	double value1[] = {
		3, 1, 3,
		1, 5, 9, 
		2, 6, 5,
		1, 1, 1
	};
	
	double value2 []= {
		-1.0, 0.0, 1.0
	};
	
	struct CGNode* lhsNode = cg_newMatrixNode(4, 3, value1);
	struct CGNode* rhsNode = cg_newVectorNode(3, value2);
	struct CGNode* node = cg_newBinOp(CGBOT_ADD, lhsNode, rhsNode);
	
	struct CGraph* graph = cg_newGraph("runAdd_MV", node);
	struct CGResultNode* result = cg_evalGraph(graph);

	CHECK_ERROR(result);
	ASSERT_MATRIX(result);
	
	CGMatrix* M = cg_getResultMatrixVal(result);
	
	double gt[] = {
		2, 1, 4,
		0, 5, 10,
		1, 6, 6,
		0, 1, 2
	};
	
	ASSERT_MATRIX_EQ(gt, M);
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
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
	
	cg_freeGraph(graph); free(graph);
}

MU_TEST(diffSimpleNN){
	double x_val[] = {0.2, 0.4};
	double T1_val[] = {0.1, 0.5, -0.3, 0.8};
	
	
	
	struct CGNode* x = cg_newVariable("x");
	struct CGNode* T_1 = cg_newVariable("T_1");
	
	struct CGNode* L2 = cg_newAxisBoundOp(CGABOT_SUM, cg_newBinOp(CGBOT_POW, cg_newBinOp(CGBOT_DOT, x, cg_newUnOp(CGUOT_TRANSPOSE, T_1)), cg_newDoubleNode(2.0)), 0);
	
	
	struct CGraph* graph = cg_newGraph("nn", (L2));
	
	cg_setVar(graph, "x", cg_newVectorNode(2, x_val));
	cg_setVar(graph, "T_1", cg_newMatrixNode(2, 2, T1_val));
	struct CGResultNode* res = cg_evalGraph(graph);
	
	CHECK_ERROR(res);
	ASSERT_DOUBLE(res);
	
	CGDouble* r = cg_getResultDoubleVal(res);
	mu_assert_double_eq(0.116, r->value);
	
	cg_autoDiffGraph(graph);
	
	struct CGNode* dT_1 = cg_getVarDiff(graph, "T_1");
	struct CGResultNode* res1 = cg_constantToResult(dT_1);
	ASSERT_MATRIX(res1);
	CGMatrix* M = cg_getResultMatrixVal(res1);
	
	double gt[] = {	0.088000, 0.176000, 0.104000, 0.208000};
	
	ASSERT_MATRIX_DIM(M, 2, 2);
	ASSERT_MATRIX_EQ(gt, M);
	
	struct CGNode* dx = cg_getVarDiff(graph, "x");
	struct CGResultNode* res2 = cg_constantToResult(dx);
	ASSERT_VECTOR(res2);
	CGVector* V = cg_getResultVectorVal(res2);
	
	double gt2[] = {-0.112000, 0.636000};
	
	ASSERT_VECTOR_DIM(V, 2);
	ASSERT_VECTOR_EQ(gt2, V);
	
	cg_freeGraph(graph); free(graph);
}


MU_TEST(runCrossEntropyLossVec){
	double y_val[] = {0};
	double y_hat_val[] = {0.26980, 0.32235, 0.40784};
	
	
	struct CGNode* y = cg_newVariable("y");
	struct CGNode* y_hat = cg_newVariable("y_hat");
	struct CGNode* CE_LOSS = cg_newCrossEntropyLoss(y_hat, y, 3);
	
	struct CGraph* graph = cg_newGraph("nn", CE_LOSS);

	
	cg_setVar(graph, "y", cg_newVectorNode(1, y_val));
	cg_setVar(graph, "y_hat", cg_newVectorNode(3, y_hat_val));
	
	struct CGResultNode* res = cg_evalGraph(graph);
	
	ASSERT_DOUBLE(res);
	
	mu_assert_double_eq(1.163768, cg_getResultDoubleVal(res)->value);
	
	cg_freeGraph(graph); free(graph);
}
	
struct CGNode* sigmoid_node(struct CGNode* x){
	return cg_newBinOp(CGBOT_DIV, cg_newDoubleNode(1.0), cg_newBinOp(CGBOT_ADD, cg_newDoubleNode(1.0), cg_newUnOp(CGUOT_EXP, cg_newUnOp(CGUOT_MINUS, x))));
}

struct CGNode* softmax_node_tmp(struct CGNode* x){
	return cg_newUnOp(CGUOT_TRANSPOSE, cg_newBinOp(CGBOT_DIV, cg_newUnOp(CGUOT_TRANSPOSE, cg_newUnOp(CGUOT_EXP, x)), cg_newAxisBoundOp(CGABOT_SUM,cg_newUnOp(CGUOT_EXP, x), 0)));
}

MU_TEST(runReluSigmoidSoftmax){
	double x_val[] = {0.1, 0.2, 0.7};
	double T1_val[] = {0.1, 0.4, 0.3, 0.3, 0.7, 0.7,0.5,0.2,0.9 };
	double b1_val[] = {1.0, 1.0, 1.0};
	double T2_val[] =  {0.2, 0.3, 0.5, 0.3, 0.5, 0.7,0.6,0.4,0.8 };
	double b2_val[] = {1.0, 1.0, 1.0};
	double T3_val[] =  {0.1,0.4,0.8,0.3,0.7,0.2,0.5,0.2,0.9 };
	double b3_val[] = {1.0, 1.0, 1.0};
	double y_val[] = {0, 1};
	
	struct CGNode* x = cg_newVariable("x");
	//struct CGNode* y = cg_newVariable("y");
	struct CGNode* T_1 = cg_newVariable("T_1");
	struct CGNode* b_1 = cg_newVariable("b_1");
	struct CGNode* T_2 = cg_newVariable("T_2");
	struct CGNode* b_2 = cg_newVariable("b_2");
	struct CGNode* T_3 = cg_newVariable("T_3");
	struct CGNode* b_3 = cg_newVariable("b_3");
	
	struct CGNode* L1 = cg_newUnOp(CGUOT_RELU, cg_newBinOp(CGBOT_ADD, cg_newBinOp(CGBOT_DOT, x, T_1), b_1));
	struct CGNode* L2 = sigmoid_node(cg_newBinOp(CGBOT_ADD, cg_newBinOp(CGBOT_DOT, L1, T_2), b_2));
	struct CGNode* H  = softmax_node_tmp(cg_newBinOp(CGBOT_ADD, cg_newBinOp(CGBOT_DOT, L2, T_3), b_3));
	
	struct CGraph* graph = cg_newGraph("nn", H);
	
	cg_setVar(graph, "x", cg_newMatrixNode(1, 3, x_val));
	//cg_setVar(graph, "y", cg_newVectorNode(2, y_val));
	cg_setVar(graph, "T_1", cg_newMatrixNode(3, 3, T1_val));
	cg_setVar(graph, "b_1", cg_newVectorNode(3, b1_val));
	cg_setVar(graph, "T_2", cg_newMatrixNode(3, 3, T2_val));
	cg_setVar(graph, "b_2", cg_newVectorNode(3, b2_val));
	cg_setVar(graph, "T_3", cg_newMatrixNode(3, 3, T3_val));
	cg_setVar(graph, "b_3", cg_newVectorNode(3, b3_val));
	
	struct CGResultNode* res = cg_evalGraph(graph);
		
	ASSERT_VECTOR(res);
	
	CGVector* vec = cg_getResultVectorVal(res);
	
	ASSERT_VECTOR_DIM(vec, 3);
	
	double gt[] = {0.198241, 0.285387, 0.516372};
	
	ASSERT_VECTOR_EQ(gt, vec);
	
	cg_freeGraph(graph); free(graph);
}


MU_TEST(runArgMaxMat){
	double value1[] = {
			3, 1, 3, 1, 0,
			1, 0, 9, 1, 0.5,
			2, 6, 5, 1, 0.3,
			1, 1, 1, 0, 1.1
	};


	struct CGNode* uhsNode = cg_newMatrixNode(4, 5, value1);

	struct CGNode* node = cg_newAxisBoundOp(CGABOT_ARGMAX, uhsNode, 1);


	struct CGraph* graph = cg_newGraph("runT_M", node);
	struct CGResultNode* result = cg_evalGraph(graph);

	CHECK_ERROR(result);
	ASSERT_VECTOR(result);

	CGVector* v = cg_getResultVectorVal(result);
	double gt[] = {0, 2, 1, 4};

	ASSERT_VECTOR_DIM(v, 4)
	ASSERT_VECTOR_EQ(gt, v);

	cg_freeGraph(graph); free(graph);
}


MU_TEST(runArgMaxMat2){
    double value1[] = {
            3, 1, 3, 1, 0,
            1, 0, 9, 1, 0.5,
            2, 6, 5, 1, 0.3,
            1, 1, 1, 0, 1.1
    };


    struct CGNode* uhsNode = cg_newMatrixNode(4, 5, value1);

    struct CGNode* node = cg_newAxisBoundOp(CGABOT_ARGMAX, uhsNode, 0);


    struct CGraph* graph = cg_newGraph("runT_M", node);
    struct CGResultNode* result = cg_evalGraph(graph);

    CHECK_ERROR(result);
    ASSERT_VECTOR(result);

    CGVector* v = cg_getResultVectorVal(result);
    double gt[] = {0, 2, 1, 0, 3};

    ASSERT_VECTOR_DIM(v, 5)
    ASSERT_VECTOR_EQ(gt, v);

    cg_freeGraph(graph); free(graph);
}

MU_TEST(runArgMinMat){
	double value1[] = {
			3, 1, 3, 1, 0,
			1, 0, 9, 1, 0.5,
			2, 6, 5, 1, 0.3,
			1, 1, 1, 0, 1.1
	};


	struct CGNode* uhsNode = cg_newMatrixNode(4, 5, value1);

	struct CGNode* node = cg_newAxisBoundOp(CGABOT_ARGMIN, uhsNode, 0);

	struct CGraph* graph = cg_newGraph("runT_M", node);
	struct CGResultNode* result = cg_evalGraph(graph);

	CHECK_ERROR(result);
	ASSERT_VECTOR(result);

	CGVector* v = cg_getResultVectorVal(result);
	double gt[] = {1, 1, 3, 3, 0};

	ASSERT_VECTOR_DIM(v, 5)
	ASSERT_VECTOR_EQ(gt, v);

	cg_freeGraph(graph); free(graph);
}

MU_TEST_SUITE(node_ops) {
	MU_RUN_TEST(runMult_MV);
	MU_RUN_TEST(runMult_MM);
	MU_RUN_TEST(runMult_Md);
	MU_RUN_TEST(runMult_dd);
	MU_RUN_TEST(runMult_dV);
	MU_RUN_TEST(runMult_MvM);
	MU_RUN_TEST(runDot_MM);
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
	MU_RUN_TEST(diffSimpleNN);
	MU_RUN_TEST(runCrossEntropyLossVec);
	MU_RUN_TEST(runReluSigmoidSoftmax);
	
	MU_RUN_TEST(runArgMaxMat);
    MU_RUN_TEST(runArgMaxMat2);
	MU_RUN_TEST(runArgMinMat);
}

void runAllTests(){
	MU_RUN_SUITE(node_ops);
	MU_REPORT();
}
