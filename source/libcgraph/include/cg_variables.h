#ifndef CG_VARIABLES_H
#define CG_VARIABLES_H

#include "cg_types.h"
#include "cg_enums.h"

typedef struct CGVariable {
	char* name;
} CGVariable;

typedef struct CGPConstant {
	CGVarType type;
	void* value;
} CGPConstant;

CGVariable* newVariable(CGVarType type);

#endif
