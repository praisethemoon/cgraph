#ifndef CG_VARIABLES_H
#define CG_VARIABLES_H

#include "cg_types.h"
#include "cg_enums.h"

typedef struct CGPVariable {
	char* name;
} CGPVariable;

typedef struct CGPConstant {
	CGVarType type;
	void* value;
} CGPConstant;

CGPVariable* newVariable(CGVarType type);

#endif
