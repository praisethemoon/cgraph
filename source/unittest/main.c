/*
 * 
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include "Remotery.h"
#include "cg_unittests.h"
#include "cg_api.h"

/*
 * Main program
 */
int main(int argc, char *argv[]) {
#ifdef CG_USE_LIBCPUID
	struct CGCPUInfo* cpuInfo = getCPUInformation();
	printCPUInfo(cpuInfo);
#endif
	printf("Running unit tests\n");
	runAllTests();
	return 0;
}
