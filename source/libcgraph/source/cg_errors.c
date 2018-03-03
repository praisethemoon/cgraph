
#include <inttypes.h>
#include <stdlib.h>

#include "cgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"
#include "cg_factory.h"
#include "cg_errors.h"
#include "cg_constants.h"
#include <malloc.h>

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

CGResultNode* returnResultError(CGraph* graph, CGErrorType errorType, CGNode* faultyNode, char* errmsg){
	if(graph)
		fprintf(stderr, "Exception in graph: `%s`: %s\n", graph->name, getErrorTypeString(errorType));
	else
		fprintf(stderr, "Exception in graph (anonymous): %s\n", getErrorTypeString(errorType));
	fprintf(stderr, "%s\n", errmsg);
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->error = calloc(1, sizeof(CGError));
	result->error->errorType = errorType;
	result->error->faultyNode = faultyNode;
	result->error->message = errmsg;
	
	return result;
}
