
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "cg_diff.h"
#include "cgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"
#include "cg_factory.h"
#include "cg_constants.h"
#include "cg_errors.h"
#include "cg_enums.h"
#include "cg_math.h"

#include <malloc.h>

uint8_t nodeValueIsZero(CGNode* node){
	assert(node->type == CGNT_CONSTANT);
	
	switch(node->constant->type){
		case CGVT_DOUBLE: {
			double value = ((CGDouble*)node->constant->value)->value;
			
			return value == 0.0;
		}
		
		case CGVT_VECTOR: {
			uint64_t len = ((CGVector*)node->constant->value)->len;
			double* values = ((CGVector*)node->constant->value)->data;
			
			uint64_t i = 0;
			for(; i < len; i++)
				if(values[i] != 0)
					return 0;
			
			return 1;
		}
		
		case CGVT_MATRIX: {
			CGMatrix* M = node->constant->value;
			uint64_t len = M->rows * M->cols;
			double* values = M->data;
			
			uint64_t i = 0;
			for(; i < len; i++)
				if(values[i] != 0)
					return 0;

			return 1;
		}
	}
	
}

/*
 * TODO: Fix memory leaks
 */
CGNode* optimizeNode(CGNode* node, CGraph* graph){
	switch(node->type){
		case CGNT_BINARY_OPERATION:{
			node->bop->lhs = optimizeNode(node->bop->lhs, graph);
			node->bop->rhs = optimizeNode(node->bop->rhs, graph);
			break;
		}
		case CGNT_UNARY_OPERATION:{
			node->uop->uhs = optimizeNode(node->uop->uhs, graph);
			break;
		}
		default:
			break;
	}
	
	if(node->type == CGNT_BINARY_OPERATION){
		if((node->bop->type == CGBOT_MULT) || (node->bop->type == CGBOT_DIV)){
			if(node->bop->lhs->type == CGNT_CONSTANT){
				if(nodeValueIsZero(node->bop->lhs)){
					//freeNode(graph, node);
					return makeZeroDoubleConstantNode(0);
				}
			}
			
			if((node->bop->type == CGBOT_MULT) && (node->bop->rhs->type == CGNT_CONSTANT)){
				if(nodeValueIsZero(node->bop->rhs)){
					//freeNode(graph, node);
					return makeZeroDoubleConstantNode(0);
				}
			}
		}
		
		if((node->bop->type == CGBOT_ADD) || (node->bop->type == CGBOT_SUB)){
			if(node->bop->lhs->type == CGNT_CONSTANT){
				if(nodeValueIsZero(node->bop->lhs)){
					CGNode* rhs = node->bop->rhs;
					freeNode(graph, node->bop->lhs);
					CGNode* newVal = (node->bop->type == CGBOT_SUB)?makeUnaryOpNode(CGUOT_MINUS, rhs):rhs;
					free(node);
					return newVal;
				}
			}
			
			if(node->bop->rhs->type == CGNT_CONSTANT){
				if(nodeValueIsZero(node->bop->rhs)){
					CGNode* lhs = node->bop->lhs;
					freeNode(graph, node->bop->rhs);
					free(node);
					return lhs;
				}
			}
		}
	}
	return node;
}

void optimizeGraph(CGraph* graph){
	graph->root = optimizeNode(graph->root, graph);
}


void printNode(CGNode* node){
	switch(node->constant->type){
		case CGVT_DOUBLE:
		{
			printf("current diff %f\n",  ((CGDouble*) node->constant->value)->value);
			break;
		}
		
		case CGVT_VECTOR:
		{
			CGVector* vec = (CGVector*)node->constant->value;
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
			CGMatrix* m = (CGMatrix*) node->constant->value;
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
}

void freeNodeDiff(CGNode* node){
	if(node->diff != NULL){
		freeNode(NULL, node->diff);
		free(node->diff);
	}
}

void autoDifferenciateNode(CGraph* graph, CGNode* node){
	/*
	//if(node->type != CGNT_VARIABLE)
		switch((node->diff)->constant->type){
			case CGVT_DOUBLE:
			{
				printf("current diff %f\n",  ((CGDouble*) (node->diff)->constant->value)->value);
				break;
			}
			
			case CGVT_VECTOR:
			{
				CGVector* vec = (CGVector*)(node->diff)->constant->value;
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
				CGMatrix* m = (CGMatrix*) (node->diff)->constant->value;
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
		*/
	
	
	
	switch(node->type){
		case CGNT_CONSTANT:

			break;
		case CGNT_VARIABLE:
		{
			CGNode* original = graphGetVar(graph, node->var->name);
			freeNodeDiff(original);
			original->diff = copyNode((node->diff));
			break;
		}
			/*
			printf("%s = ", node->var->name);
			switch((node->diff)->constant->type){
				case CGVT_DOUBLE:
				{
					printf("%f\n",  ((CGDouble*) (node->diff)->constant->value)->value);
					break;
				}
				
				case CGVT_VECTOR:
				{
					CGVector* vec = (CGVector*)(node->diff)->constant->value;
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
					CGMatrix* m = (CGMatrix*) (node->diff)->constant->value;
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
			*/
			break;
			
		case CGNT_BINARY_OPERATION:
		{
			switch(node->bop->type){
				case CGBOT_MULT:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->bop->lhs->diff), makeBinaryOpNode(CGBOT_MULT, copyNode(node->diff), resultNodeToConstantNodeCopy(node->bop->rhs->result)));
					CGResultNode* res1 = computeRawNode(mult1);
					freeNodeDiff(node->bop->lhs);
					node->bop->lhs->diff = resultNodeToConstantNodeCopy(res1);
					
					CGNode* mult2 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->bop->rhs->diff), makeBinaryOpNode(CGBOT_MULT, copyNode(node->diff), resultNodeToConstantNodeCopy(node->bop->lhs->result)));
					CGResultNode* res2 = computeRawNode(mult2);
					freeNodeDiff(node->bop->rhs);
					node->bop->rhs->diff = resultNodeToConstantNodeCopy(res2);
					
					freeResultNode(res1);
					freeResultNode(res2);
					free(res1);
					free(res2);
					
					autoDifferenciateNode(graph, node->bop->lhs);
					autoDifferenciateNode(graph, node->bop->rhs);
					break;
				}
				
				
				case CGBOT_ADD:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->bop->lhs->diff), copyNode(node->diff));
					CGResultNode* res1 = computeRawNode(mult1);
					freeNodeDiff(node->bop->lhs);
					node->bop->lhs->diff = resultNodeToConstantNodeCopy(res1);
					
					CGNode* mult2 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->bop->rhs->diff), copyNode(node->diff));
					CGResultNode* res2 = computeRawNode(mult2);
					freeNodeDiff(node->bop->rhs);
					node->bop->rhs->diff = resultNodeToConstantNodeCopy(res2);
					
					autoDifferenciateNode(graph, node->bop->lhs);
					autoDifferenciateNode(graph, node->bop->rhs);
					
					freeResultNode(res1);
					freeResultNode(res2);
					free(res1);
					free(res2);
					
					break;
				}
				
				
				case CGBOT_SUB:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->bop->lhs->diff), copyNode(node->diff));
					CGResultNode* res1 = computeRawNode(mult1);
					freeNodeDiff(node->bop->lhs);
					(node->bop->lhs->diff) = resultNodeToConstantNodeCopy(res1);
					
					
					CGNode* mult2 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->bop->rhs->diff), makeUnaryOpNode(CGUOT_MINUS, copyNode(node->diff)));
					CGResultNode* res2 = computeRawNode(mult2);
					freeNodeDiff(node->bop->rhs);
					(node->bop->rhs->diff) = resultNodeToConstantNodeCopy(res2);
					
					autoDifferenciateNode(graph, node->bop->lhs);
					autoDifferenciateNode(graph, node->bop->rhs);
					
					freeResultNode(res1);
					freeResultNode(res2);
					free(res1);
					free(res2);
					
					break;
				}
				
				case CGBOT_DIV:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->bop->lhs->diff), makeBinaryOpNode(CGBOT_DOT, makeBinaryOpNode(CGBOT_DIV,  makeDoubleConstantNode(1.0), resultNodeToConstantNodeCopy(node->bop->rhs->result)), copyNode(node->diff)));
					CGResultNode* res1 = computeRawNode(mult1);
					freeNodeDiff(node->bop->lhs);
					(node->bop->lhs->diff) = resultNodeToConstantNodeCopy(res1);
					
					CGNode* mult2 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->bop->rhs->diff), makeBinaryOpNode(CGBOT_MULT, makeUnaryOpNode(CGUOT_MINUS, makeBinaryOpNode(CGBOT_DIV, resultNodeToConstantNodeCopy(node->bop->lhs->result), makeBinaryOpNode(CGBOT_POW, resultNodeToConstantNodeCopy(node->bop->rhs->result), makeDoubleConstantNode(2.0)))), copyNode(node->diff)));
					CGResultNode* res2 = computeRawNode(mult2);
					freeNodeDiff(node->bop->rhs);
					(node->bop->rhs->diff) = resultNodeToConstantNodeCopy(res2);
					
					autoDifferenciateNode(graph, node->bop->lhs);
					autoDifferenciateNode(graph, node->bop->rhs);
					
					freeResultNode(res1);
					freeResultNode(res2);
					free(res1);
					free(res2);
					
					break;
				}
				
				case CGBOT_POW:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->bop->lhs->diff), makeBinaryOpNode(CGBOT_MULT, copyNode(node->diff), makeBinaryOpNode(CGBOT_MULT, resultNodeToConstantNodeCopy(node->bop->rhs->result), makeBinaryOpNode(CGBOT_POW, resultNodeToConstantNodeCopy(node->bop->lhs->result), makeBinaryOpNode(CGBOT_SUB, resultNodeToConstantNodeCopy(node->bop->rhs->result), makeDoubleConstantNode(1.0))))));
					CGResultNode* res1 = computeRawNode(mult1);
					freeNodeDiff(node->bop->lhs);
					(node->bop->lhs->diff) = resultNodeToConstantNodeCopy(res1);
					
					
					CGNode* mult2 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->bop->rhs->diff), makeBinaryOpNode(CGBOT_MULT, copyNode(node->diff), makeBinaryOpNode(CGBOT_MULT,  resultNodeToConstantNodeCopy(node->result), makeUnaryOpNode(CGUOT_LOG, resultNodeToConstantNodeCopy(node->bop->lhs->result)))));
					CGResultNode* res2 = computeRawNode(mult2);
					freeNodeDiff(node->bop->rhs);
					(node->bop->rhs->diff) = resultNodeToConstantNodeCopy(res2);
					
					autoDifferenciateNode(graph, node->bop->lhs);
					autoDifferenciateNode(graph, node->bop->rhs);
					
					freeResultNode(res1);
					freeResultNode(res2);
					free(res1);
					free(res2);
					
					break;
				}
				
				
				case CGBOT_TMULT:
				{
					break;
				}
				
				case CGBOT_DOT:
				{
					//printf("Node Diff\n");
					//printNode((node->diff));
					//printf("LHS Diff\n");
					//printNode((node->bop->lhs->diff));
					//printf("RHS DIFF\n");
					//printNode((node->bop->rhs->diff));
					
					//printf("LHS Res\n");
					//printNode(resultNodeToConstantNodeCopy(node->bop->lhs->result));
					//printf("RHS Res\n");
					//printNode(resultNodeToConstantNodeCopy(node->bop->rhs->result));
					
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->bop->lhs->diff), makeBinaryOpNode(CGBOT_DOT, copyNode(node->diff), makeUnaryOpNode(CGUOT_TRANSPOSE, resultNodeToConstantNodeCopy(node->bop->rhs->result))));
					CGResultNode* res1 = computeRawNode(mult1);
					freeNodeDiff(node->bop->lhs);
					(node->bop->lhs->diff) = resultNodeToConstantNodeCopy(res1);
					//printf("LHS\n");
					//printNode(copyNode(node->bop->lhs->diff));
					
					CGNode* mult2 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->bop->rhs->diff), makeBinaryOpNode(CGBOT_DOT, makeUnaryOpNode(CGUOT_TRANSPOSE,  resultNodeToConstantNodeCopy(node->bop->lhs->result)), copyNode(node->diff)));
					CGResultNode* res2 = computeRawNode(mult2);
					freeNodeDiff(node->bop->rhs);
					(node->bop->rhs->diff) = resultNodeToConstantNodeCopy(res2);
					//printf("RHS\n");
					//printNode(copyNode(node->bop->rhs->diff));
					
					autoDifferenciateNode(graph, node->bop->lhs);
					autoDifferenciateNode(graph, node->bop->rhs);
					
					freeResultNode(res1);
					freeResultNode(res2);
					free(res1);
					free(res2);
					
					break;
				}
			}
		}
		
		case CGNT_UNARY_OPERATION:
		{
			switch(node->uop->type)
			{
				case CGUOT_MINUS:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->uop->uhs->diff), makeBinaryOpNode(CGBOT_MULT, copyNode(node->diff), makeDoubleConstantNode(-1.0)));
					CGResultNode* res1 = computeRawNode(mult1);
					freeNodeDiff(node->uop->uhs);
					(node->uop->uhs->diff) = resultNodeToConstantNodeCopy(res1);
					autoDifferenciateNode(graph, node->uop->uhs);
					
					freeResultNode(res1);
					free(res1);
					
					break;
				}
				
				case CGUOT_COS:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->uop->uhs->diff), makeBinaryOpNode(CGBOT_MULT, copyNode(node->diff), makeUnaryOpNode(CGUOT_MINUS, makeUnaryOpNode(CGUOT_SIN , resultNodeToConstantNodeCopy(node->uop->uhs->result)))));
					CGResultNode* res1 = computeRawNode(mult1);
					freeNodeDiff(node->uop->uhs);
					(node->uop->uhs->diff) = resultNodeToConstantNodeCopy(res1);
					autoDifferenciateNode(graph, node->uop->uhs);
					
					freeResultNode(res1);
					free(res1);
					
					break;
				}
				
				case CGUOT_SIN:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->uop->uhs->diff), makeBinaryOpNode(CGBOT_MULT, copyNode(node->diff), makeUnaryOpNode(CGUOT_COS, resultNodeToConstantNodeCopy(node->uop->uhs->result))));
					CGResultNode* res1 = computeRawNode(mult1);
					freeNodeDiff(node->uop->uhs);
					(node->uop->uhs->diff) = resultNodeToConstantNodeCopy(res1);
					autoDifferenciateNode(graph, node->uop->uhs);
					
					freeResultNode(res1);
					free(res1);
					
					break;
				}
				
				case CGUOT_EXP:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->uop->uhs->diff), makeBinaryOpNode(CGBOT_MULT, copyNode(node->diff), resultNodeToConstantNodeCopy(node->result)));
					CGResultNode* res1 = computeRawNode(mult1);
					freeNodeDiff(node->uop->uhs);
					(node->uop->uhs->diff) = resultNodeToConstantNodeCopy(res1);
					autoDifferenciateNode(graph, node->uop->uhs);
					
					freeResultNode(res1);
					free(res1);
					
					break;
				}
				
				case CGUOT_LOG:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->uop->uhs->diff), makeBinaryOpNode(CGBOT_MULT, copyNode(node->diff), makeBinaryOpNode(CGBOT_DIV, makeDoubleConstantNode(1.0), resultNodeToConstantNodeCopy(node->uop->uhs->result))));
					CGResultNode* res1 = computeRawNode(mult1);
					freeNodeDiff(node->uop->uhs);
					(node->uop->uhs->diff) = resultNodeToConstantNodeCopy(res1);
					autoDifferenciateNode(graph, node->uop->uhs);
					
					freeResultNode(res1);
					free(res1);
					
					break;
				}
				
				case CGUOT_TAN:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->uop->uhs->diff), makeBinaryOpNode(CGBOT_MULT, copyNode(node->diff), makeBinaryOpNode(CGBOT_DIV, makeDoubleConstantNode(1.0), makeUnaryOpNode(CGUOT_COS, makeBinaryOpNode(CGBOT_MULT, makeDoubleConstantNode(2.0), resultNodeToConstantNodeCopy(node->uop->uhs->result))))));
					CGResultNode* res1 = computeRawNode(mult1);
					freeNodeDiff(node->uop->uhs);
					(node->uop->uhs->diff) = resultNodeToConstantNodeCopy(res1);
					autoDifferenciateNode(graph, node->uop->uhs);
					
					freeResultNode(res1);
					free(res1);
					
					break;
				}
				
				case CGUOT_TANH:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->uop->uhs->diff), makeBinaryOpNode(CGBOT_MULT, copyNode(node->diff), makeBinaryOpNode(CGBOT_DIV, makeDoubleConstantNode(1.0), makeBinaryOpNode(CGBOT_POW, resultNodeToConstantNodeCopy(node->uop->uhs->result), 2))));
					CGResultNode* res1 = computeRawNode(mult1);
					freeNodeDiff(node->uop->uhs);
					(node->uop->uhs->diff) = resultNodeToConstantNodeCopy(res1);
					autoDifferenciateNode(graph, node->uop->uhs);
					
					freeResultNode(res1);
					free(res1);
					
					break;
				}
				
				
				case CGUOT_TRANSPOSE:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->uop->uhs->diff), makeUnaryOpNode(CGUOT_TRANSPOSE, copyNode(node->diff)));
					CGResultNode* res1 = computeRawNode(mult1);
					freeNodeDiff(node->uop->uhs);
					(node->uop->uhs->diff) = resultNodeToConstantNodeCopy(res1);
					autoDifferenciateNode(graph, node->uop->uhs);
					
					freeResultNode(res1);
					free(res1);
					
					break;
				}
				
				case CGUOT_RELU:{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->uop->uhs->diff), dx_relu(node->uop->uhs->result));
					CGResultNode* res1 = computeRawNode(mult1);
					freeNodeDiff(node->uop->uhs);
					(node->uop->uhs->diff) = resultNodeToConstantNodeCopy(res1);
					autoDifferenciateNode(graph, node->uop->uhs);
					
					freeResultNode(res1);
					free(res1);
					
					break;
				}
				
				case CGUOT_INV:
				{
					
					break;
				}
			}
		}
		
		case CGNT_GRAPH: 
		{
			break;
		}
		
		case CGNT_AXIS_BOUND_OPERATION: 
		{
			switch(node->axop->type){
				case CGABOT_SUM: {
					CGNode* mult1 = NULL;
					//if(node->axop->axis == 0){
						//printNode(node->axop->uhs->diff);
						mult1 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->axop->uhs->diff), makeUnaryOpNode(CGUOT_TRANSPOSE, copyNode(node->diff)));
					/*}
					else
					{
						mult1 = makeUnaryOpNode(CGUOT_TRANSPOSE, makeBinaryOpNode(CGBOT_ADD, makeUnaryOpNode(CGUOT_TRANSPOSE, copyNode(node->axop->uhs->diff)), copyNode(node->diff)));
					}*/
					CGResultNode* res1 = computeRawNode(mult1);
					freeNodeDiff(node->axop->uhs);
					node->axop->uhs->diff = resultNodeToConstantNodeCopy(res1);
					autoDifferenciateNode(graph, node->axop->uhs);
					
					freeResultNode(res1);
					free(res1);
					
					break;
				}
				
				default:
				{
					break;
				}
			}
			break;
		}
		
		case CGNT_CROSS_ENTROPY_LOSS_FUNC:
		{
			CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, copyNode(node->crossEntropyLoss->x->diff), makeBinaryOpNode(CGBOT_MULT, copyNode(node->diff), dx_crossEntropy(node->crossEntropyLoss->x->result, node->crossEntropyLoss->y->result, node->crossEntropyLoss->num_classes)));
			CGResultNode* res1 = computeRawNode(mult1);
			freeNodeDiff(node->crossEntropyLoss->x->diff);
			node->crossEntropyLoss->x->diff = resultNodeToConstantNodeCopy(res1);
			
			autoDifferenciateNode(graph, node->crossEntropyLoss->x);
			
			break;
		}
	}
}

void autoDifferenciateGraph(CGraph* graph){
	switch(graph->root->result->type){
		case CGVT_DOUBLE:
			graph->root->diff = makeOnesDoubleConstantNode();
			break;
			
		case CGVT_VECTOR:{
			CGVector* v = (CGVector*)graph->root->result->value;
			graph->root->diff = makeOnesVectorConstantNode(v->len);
			break;
		}
		
		case CGVT_MATRIX:{
			CGMatrix* v = (CGMatrix*)graph->root->result->value;
			graph->root->diff = makeOnesMatrixConstantNode(v->rows, v->cols);
			break;
		}
	}
	autoDifferenciateNode(graph, graph->root);
}
