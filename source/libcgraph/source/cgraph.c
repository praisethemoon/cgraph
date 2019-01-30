/*
 * 
 * 
 * 
 */

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <cblas.h>
#include <string.h> // memcpy
#include <math.h>
#include <stdarg.h>

#include "cgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"
#include "cg_errors.h"
#include "cg_constants.h"
#include "cg_enums.h"
#include "cg_factory.h"
#include "cg_math.h"
#include "cg_ops.h"


void cg_assert(int cond, const char * rawcond, const char * fmt, ...)
{
	if(cond)
		return;

	char temp[1024];
	va_list vl;
	va_start(vl, fmt);
	vsprintf(temp, fmt, vl);
	va_end(vl);
	fprintf(stdout, "Fatal error, assertion failed: %s\n", rawcond);
	fprintf(stdout, temp);
	fprintf(stdout, "\n");

	exit(-1);
}


#define CHECK_RESULT(node) \
if(node->error != NULL){\
	return node;\
}

/*
 * Works only with constant types
 */
CGNode* copyNode(CGNode* node){
	CGNode* n = calloc(1, sizeof(CGNode));
	if(node->type != CGNT_CONSTANT){
		fprintf(stderr, "Call to copyNodeValue with a non-constant node.\n... This should not happen, but who knows these days.");
		exit(-1);
	}
	
	n->type = CGNT_CONSTANT;
	n->constant = calloc(1, sizeof(CGPConstant));
	n->constant->type = node->constant->type;
	n->result = NULL;
	n->diff = NULL;
	
	switch(node->constant->type){
		case CGVT_DOUBLE: {
			CGDouble* d = calloc(1, sizeof(CGDouble));
			d->value = ((CGDouble*)node->constant->value)->value;
			
			n->constant->value = d;
			break;
		}
		
		case CGVT_VECTOR: {
			CGVector* V = calloc(1, sizeof(CGVector));
			CGVector* src = (CGVector*)node->constant->value;
			
			V->len = src->len;
			V->data = calloc(V->len, sizeof(CG_SCALAR_TYPE));
			
			memcpy(V->data, src->data, V->len*sizeof(CG_SCALAR_TYPE));
			
			n->constant->value = V;
			break;
		}
		
		case CGVT_MATRIX: {
			CGMatrix* M = calloc(1, sizeof(CGMatrix));
			CGMatrix* src = (CGMatrix*)node->constant->value;
			
			uint64_t size = src->cols * src->rows;
			
			M->rows = src->rows;
			M->cols = src->cols;
			M->data = calloc(size, sizeof(CG_SCALAR_TYPE));
			
			memcpy(M->data, src->data, size*sizeof(CG_SCALAR_TYPE));
			
			n->constant->value = M;
			break;
		}
	}
	
	return n;
}

void* copyNodeValue(CGNode* node){
	if(node->type != CGNT_CONSTANT){
		fprintf(stderr, "Call to copyNodeValue with a non-constant node.\n... This should not happen, but who knows these days.");
		exit(-1);
	}
	
	switch(node->constant->type){
		case CGVT_DOUBLE: {
			CGDouble* d = calloc(1, sizeof(CGDouble));
			d->value = ((CGDouble*)node->constant->value)->value;
			
			return d;
		}
		
		case CGVT_VECTOR: {
			CGVector* V = calloc(1, sizeof(CGVector));
			CGVector* src = (CGVector*)node->constant->value;
			
			V->len = src->len;
			V->data = calloc(V->len, sizeof(CG_SCALAR_TYPE));
			
			memcpy(V->data, src->data, V->len*sizeof(CG_SCALAR_TYPE));
			return V;
		}
		
		case CGVT_MATRIX: {
			CGMatrix* M = calloc(1, sizeof(CGMatrix));
			CGMatrix* src = (CGMatrix*)node->constant->value;
			
			uint64_t size = src->cols * src->rows;
			
			M->rows = src->rows;
			M->cols = src->cols;
			M->data = calloc(size, sizeof(CG_SCALAR_TYPE));
			
			memcpy(M->data, src->data, size*sizeof(CG_SCALAR_TYPE));
			return M;
		}
	}
}

/*
 * @Deprecated
 */
void* copyRNodeValue(CGResultNode* node){
	switch(node->type){
		case CGVT_DOUBLE: {
			CGDouble* d = calloc(1, sizeof(CGDouble));
			d->value = ((CGDouble*)node->value)->value;
			
			return d;
		}
		
		case CGVT_VECTOR: {
			CGVector* V = calloc(1, sizeof(CGVector));
			CGVector* src = (CGVector*)node->value;
			
			V->len = src->len;
			V->data = calloc(V->len, sizeof(CG_SCALAR_TYPE));
			
			memcpy(V->data, src->data, V->len*sizeof(CG_SCALAR_TYPE));
			return V;
		}
		
		case CGVT_MATRIX: {
			CGMatrix* M = calloc(1, sizeof(CGMatrix));
			CGMatrix* src = (CGMatrix*)node->value;
			
			uint64_t size = src->cols * src->rows;
			
			M->rows = src->rows;
			M->cols = src->cols;
			M->data = calloc(size, sizeof(CG_SCALAR_TYPE));
			
			memcpy(M->data, src->data, size*sizeof(CG_SCALAR_TYPE));
			return M;
		}
	}
}


CGResultNode* copyResultNode(CGResultNode* node){
	CGResultNode* res = calloc(1, sizeof(CGResultNode));
	
	switch(node->type){
		case CGVT_DOUBLE: {
			CGDouble* d = calloc(1, sizeof(CGDouble));
			CGDouble* d2 = (CGDouble*)node->value;
			d->value = d2->value;
			
			res->value = d;
			break;
		}
		
		case CGVT_VECTOR: {
			CGVector* V = calloc(1, sizeof(CGVector));
			CGVector* src = (CGVector*)node->value;
			
			V->len = src->len;
			V->data = calloc(V->len, sizeof(CG_SCALAR_TYPE));

#ifdef CG_USE_OPENCL
			V->buf = src->buf;
#endif
			memcpy(V->data, src->data, V->len*sizeof(CG_SCALAR_TYPE));
			
			res->value = V;
			break;
		}
		
		case CGVT_MATRIX: {
			CGMatrix* M = calloc(1, sizeof(CGMatrix));
			CGMatrix* src = (CGMatrix*)node->value;
			
			uint64_t size = src->cols * src->rows;
			
			M->rows = src->rows;
			M->cols = src->cols;
			M->data = calloc(size, sizeof(CG_SCALAR_TYPE));
#ifdef CG_USE_OPENCL
            M->buf = src->buf;
#endif
			
			memcpy(M->data, src->data, size*sizeof(CG_SCALAR_TYPE));
			
			res->value = M;
			break;
		}
	}
	res->type = node->type;
	
	return res;
}

CGMatrix* vectorToMatrix(CGVector* v){
	CGMatrix* m = calloc(1, sizeof(CGMatrix));
	m->rows = v->len;
	m->cols = 1;
	m->data = v->data;
	
	return m;
}


/*
 * Computational Graph traversing
 */

CGResultNode* processUnaryOperation(CGraph* graph, CGUnaryOperationType type, CGNode* uhs, CGNode* parentNode){
	CGVarType uhsType = CGVT_DOUBLE;
	void* uhsValue = NULL;
	CGResultNode* newres = NULL;
	
	CGResultNode* lhsResult = computeCGNode(graph, uhs);
	CHECK_RESULT(lhsResult)
	uhsType = lhsResult->type;
	uhsValue = lhsResult->value;
	
	switch(type){
		case CGUOT_EXP:{
			if(uhsType == CGVT_DOUBLE){
				newres = expD((CGDouble*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_VECTOR){
				newres = expV((CGVector*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_MATRIX){
				newres = expM((CGMatrix*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
		
		case CGUOT_LOG:{
			if(uhsType == CGVT_DOUBLE){
				newres = logD((CGDouble*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_VECTOR){
				newres = logV((CGVector*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_MATRIX){
				newres = logM((CGMatrix*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
	
		case CGUOT_MINUS:{
			if(uhsType == CGVT_DOUBLE){
				CGDouble* rhs = calloc(1, sizeof(CGDouble));
				rhs->value = -1;
				
				CGResultNode* res = mulDD((CGDouble*)uhsValue, rhs, graph, parentNode);
				
				freeDoubleValue(&rhs);
				
				parentNode->result = res;
				return res;
			}
			
			if(uhsType == CGVT_VECTOR){
				CGDouble* lhs = calloc(1, sizeof(CGDouble));
				lhs->value = -1;
				
				CGResultNode* res = mulDV(lhs, (CGVector*)uhsValue, graph, parentNode);
			
				free(lhs);
				parentNode->result = res;
				return res;
			}
			
			if(uhsType == CGVT_MATRIX){
				CGDouble* lhs = calloc(1, sizeof(CGDouble));
				lhs->value = -1;
				CGResultNode* res = mulDM(lhs, (CGMatrix*)uhsValue, graph, parentNode);
				
				free(lhs);
				
				parentNode->result = res;
				return res;
			}
			break;
		}
		
		case CGUOT_SIN:{
			if(uhsType == CGVT_DOUBLE){
				newres = sinD((CGDouble*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_VECTOR){
				newres = sinV((CGVector*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_MATRIX){
				newres = sinM((CGMatrix*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
		
		case CGUOT_COS:{
			if(uhsType == CGVT_DOUBLE){
				newres = cosD((CGDouble*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_VECTOR){
				newres = cosV((CGVector*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_MATRIX){
				newres = cosM((CGMatrix*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
		
		case CGUOT_TAN:{
			if(uhsType == CGVT_DOUBLE){
				newres = tanD((CGDouble*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_VECTOR){
				newres = tanV((CGVector*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_MATRIX){
				newres = tanM((CGMatrix*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
		
		case CGUOT_TANH:{
			if(uhsType == CGVT_DOUBLE){
				newres = tanhD((CGDouble*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_VECTOR){
				newres = tanhV((CGVector*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_MATRIX){
				newres = tanhM((CGMatrix*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
		
		case CGUOT_INV:{
			char msg[MAX_ERR_FMT_LEN];
			snprintf(msg, MAX_ERR_FMT_LEN, "Operation `%s` is not implemented/supported", getUnaryOperationTypeString(type));
			newres = returnResultError(graph, CGET_OPERATION_NOT_IMPLEMENTED, parentNode, msg);
		}
			
		case CGUOT_TRANSPOSE:{
			
			if(uhsType == CGVT_DOUBLE){
				newres = transposeD((CGDouble*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_VECTOR){
				newres = transposeV((CGVector*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			
			if(uhsType == CGVT_MATRIX){
				newres = transposeM((CGMatrix*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			break;
		}
	
		case CGUOT_RELU:{
			newres = relu(lhsResult);
			parentNode->result = newres;
			return newres;
		}

        case CGUOT_SOFTPLUS:{
            newres = softplus(lhsResult);
            parentNode->result = newres;
            return newres;
        }
	}
	char msg[MAX_ERR_FMT_LEN];
	snprintf(msg, MAX_ERR_FMT_LEN, "Operation [%s %s] cannot be applied", getVariableTypeString(uhsType), getUnaryOperationTypeString(type));
	newres = returnResultError(graph, CGET_INCOMPATIBLE_ARGS_EXCEPTION, parentNode, msg);
	return newres;
}

CGResultNode* processBinaryOperation(CGraph* graph, CGBinaryOperationType type, CGNode* lhs, CGNode* rhs, CGNode* parentNode){
	CGVarType lhsType = CGVT_DOUBLE;
	CGVarType rhsType = CGVT_DOUBLE;
	CGResultNode* newres = NULL;
	void* lhsValue = NULL;
	void* rhsValue = NULL;
	
	CGResultNode* lhsResult = computeCGNode(graph, lhs);
	CHECK_RESULT(lhsResult)
	lhsType = lhsResult->type;
	lhsValue = lhsResult->value;
		
	CGResultNode* rhsResult = computeCGNode(graph, rhs);
	CHECK_RESULT(rhsResult)
	rhsType = rhsResult->type;
	rhsValue = rhsResult->value;
	
	switch(type){
		case CGBOT_ADD:{
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_DOUBLE)){
				newres = addDD((CGDouble*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_DOUBLE)){
				newres = addVD((CGVector*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_VECTOR)){
				newres = addVD((CGVector*)rhsValue, (CGDouble*)lhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_DOUBLE)){
				newres = addMD((CGMatrix*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_MATRIX)){
				newres = addMD((CGMatrix*)rhsValue, (CGDouble*)lhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_VECTOR)){
				newres = addVV((CGVector*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_VECTOR)){
				newres = addMV((CGMatrix*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_MATRIX)){
				newres = addMV((CGMatrix*)rhsValue, (CGVector*)lhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_MATRIX)){
				newres = addMM((CGMatrix*)lhsValue, (CGMatrix*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
		
		case CGBOT_SUB:{
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_DOUBLE)){
				newres = subDD((CGDouble*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_DOUBLE)){
				newres = subVD((CGVector*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_VECTOR)){
				newres = subDV((CGDouble*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_DOUBLE)){
				newres = subMD((CGMatrix*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_MATRIX)){
				newres = subDM((CGDouble*)lhsValue, (CGMatrix*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_VECTOR)){
				newres = subVV((CGVector*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_MATRIX)){
				newres = subMM((CGMatrix*)lhsValue, (CGMatrix*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_VECTOR)){
				newres = subMV((CGMatrix*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_MATRIX)){
				newres = subVM((CGVector*)lhsValue, (CGMatrix*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
		
		case CGBOT_DIV:{
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_DOUBLE)){
				newres = divDD((CGDouble*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_DOUBLE)){
				newres = divVD((CGVector*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_VECTOR)){
				newres = divDV((CGDouble*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_VECTOR)){
				newres = divVV((CGVector*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_DOUBLE)){
				newres = divMD((CGMatrix*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_VECTOR)){
				newres = divMV((CGMatrix*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_MATRIX)){
				newres = divDM((CGDouble*)lhsValue, (CGMatrix*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			break;
		}
		
		case CGBOT_MULT:{
			// TODO: update
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_MATRIX)){
				newres = mulMM((CGMatrix*)lhsValue, (CGMatrix*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			// TODO: update
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_VECTOR)){
				newres = mulMV((CGMatrix*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			// TODO: update
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_MATRIX)){
				newres = mulMV((CGMatrix*)rhsValue, (CGVector*)lhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_DOUBLE)){
				newres = mulDD((CGDouble*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_VECTOR)){
				newres = mulDV((CGDouble*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_DOUBLE)){
				newres = mulDV((CGDouble*)rhsValue, (CGVector*)lhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_MATRIX)){
				newres = mulDM((CGDouble*)lhsValue, (CGMatrix*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_DOUBLE)){
				newres = mulDM((CGDouble*)rhsValue, (CGMatrix*)lhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_VECTOR)){
				newres = crossVV((CGVector*)rhsValue, (CGVector*)lhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
		
		case CGBOT_POW:{
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_DOUBLE)){
				newres = powDD((CGDouble*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_DOUBLE)){
				newres = powVD((CGVector*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_DOUBLE)){
				newres = powMD((CGMatrix*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
		
		case CGBOT_DOT: {
			/*
			 * The following are the same as MUL
			 */
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_DOUBLE)){
				newres = mulDD((CGDouble*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_VECTOR)){
				newres = mulDV((CGDouble*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_DOUBLE)){
				newres = mulDV((CGDouble*)rhsValue, (CGVector*)lhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_MATRIX)){
				newres = mulDM((CGDouble*)lhsValue, (CGMatrix*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_DOUBLE)){
				newres = mulDM((CGDouble*)rhsValue, (CGMatrix*)lhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			/* here starts dot specific impl */
			
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_MATRIX)){
				newres = dotMM((CGMatrix*)lhsValue, (CGMatrix*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_VECTOR)){
				newres = dotMV((CGMatrix*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_MATRIX)){
				newres = dotVM((CGVector*)lhsValue, (CGMatrix*)rhsValue, graph, parentNode);
				//newres = dotMM(vectorToMatrix((CGVector*)lhsValue), (CGMatrix*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			/*
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_MATRIX)){
				newres = mulMV((CGMatrix*)rhsValue, (CGVector*)lhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			*/
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_VECTOR)){
				newres = dotVV((CGVector*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
		
		case CGBOT_TMULT:{
			char msg[MAX_ERR_FMT_LEN];
			snprintf(msg, MAX_ERR_FMT_LEN, "Operation `%s` is not implemented/supported", getBinaryOperationTypeString(type));
			return returnResultError(graph, CGET_OPERATION_NOT_IMPLEMENTED, parentNode, msg);
		}
	}
	
	char msg[MAX_ERR_FMT_LEN];
	snprintf(msg, MAX_ERR_FMT_LEN, "Operation [%s %s %s] cannot be applied", getVariableTypeString(lhsType), getBinaryOperationTypeString(type), getVariableTypeString(rhsType));
	return returnResultError(graph, CGET_INCOMPATIBLE_ARGS_EXCEPTION, parentNode, msg);
}

CGResultNode* computeRawNode(CGNode* node){
	CGraph* tmp_G = makeGraph("temp_g");
	
	tmp_G->root = node;
	storeNodesInGraph(tmp_G, node);
	
	CGResultNode* res = computeGraph(tmp_G);
	res = copyResultNode(res);
	freeGraph(tmp_G);
	free(tmp_G);
	
	return res;
}


CGResultNode* computeCGNode(CGraph* graph, CGNode* node){
	CGResultNode* result = NULL;
	
	if(node->result != NULL){
		return node->result;
	}
	
	switch(node->type){
		case CGNT_CONSTANT:{
			result = constantNodeToResultNodeCopy(node);
			break;
		}

		case CGNT_VARIABLE:{
			if(graph == NULL)
			{
				char msg[MAX_ERR_FMT_LEN];
				snprintf(msg, MAX_ERR_FMT_LEN, "Cannot compute variable`%s` without the graph instance", node->var->name);
				return returnResultError(graph, CGET_NO_GRAPH_INSTANCE, node, msg);
			}
			
			
			CGNode* constantNode = *map_get(&graph->vars, node->var->name);
			if(constantNode == NULL)
			{
				char msg[MAX_ERR_FMT_LEN];
				snprintf(msg, MAX_ERR_FMT_LEN, "No variable `%s` was found in graph `%s`", node->var->name, graph!=NULL?graph->name:"[anonymous]");
				return returnResultError(graph, CGET_VARIABLE_DOES_NOT_EXIST, node, msg);
			}
			
			CGResultNode* rnode = computeCGNode(graph, constantNode);
			CHECK_RESULT(rnode)
			constantNode->result = rnode;
			node->result = copyResultNode(rnode);
			
			result = node->result;
			break;
		}
		case CGNT_BINARY_OPERATION:
			result = processBinaryOperation(graph, node->bop->type, node->bop->lhs, node->bop->rhs, node);
			break;
		case CGNT_UNARY_OPERATION:
			result = processUnaryOperation(graph, node->uop->type, node->uop->uhs, node);
			break;
		
		/* 
		 * TODO: add this test to unittest
		 */
		case CGNT_AXIS_BOUND_OPERATION:
		{
			switch(node->axop->type){
				case CGABOT_SUM:{
					CGResultNode* newres = computeCGNode(graph, node->axop->uhs);
					CHECK_RESULT(newres)
					node->axop->uhs->result = newres;
					
					if(newres->type == CGVT_DOUBLE){
						result = sumD((CGDouble*)newres->value, graph, node);
					}
					
					if(newres->type == CGVT_VECTOR){
						result = sumV((CGVector*)newres->value, graph, node);
					}
					
					if(newres->type == CGVT_MATRIX){
						result = sumM((CGMatrix*)newres->value, graph, node, node->axop->axis);
					}
				
					break;
				}
				
				case CGABOT_MAX:{
					result = max(node, graph);
					break;
				}
				
				case CGABOT_MIN:{
					result = min(node, graph);
					break;
				}
				
				case CGABOT_MEAN:{
					result = mean(node, graph);
					break;
				}

				case CGABOT_SOFTMAX:{
					char msg[MAX_ERR_FMT_LEN];
					snprintf(msg, MAX_ERR_FMT_LEN, "Operation [CGABOT_SOFTMAX] is not implemented/supported");
					return returnResultError(graph, CGET_OPERATION_NOT_IMPLEMENTED, node, msg);
					//CGResultNode* res = computeCGNode(graph, node->axop->uhs);
					//break;
				}

				case CGABOT_ARGMAX:{
				    result = argmax(node, graph);
                    break;
				}

                case CGABOT_ARGMIN:{
                    result = argmin(node, graph);
                    break;
                }

				default:
				    break;
			}
			
			break;
		}
		case CGNT_GRAPH:{
			result = computeGraph(node->graph);
			break;
			/*
			char msg[MAX_ERR_FMT_LEN];
			snprintf(msg, MAX_ERR_FMT_LEN, "Operation [GRAPH] is not implemented/supported");
			return returnResultError(graph, CGET_OPERATION_NOT_IMPLEMENTED, node, msg);
			*/
		}
		
		case CGNT_CROSS_ENTROPY_LOSS_FUNC:
		{
			CGResultNode* X_res = computeCGNode(graph, node->crossEntropyLoss->x);
			CGNode* X = resultNodeToConstantNodeCopy(X_res);
		
			CGNode* X_val = softmax_node(X);
			
						
			CGResultNode* x_res = computeRawNode(X_val);

			CGResultNode* y_res = computeCGNode(graph, node->crossEntropyLoss->y);
			
			result = crossEntropy(x_res, y_res, node->crossEntropyLoss->num_classes);
			
			freeResultNode(x_res);
			free(x_res);
			break;
		}
	}
	
	node->result = reduceDim(result);
	
	
	if(node->diff == NULL)
		switch(node->result->type){
			case CGVT_DOUBLE:
				node->diff = makeZeroDoubleConstantNode();
				break;
			case CGVT_VECTOR:{
				CGVector* v = (CGVector*)node->result ->value;
				node->diff = makeZeroVectorConstantNode(v->len);
				break;
			}
			
			case CGVT_MATRIX:{
				CGMatrix* v = (CGMatrix*)node->result ->value;
				node->diff = makeZeroMatrixConstantNode(v->rows, v->cols);
				break;
			}
		}
	
	return node->result;
}

CGResultNode* reduceDim(CGResultNode* result){
    return result;
    /*
	switch(result->type){
		case CGVT_DOUBLE:{
			return result;
		}
		
		case CGVT_VECTOR:{
			CGVector* vec = (CGVector*)result->value;
			if (vec->len > 1)
				return result;

			copyDataToHost(result);
			
			CGDouble* d = calloc(1, sizeof(CGDouble));
			d->value = vec->data[0];
			
			freeVectorValue(result->value);
			free(result->value);
			
			result->type = CGVT_DOUBLE;
			result->value = d;
			
			return result;
		}
		
		case CGVT_MATRIX:{
			CGMatrix* mat = (CGMatrix*)result->value;
			
			if((mat->rows>1) &&(mat->cols>1))
				return result;
			
			if((mat->rows == 1) && (mat->cols == 1)){
                copyDataToHost(result);

				CGDouble* d = calloc(1, sizeof(CGDouble));
				d->value = mat->data[0];
				
				
				freeMatrixValue(result->value);
				free(result->value);
				
				result->type = CGVT_DOUBLE;
				result->value = d;
				
				return result;
			}
			
			if(mat->rows == 1){
				CGVector* vec = calloc(1, sizeof(CGVector));
				vec->len = mat->cols;
				vec->data = mat->data;
				//TODO: Memory leak here
				vec->buf = mat->buf;

				
				//freeMatrixValue(result->value);
				free(result->value);
				
				result->type = CGVT_VECTOR;
				result->value = vec;
				
				return result;
			}
			
			return result;
		}
	}*/
}

CGResultNode* computeGraph(CGraph* graph){
	resetGraphResultNodes(graph, graph->root);
	CGResultNode* res = computeCGNode(graph, graph->root);
	copyDataToHost(res);

	return res;
}

CGResultNode* computeGraphNode(CGraph* graph, CGNode* node){
	storeNodesInGraph(graph, node);
	resetGraphResultNodes(graph, node);
	return computeCGNode(graph, node);
}


void storeNodesInGraph(CGraph* graph, CGNode* node){
	int idx = -1;
	
	vec_find(&graph->nodes, node, idx);
	
	if(idx != -1)
		return;
	
	vec_push(&graph->nodes, node);
	
	
	switch(node->type){
		case CGNT_CONSTANT:
			break;
		case CGNT_VARIABLE:
			break;
		case CGNT_BINARY_OPERATION:
			storeNodesInGraph(graph, node->bop->lhs);
			storeNodesInGraph(graph, node->bop->rhs);
			break;
		case CGNT_UNARY_OPERATION:
			storeNodesInGraph(graph, node->uop->uhs);
			break;
		case CGNT_AXIS_BOUND_OPERATION:
			storeNodesInGraph(graph, node->axop->uhs);
			break;
		case CGNT_GRAPH:
			storeNodesInGraph(graph, node->graph->root);
			break;
		case CGNT_CROSS_ENTROPY_LOSS_FUNC:
			storeNodesInGraph(graph, node->crossEntropyLoss->x);
			storeNodesInGraph(graph, node->crossEntropyLoss->y);
			break;
	}
}

void resetGraphResultNodes(CGraph* graph, CGNode* node){
	if(node->result != NULL){
		freeResultNode(node->result);
		free(node->result);
		node->result = NULL;
	}
	
	if(node->diff != NULL){
		freeNode(graph, node->diff);
		free(node->diff);
		node->diff = NULL;
	}
	
	switch(node->type){
		case CGNT_CONSTANT:
			break;
		case CGNT_VARIABLE:{
			CGNode* var = graphGetVar(graph, node->var->name);
			if(var != NULL)
				resetGraphResultNodes(graph, var);
			break;
		}
		case CGNT_BINARY_OPERATION:
			resetGraphResultNodes(graph, node->bop->lhs);
			resetGraphResultNodes(graph, node->bop->rhs);
			break;
		case CGNT_UNARY_OPERATION:
			resetGraphResultNodes(graph, node->uop->uhs);
			break;
		case CGNT_AXIS_BOUND_OPERATION:
			resetGraphResultNodes(graph, node->axop->uhs);
			break;
		case CGNT_GRAPH:
			resetGraphResultNodes(graph, node->graph->root);
			break;
		case CGNT_CROSS_ENTROPY_LOSS_FUNC:
			resetGraphResultNodes(graph, node->crossEntropyLoss->x);
			resetGraphResultNodes(graph, node->crossEntropyLoss->y);
			break;
	}
}


void graphSetVar_lua(CGraph* graph, const char* name, CGNode* value){
	CGNode** old = map_get(&graph->vars, name); 
	if(old != NULL){
		//freeNode(graph, *old);
		map_remove(&graph->vars, name);
	}
	
	int res = map_set(&graph->vars, name, value);
}
