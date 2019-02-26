#ifndef CG_MATH_H
#define CG_MATH_H

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>


#include "cgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"
#include "cg_errors.h"
#include "cg_constants.h"
#include "cg_enums.h"
#include "cg_factory.h"

void __cg_map_array(cg_float* src, cg_float* dest, uint64_t len, cg_float(*f)(cg_float));

cg_float __cg_relu(cg_float x);

cg_float __cg_dx_relu(cg_float x);

cg_float __cg_sigmoid(cg_float x);

cg_float __cg_softplus(cg_float x);

cg_float __cg_dx_softplus(cg_float x);


CGNode* dx_relu(CGResultNode* x);
CGResultNode* softplus(CGResultNode* x);
CGNode* dx_softplus(CGResultNode* x);

CGNode* softmax_node(CGNode* x);
CGResultNode* crossEntropy(CGResultNode* x, CGResultNode* y, uint64_t num_classes);
CGNode* dx_crossEntropy(CGResultNode* x, CGResultNode* y, uint64_t num_classes);
#endif
