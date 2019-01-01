
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

#include <malloc.h>

#define diff_node differentiateNodeWRTNode

CGNode* differentiateNodeWRTNode(CGNode* node, CGraph* graph, CGNode* wrtNode){
	if(node == wrtNode){
		return makeDoubleConstantNode(1);
	}
	
	switch(node->type){
		case CGNT_VARIABLE: {
			return makeDoubleConstantNode(0);
		}
		
		case CGNT_CONSTANT:{
			switch(node->constant->type){
				case CGVT_DOUBLE: {
					return makeZeroDoubleConstantNode();
				}
				
				case CGVT_VECTOR: {
					uint64_t len = ((CGVector*)node->constant->value)->len;
					return makeZeroVectorConstantNode(len);
				}
				
				case CGVT_MATRIX: {
					CGMatrix* M = node->constant->value;
					return makeZeroMatrixConstantNode(M->rows, M->cols);
				}
			}
		}
		
		case CGNT_BINARY_OPERATION:
		{
			switch(node->bop->type){
				case CGBOT_ADD:
					// f + g = f' + g'
					return makeBinaryOpNode(CGBOT_ADD, diff_node(node->bop->lhs, graph, wrtNode), diff_node(node->bop->rhs, graph, wrtNode));
					
				case CGBOT_SUB:
					// f - g = f' + g'
					return makeBinaryOpNode(CGBOT_SUB, diff_node(node->bop->lhs, graph, wrtNode), diff_node(node->bop->rhs, graph, wrtNode));
					
				case CGBOT_MULT:{
					// f * g = f*g' + f'*g
					CGNode* lhs = makeBinaryOpNode(CGBOT_MULT, node->bop->lhs, diff_node(node->bop->rhs, graph, wrtNode));
					CGNode* rhs = makeBinaryOpNode(CGBOT_MULT, diff_node(node->bop->lhs, graph, wrtNode), node->bop->rhs);
					
					return makeBinaryOpNode(CGBOT_ADD, lhs, rhs);
				}
				
				case CGBOT_DIV:{
					// f / g = (f'*g − g'*f )/g^2
					
					CGNode* lhs1 = makeBinaryOpNode(CGBOT_MULT, diff_node(node->bop->lhs, graph, wrtNode), node->bop->rhs);
					CGNode* rhs1 = makeBinaryOpNode(CGBOT_MULT, node->bop->lhs, diff_node(node->bop->rhs, graph, wrtNode));
					
					CGNode* lhs2 = makeBinaryOpNode(CGBOT_SUB, lhs1, rhs1);
					CGNode* rhs2 = makeBinaryOpNode(CGBOT_POW, node->bop->rhs, makeDoubleConstantNode(2));
					
					
					return makeBinaryOpNode(CGBOT_DIV, lhs2, rhs2);
				}
				
				case CGBOT_POW: {
					// f ^ g = f^g * (g'*ln(f) + g*(f'/f)
					// TODO test d(x^(2x))
					CGNode* term1 = makeBinaryOpNode(CGBOT_POW, node->bop->lhs, node->bop->rhs);
					CGNode* term2 = makeBinaryOpNode(CGBOT_MULT, diff_node(node->bop->rhs, graph, wrtNode), makeUnaryOpNode(CGUOT_LOG, node->bop->lhs));
					CGNode* term3 = makeBinaryOpNode(CGBOT_MULT, node->bop->rhs, makeBinaryOpNode(CGBOT_DIV, diff_node(node->bop->lhs, graph, wrtNode), node->bop->lhs));
					
					CGNode* lhs = term1;
					CGNode* rhs = makeBinaryOpNode(CGBOT_ADD, term2, term3);
					
					return makeBinaryOpNode(CGBOT_MULT, lhs, rhs);
				}
				// TODO
				case CGBOT_TMULT:
				case CGBOT_DOT: 
					return NULL;

			}
		}
		
		case CGNT_UNARY_OPERATION:{
			switch(node->uop->type){
				case CGUOT_SIN:{
					// d(sin(f)) = cos(f)*d(f)
					CGNode* lhs = makeUnaryOpNode(CGUOT_COS, node->uop->uhs);
					CGNode* rhs = diff_node(node->uop->uhs, graph, wrtNode);
					
					return makeBinaryOpNode(CGBOT_MULT, lhs, rhs);
				}
				
				case CGUOT_COS:{
					// d(cos(f)) = -sin(f)*d(f)
					CGNode* lhs = makeUnaryOpNode(CGUOT_MINUS, makeUnaryOpNode(CGUOT_SIN, node->uop->uhs));
					CGNode* rhs = diff_node(node->uop->uhs, graph, wrtNode);
					
					return makeBinaryOpNode(CGBOT_MULT, lhs, rhs);
				}
				
				case CGUOT_EXP: {
					// d(e^f) = e^f*d(f)
					CGNode* lhs = makeUnaryOpNode(CGUOT_EXP, node->uop->uhs);
					CGNode* rhs = diff_node(node->uop->uhs, graph, wrtNode);
					
					return makeBinaryOpNode(CGBOT_MULT, lhs, rhs);
				}
				
				case CGUOT_LOG: {
					// d(ln(f)) = f'/f
					CGNode* lhs = diff_node(node->uop->uhs, graph, wrtNode);
					
					return makeBinaryOpNode(CGBOT_DIV, lhs, node->uop->uhs);
				}
				
				case CGUOT_TAN: {
					// d(tan f) = sec(f)^2 * d(f)
					CGNode* secSquared= makeBinaryOpNode(CGBOT_POW, makeBinaryOpNode(CGBOT_DIV, makeDoubleConstantNode(1), makeUnaryOpNode(CGUOT_COS, node->uop->uhs)), makeDoubleConstantNode(2));
					CGNode* rhs = diff_node(node->uop->uhs, graph, wrtNode);
					
					return makeBinaryOpNode(CGBOT_MULT, secSquared, rhs);
				}
				
				case CGUOT_MINUS:{
					return makeUnaryOpNode(CGUOT_MINUS, diff_node(node->uop->uhs, graph, wrtNode));
				}
				// TODO:
				case CGUOT_TANH:
				case CGUOT_INV:
					return NULL;
					
				case CGUOT_TRANSPOSE:
					return makeUnaryOpNode(CGUOT_TRANSPOSE, diff_node(node->uop->uhs, graph, wrtNode));
			}
		}
		
		/* TODO: Fix
		case CGNT_SUM_OPERATION: {
			return make(CGUOT_SUM, diff_node(node->uop->uhs, graph, wrtNode));
		}
      */
		case CGNT_GRAPH:
		{
			return diff_node(node->graph->root, node->graph, wrtNode);
		}
	}
}

CGraph* partialDiffWRTNode(CGNode* node, CGraph* graph, CGNode* rtNode) {
	if(node == rtNode){
		return NULL;
	}
}

CGraph* differentiateGraphWRTNode(CGraph* graph, char* newName, CGNode* rtNode){
	CGraph* diff = makeGraph(newName);
	diff->root = diff_node(graph->root, graph, rtNode);
	return diff;
}

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

void autoDifferenciateNode(CGraph* graph, CGNode* node){
	/*
	//if(node->type != CGNT_VARIABLE)
		switch(node->diff->constant->type){
			case CGVT_DOUBLE:
			{
				printf("current diff %f\n",  ((CGDouble*) node->diff->constant->value)->value);
				break;
			}
			
			case CGVT_VECTOR:
			{
				CGVector* vec = (CGVector*)node->diff->constant->value;
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
				CGMatrix* m = (CGMatrix*) node->diff->constant->value;
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
			original->diff = node->diff;
			break;
		}
			/*
			printf("%s = ", node->var->name);
			switch(node->diff->constant->type){
				case CGVT_DOUBLE:
				{
					printf("%f\n",  ((CGDouble*) node->diff->constant->value)->value);
					break;
				}
				
				case CGVT_VECTOR:
				{
					CGVector* vec = (CGVector*)node->diff->constant->value;
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
					CGMatrix* m = (CGMatrix*) node->diff->constant->value;
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
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, node->bop->lhs->diff, makeBinaryOpNode(CGBOT_MULT, node->diff, resultNodeToConstantNode(node->bop->rhs->result)));
					CGResultNode* res1 = computeRawNode(mult1);
					node->bop->lhs->diff = resultNodeToConstantNode(res1);
					
					
					CGNode* mult2 = makeBinaryOpNode(CGBOT_ADD, node->bop->rhs->diff, makeBinaryOpNode(CGBOT_MULT, node->diff, resultNodeToConstantNode(node->bop->lhs->result)));
					CGResultNode* res2 = computeRawNode(mult2);
					node->bop->rhs->diff = resultNodeToConstantNode(res2);
					
					autoDifferenciateNode(graph, node->bop->lhs);
					autoDifferenciateNode(graph, node->bop->rhs);
					break;
				}
				
				
				case CGBOT_ADD:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, node->bop->lhs->diff, node->diff);
					CGResultNode* res1 = computeRawNode(mult1);
					node->bop->lhs->diff = resultNodeToConstantNode(res1);
					
					
					CGNode* mult2 = makeBinaryOpNode(CGBOT_ADD, node->bop->rhs->diff, node->diff);
					CGResultNode* res2 = computeRawNode(mult2);
					node->bop->rhs->diff = resultNodeToConstantNode(res2);
					
					autoDifferenciateNode(graph, node->bop->lhs);
					autoDifferenciateNode(graph, node->bop->rhs);
					break;
				}
				
				
				case CGBOT_SUB:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, node->bop->lhs->diff, node->diff);
					CGResultNode* res1 = computeRawNode(mult1);
					node->bop->lhs->diff = resultNodeToConstantNode(res1);
					
					
					CGNode* mult2 = makeBinaryOpNode(CGBOT_ADD, node->bop->rhs->diff, makeUnaryOpNode(CGUOT_MINUS, node->diff));
					CGResultNode* res2 = computeRawNode(mult2);
					node->bop->rhs->diff = resultNodeToConstantNode(res2);
					
					autoDifferenciateNode(graph, node->bop->lhs);
					autoDifferenciateNode(graph, node->bop->rhs);
					break;
				}
				
				case CGBOT_DIV:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, node->bop->lhs->diff, makeBinaryOpNode(CGBOT_MULT,node->diff, makeBinaryOpNode(CGBOT_DIV,  makeDoubleConstantNode(1.0), resultNodeToConstantNode(node->bop->rhs->result))));
					CGResultNode* res1 = computeRawNode(mult1);
					node->bop->lhs->diff = resultNodeToConstantNode(res1);
					
					CGNode* mult2 = makeBinaryOpNode(CGBOT_ADD, node->bop->rhs->diff, makeBinaryOpNode(CGBOT_MULT, node->diff, makeUnaryOpNode(CGUOT_MINUS, makeBinaryOpNode(CGBOT_DIV, resultNodeToConstantNode(node->bop->lhs->result), makeBinaryOpNode(CGBOT_POW, resultNodeToConstantNode(node->bop->rhs->result), makeDoubleConstantNode(2.0))))));
					CGResultNode* res2 = computeRawNode(mult2);
					node->bop->rhs->diff = resultNodeToConstantNode(res2);
					
					autoDifferenciateNode(graph, node->bop->lhs);
					autoDifferenciateNode(graph, node->bop->rhs);
					break;
				}
				
				case CGBOT_POW:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, node->bop->lhs->diff, makeBinaryOpNode(CGBOT_MULT, node->diff, makeBinaryOpNode(CGBOT_MULT, resultNodeToConstantNode(node->bop->rhs->result), makeBinaryOpNode(CGBOT_POW, resultNodeToConstantNode(node->bop->lhs->result), makeBinaryOpNode(CGBOT_SUB, resultNodeToConstantNode(node->bop->rhs->result), makeDoubleConstantNode(1.0))))));
					CGResultNode* res1 = computeRawNode(mult1);
					node->bop->lhs->diff = resultNodeToConstantNode(res1);
					
					
					CGNode* mult2 = makeBinaryOpNode(CGBOT_ADD, node->bop->rhs->diff, makeBinaryOpNode(CGBOT_MULT, node->diff, makeBinaryOpNode(CGBOT_MULT,  resultNodeToConstantNode(node->result), makeUnaryOpNode(CGUOT_LOG, resultNodeToConstantNode(node->bop->lhs->result)))));
					CGResultNode* res2 = computeRawNode(mult2);
					node->bop->rhs->diff = resultNodeToConstantNode(res2);
					
					autoDifferenciateNode(graph, node->bop->lhs);
					autoDifferenciateNode(graph, node->bop->rhs);
					break;
				}
				
				
				case CGBOT_TMULT:
				{
					break;
				}
				
				case CGBOT_DOT:
				{
					//printf("Node Diff\n");
					//printNode(node->diff);
					//printf("LHS Diff\n");
					//printNode(node->bop->lhs->diff);
					//printf("RHS DIFF\n");
					//printNode(node->bop->rhs->diff);
					
					//printf("LHS Res\n");
					//printNode(resultNodeToConstantNode(node->bop->lhs->result));
					//printf("RHS Res\n");
					//printNode(resultNodeToConstantNode(node->bop->rhs->result));
					
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, node->bop->lhs->diff, makeBinaryOpNode(CGBOT_DOT, node->diff, makeUnaryOpNode(CGUOT_TRANSPOSE, resultNodeToConstantNode(node->bop->rhs->result))));
					CGResultNode* res1 = computeRawNode(mult1);
					node->bop->lhs->diff = resultNodeToConstantNode(res1);
					//printf("LHS\n");
					//printNode(node->bop->lhs->diff);
					
					CGNode* mult2 = makeBinaryOpNode(CGBOT_ADD, node->bop->rhs->diff, makeBinaryOpNode(CGBOT_DOT, makeUnaryOpNode(CGUOT_TRANSPOSE,  resultNodeToConstantNode(node->bop->lhs->result)), node->diff));
					CGResultNode* res2 = computeRawNode(mult2);
					node->bop->rhs->diff = resultNodeToConstantNode(res2);
					//printf("RHS\n");
					//printNode(node->bop->rhs->diff);
					
					autoDifferenciateNode(graph, node->bop->lhs);
					autoDifferenciateNode(graph, node->bop->rhs);
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
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, node->uop->uhs->diff, makeBinaryOpNode(CGBOT_MULT, node->diff, makeDoubleConstantNode(-1.0)));
					CGResultNode* res1 = computeRawNode(mult1);
					node->uop->uhs->diff = resultNodeToConstantNode(res1);
					autoDifferenciateNode(graph, node->uop->uhs);
					break;
				}
				
				case CGUOT_COS:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, node->uop->uhs->diff, makeBinaryOpNode(CGBOT_MULT, node->diff, makeUnaryOpNode(CGUOT_MINUS, makeUnaryOpNode(CGUOT_SIN , resultNodeToConstantNode(node->uop->uhs->result)))));
					CGResultNode* res1 = computeRawNode(mult1);
					node->uop->uhs->diff = resultNodeToConstantNode(res1);
					autoDifferenciateNode(graph, node->uop->uhs);
					break;
				}
				
				case CGUOT_SIN:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, node->uop->uhs->diff, makeBinaryOpNode(CGBOT_MULT, node->diff, makeUnaryOpNode(CGUOT_COS, resultNodeToConstantNode(node->uop->uhs->result))));
					CGResultNode* res1 = computeRawNode(mult1);
					node->uop->uhs->diff = resultNodeToConstantNode(res1);
					autoDifferenciateNode(graph, node->uop->uhs);
					break;
				}
				
				case CGUOT_EXP:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, node->uop->uhs->diff, makeBinaryOpNode(CGBOT_MULT, node->diff, resultNodeToConstantNode(node->result)));
					CGResultNode* res1 = computeRawNode(mult1);
					node->uop->uhs->diff = resultNodeToConstantNode(res1);
					autoDifferenciateNode(graph, node->uop->uhs);
					break;
				}
				
				case CGUOT_LOG:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, node->uop->uhs->diff, makeBinaryOpNode(CGBOT_MULT, node->diff, makeBinaryOpNode(CGBOT_DIV, makeDoubleConstantNode(1.0), resultNodeToConstantNode(node->uop->uhs->result))));
					CGResultNode* res1 = computeRawNode(mult1);
					node->uop->uhs->diff = resultNodeToConstantNode(res1);
					autoDifferenciateNode(graph, node->uop->uhs);
					break;
				}
				
				case CGUOT_TAN:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, node->uop->uhs->diff, makeBinaryOpNode(CGBOT_MULT, node->diff, makeBinaryOpNode(CGBOT_DIV, makeDoubleConstantNode(1.0), makeUnaryOpNode(CGUOT_COS, makeBinaryOpNode(CGBOT_MULT, makeDoubleConstantNode(2.0), resultNodeToConstantNode(node->uop->uhs->result))))));
					CGResultNode* res1 = computeRawNode(mult1);
					node->uop->uhs->diff = resultNodeToConstantNode(res1);
					autoDifferenciateNode(graph, node->uop->uhs);
					break;
				}
				
				case CGUOT_TANH:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, node->uop->uhs->diff, makeBinaryOpNode(CGBOT_MULT, node->diff, makeBinaryOpNode(CGBOT_DIV, makeDoubleConstantNode(1.0), makeBinaryOpNode(CGBOT_POW, resultNodeToConstantNode(node->uop->uhs->result), 2))));
					CGResultNode* res1 = computeRawNode(mult1);
					node->uop->uhs->diff = resultNodeToConstantNode(res1);
					autoDifferenciateNode(graph, node->uop->uhs);
					break;
				}
				
				
				case CGUOT_TRANSPOSE:
				{
					CGNode* mult1 = makeBinaryOpNode(CGBOT_ADD, node->uop->uhs->diff, makeUnaryOpNode(CGUOT_TRANSPOSE, node->diff));
					CGResultNode* res1 = computeRawNode(mult1);
					node->uop->uhs->diff = resultNodeToConstantNode(res1);
					autoDifferenciateNode(graph, node->uop->uhs);
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
					if(node->axop->axis == 0){
						//printNode(node->axop->uhs->diff);
						mult1 = makeBinaryOpNode(CGBOT_ADD, node->axop->uhs->diff, makeUnaryOpNode(CGUOT_TRANSPOSE, node->diff));
					}
					else
					{
						mult1 = makeUnaryOpNode(CGUOT_TRANSPOSE, makeBinaryOpNode(CGBOT_ADD, makeUnaryOpNode(CGUOT_TRANSPOSE, node->axop->uhs->diff), node->diff));
					}
					CGResultNode* res1 = computeRawNode(mult1);
					node->axop->uhs->diff = resultNodeToConstantNode(res1);
					autoDifferenciateNode(graph, node->axop->uhs);
					break;
				}
				
				default:
				{
					break;
				}
			}
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
