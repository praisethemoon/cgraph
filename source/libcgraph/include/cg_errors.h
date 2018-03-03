#ifndef CG_ERRORS_H
#define CG_ERRORS_H

#include "cgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"

#define MAX_ERR_FMT_LEN 1024

CGResultNode* returnResultError(CGraph* graph, CGErrorType errorType, CGNode* faultyNode, char* errmsg);

#endif
