
#include <stdlib.h>
#include <stdio.h>

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

#define diff_node differentiateNodeWRTVar

CGNode* differentiateNodeWRTVar(CGNode* node, CGraph* graph, const char* wrtNode){
	switch(node->type){
		case CGNT_VARIABLE: {
			if(strcmp(node->var->name, wrtNode) == 0){
				return makeDoubleConstantNode(1);
			}
			
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
				
				case CGUOT_SUM: {
					return makeUnaryOpNode(CGUOT_SUM, diff_node(node->uop->uhs, graph, wrtNode));
				}
				
				// TODO:
				case CGUOT_TANH:
				case CGUOT_INV:
				case CGUOT_TRANSPOSE:
					return NULL;
			}
		}
		
		case CGNT_GRAPH:
		{
			return diff_node(node->graph->root, node->graph, wrtNode);
		}
	}
}

CGraph* differentiateGraphWRTVar(CGraph* graph, char* newName, const char* rtNode){
	CGraph* diff = makeGraph(newName);
	diff->root = diff_node(graph->root, graph, rtNode);
	return diff;
}
