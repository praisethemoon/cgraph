#ifndef CGRAPH_CPU_H
#define CGRAPH_CPU_H

#include <inttypes.h>
#include "c_api/cg_enums.h"

#define CG_MAX_CPU_FEATURES CGCPUF_SSE4_2 + 1


typedef struct CGCPUInfo {
	CPUInfoAvailabilty infoAvailable;
	uint8_t flags[CG_MAX_CPU_FEATURES];
	char* vendor;
	char* brand;
	char* codeName;
	int32_t sseSize;
	int32_t numCores;
	int32_t numLogicalCPUs;
	int32_t totalLogicalCPUs;
	int32_t l1DataCache;
	int32_t l1InstructionCache;
	int32_t l2Cache;
	int32_t l3Cache;
	int32_t l4Cache;
} CGCPUInfo;

CGCPUInfo* getCPUInformation();
void printCPUInfo(CGCPUInfo* cpuInfo);


#endif
