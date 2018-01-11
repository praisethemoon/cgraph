
#include <stdint.h>

#include "ccgraph.h"
#include "cg_operation.h"

const char* BinaryOperationTypeString[] = {
	"+",
	"-",
	"*",
	"/",
	"dot",
	"inv",
	"T",
	"exp",
	"log"
};

const char* NodeTypeString[] = {
	"VARIABLE",
	"CONSTANT",
	"GRAPH",
	"BINARY_OPERATION",
	"UNARY_OPERATION"
};

const char* VariableTypeString[] = {
	"DOUBLE",
	"VECTOR",
	"MATRIX"
};



const char* getVariableTypeString(CGVarType type){
	return VariableTypeString[type];
}
