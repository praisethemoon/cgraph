#ifndef CG_VARIABLES_H
#define CG_VARIABLES_H

#include "cg_types.h"

typedef struct CGPVariable {
	char* name;
	CGVarType type;
	void* value;
} CGPVariable;

typedef struct CGPConstant {
	CGVarType type;
	void* value;
} CGPConstant;

CGPVariable* newVariable(CGVarType type);

#endif
