#ifndef CG_ERRORS_H
#define CG_ERRORS_H

#include "cgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"

CGResultNode* returnResultError(CGraph* graph, CGErrorType errorType, CGNode* faultyNode);

#endif
