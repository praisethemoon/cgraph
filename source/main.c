/*
 * 
 * 
 */

#include <stdlib.h>
#include <stdio.h>

#include "cgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"
#include "cg_factory.h"
#include "cg_constants.h"
#include "cg_diff.h"
#include "cg_enums.h"

#include "memory.h"

#define PROFILER_DEFINE
#include "profiler.h"


/*
 * Main program
 */
int main(int argc, char *argv[]) {
	printf("Running unit tests\n");
	runAllTests();
	return 0;
}
