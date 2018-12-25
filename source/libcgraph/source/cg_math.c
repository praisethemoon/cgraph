
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
 * A numerical stable version of softmax
 */
CGNode* softmax(CGNode* x){
	CGNode* lhs = makeUnaryOpNode(CGUOT_EXP, makeBinaryOpNode(CGUOT_MINUS, x, makeAxisBoundNode(CGABOT_MAX, x, 0)));
	CGNode* rhs = makeAxisBoundNode(CGABOT_SUM, x, 0);
	return NULL;
}

CGNode* crossEntropy(CGNode* x, CGNode* y){
	return NULL;
}
