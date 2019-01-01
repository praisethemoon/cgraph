
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <malloc.h>
#include <math.h>

#include "cgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"
#include "cg_errors.h"
#include "cg_constants.h"
#include "cg_enums.h"
#include "cg_factory.h"
#include "cg_math.h"

void __cg_map_array(double* src, double* dest, uint64_t len, double(*f)(double)){
	uint64_t i = 0;
	for(;i<len;i++)
		dest[i] = f(src[i]);
}

double __cg_relu(double x){
	return x>0?x:0;
}

double __cg_dx_relu(double x){
	return x>0?1:0;
}


CGNode* sigmoid(CGNode* x){
	return NULL;
}

/*
 * A numerically stable version of softmax
 * https://deepnotes.io/softmax-crossentropy
 */
CGNode* softmax(CGNode* x, uint8_t axis){
	return NULL;
}

/*
 * TODO: Test this highly unoptimized crap
 */
CGResultNode* crossEntropy(CGraph* graph, CGNode* X, CGNode* Y, uint64_t num_classes){
	CGResultNode* x = computeCGNode(graph, X);
	CGResultNode* y = computeCGNode(graph, Y);
	
	CGVector* y_vec = NULL;
	if(y->type == CGVT_DOUBLE){
		CGDouble* raw_val = (CGDouble*)y->value;
		y_vec = (CGVector*)makeVectorConstantNode(1, &raw_val->value)->constant->value;
	}
	else
	 
		y_vec= (CGVector*)y->value;
	
	uint64_t num_samples = 1;
	double* x_val = NULL;
	double* y_val = y_vec->data;
	
	if(X->constant->type == CGVT_MATRIX){
		CGMatrix* m = (CGMatrix*)x->value;
		num_samples = m->rows;
		x_val = m->data;
	}
	else{
		CGVector* m = (CGVector*)x->value;
		num_samples = 1;
		x_val = m->data;
	}
	
	uint64_t i = 0;
	uint64_t j = 0;
	double sum = 0.0;
	
	//printf("num samples %lu, %lu\n", num_samples, num_classes);
	
	for(;i<num_samples;i++){
		for(j=0;j<num_classes;j++){
			//printf("%lu vs %f\n", j, y_val[i]);
			if(y_val[i] == j){
				//printf("true %f => %f\n", x_val[i*num_classes+j], log(x_val[i*num_classes+j]));
				sum -= log(x_val[i*num_classes+j]);
			}
			else{
				//printf("false %f => %f\n", 1.0-x_val[i*num_classes+j], log(x_val[i*num_classes+j]));
				sum -= log(1.0-x_val[i*num_classes+j]);
			}
		}
	}
	
	sum = -sum/num_samples;
	
	return makeDoubleResultNode(sum);
}

CGResultNode* relu(CGResultNode* x){
	switch(x->type){
		case CGVT_DOUBLE:{
			double val = ((CGDouble*)x->value)->value;
			return makeDoubleResultNode(__cg_relu(val));
		}
		
		case CGVT_VECTOR: {
			CGVector* v = (CGVector*)x->value;
			
			double* y = calloc(v->len, sizeof(double));
			__cg_map_array(v->data, y, v->len, __cg_relu);
			return makeVectorResultNode(v->len, y);
		}
		
		case CGVT_MATRIX: {
			CGMatrix* m = (CGMatrix*)x->value;
			uint64_t len = m->cols*m->rows;
			
			double* y = calloc(len, sizeof(double));
			__cg_map_array(m->data, y, len, __cg_relu);
			return makeMatrixResultNode(m->rows, m->cols, y);
		}
	}
}

/*
 * Used in back-prop
 */
CGNode* dx_relu(CGResultNode* x){
	switch(x->type){
		case CGVT_DOUBLE:{
			double val = ((CGDouble*)x->value)->value;
			return makeDoubleConstantNode(__cg_dx_relu(val));
		}
		
		case CGVT_VECTOR: {
			CGVector* v = (CGVector*)x->value;
			
			double* y = calloc(v->len, sizeof(double));
			__cg_map_array(v->data, y, v->len, __cg_dx_relu);
			return makeVectorConstantNode(v->len, y);
		}
		
		case CGVT_MATRIX: {
			CGMatrix* m = (CGMatrix*)x->value;
			uint64_t len = m->cols*m->rows;
			
			double* y = calloc(len, sizeof(double));
			__cg_map_array(m->data, y, len, __cg_dx_relu);
			return makeMatrixConstantNode(m->rows, m->cols, y);
		}
	}
}
