#ifndef CG_CONSTANTS_H
#define CG_CONSTANTS_H

#include "cgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"

const char* getBinaryOperationTypeString(CGBinaryOperationType type);
const char* getUnaryOperationTypeString(CGUnaryOperationType type);
const char* getNodeTypeString(CGNodeType type);
const char* getVariableTypeString(CGVarType type);
const char* getErrorTypeString(CGErrorType type);

#endif
