#ifndef CG_MATH_H
#define CG_MATH_H

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

CGNode* sigmoid(CGNode* x);
CGNode* softmax(CGNode* x, uint8_t axis);


// log(1+ e^x)
CGNode* smoothRelu(CGNode* x);
CGNode* variance(CGNode* X, uint8_t axis);

CGResultNode* relu(CGResultNode* x);
CGNode* dx_relu(CGResultNode* x);


CGResultNode* crossEntropy(CGraph* graph, CGNode* x, CGNode* y, uint64_t num_classes);

#endif
