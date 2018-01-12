
#include <stdint.h>
#include <stdlib.h>

#include "ccgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"
#include "cg_factory.h"
#include "cg_errors.h"
#include "cg_constants.h"
#include "memory.h"

void dumpNode(CGNode* node){
	fprintf(stderr, "\t\t+Node: '%s'\n", getNodeTypeString(node->type));
	if(node->type == CGNT_CONSTANT){
		fprintf(stderr, "\t\t+Type: '%s'\n", getVariableTypeString(node->constant->type));
	}
}

void throwUnsupportedBinaryOperationException(CGBinaryOperationType type, CGNode* lhs, CGNode* rhs){
	fprintf(stderr, "Unsupported operation %s\n", getBinaryOperationTypeString(type));
	fprintf(stderr, "\tLeft node:\n");
	dumpNode(lhs);
	fprintf(stderr, "\tRight node:\n");
	dumpNode(rhs);
	
	exit(EXIT_FAILURE);
}

CGResultNode* returnResultError(CGErrorType errorType, CGNode* faultyNode){
	fprintf(stderr, "Exception in graph: %s\n", getErrorTypeString(errorType));
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->error = dmt_calloc(1, sizeof(CGError));
	result->error->errorType = errorType;
	result->error->faultyNode = faultyNode;
	
	return result;
}