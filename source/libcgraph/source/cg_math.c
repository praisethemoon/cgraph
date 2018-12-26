
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
	CGNode* lhs = makeUnaryOpNode(CGUOT_EXP, makeBinaryOpNode(CGUOT_MINUS, x, makeAxisBoundNode(CGABOT_MAX, x, axis)));
	CGNode* rhs = makeAxisBoundNode(CGABOT_SUM, lhs, axis);
	CGNode* ns_softmax = makeBinaryOpNode(CGBOT_DIV, lhs, rhs);

	return ns_softmax;
}

CGNode* crossEntropy(CGNode* x, CGNode* y){
	return NULL;
}
