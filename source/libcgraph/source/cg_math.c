
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <malloc.h>

#include "cgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"
#include "cg_errors.h"
#include "cg_constants.h"
#include "cg_enums.h"
#include "cg_factory.h"
#include "cg_math.h"


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

CGResultNode* crossEntropy(CGraph* graph, CGNode* X, CGNode* Y, uint64_t num_classes){
	CGResultNode* x = computeCGNode(graph, x);
	CGResultNode* y = computeCGNode(graph, y);
	
	if(X->constant->type != CGVT_MATRIX){
		exit(-1);
	}
	
	double* res = calloc(num_classes, sizeof(double));
	
	uint64_t i = 0;
	
	for(;i<num_classes;i++){
		//res[i]
	}
	
	return NULL;
}
