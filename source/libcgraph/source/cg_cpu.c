#ifdef CG_USE_LIBCPUID

#include <inttypes.h>
#include <stdio.h>

#include <string.h>
#include <libcpuid/libcpuid.h>
#include "c_api/cg_enums.h"
#include "cg_cpu.h"

CGCPUInfo* getCPUInformation(){
	CGCPUInfo* cpuInfo = calloc(1, sizeof(CGCPUInfo));
	
	if(!cpuid_present()){
		cpuInfo->infoAvailable = CGCPUINFO_NOT_AVAILABLE;
		return cpuInfo;
	}

	struct cpu_raw_data_t rawData;
	struct cpu_id_t data;
	
	if(cpuid_get_raw_data(&rawData) < 0){
		cpuInfo->infoAvailable = CGCPUINFO_NOT_AVAILABLE;
		return cpuInfo;
	}
	
	if(cpu_identify(&rawData, &data) <0){
		cpuInfo->infoAvailable = CGCPUINFO_NOT_AVAILABLE;
		return cpuInfo;
	}
	
	cpuInfo->infoAvailable = CGCPUINFO_AVAILABLE;
	cpuInfo->vendor = strdup(data.vendor_str);
	cpuInfo->brand = strdup(data.brand_str);
	cpuInfo->codeName = strdup(data.cpu_codename);
	
	cpuInfo->sseSize = data.sse_size;
	cpuInfo->numCores = data.num_cores;
	cpuInfo->numLogicalCPUs = data.num_logical_cpus;
	cpuInfo->totalLogicalCPUs = data.total_logical_cpus;
	cpuInfo->l1DataCache = data.l1_data_cache;
	cpuInfo->l1InstructionCache = data.l1_instruction_cache;
	cpuInfo->l2Cache = data.l2_cache;
	cpuInfo->l3Cache = data.l3_cache;
	cpuInfo->l4Cache = data.l4_cache;
	
	cpuInfo->flags[CGCPUF_MMX] = data.flags[CPU_FEATURE_MMX];
	cpuInfo->flags[CGCPUF_MMX_EXT] = data.flags[CPU_FEATURE_MMXEXT];
	cpuInfo->flags[CGCPUF_SSE] = data.flags[CPU_FEATURE_SSE];
	cpuInfo->flags[CGCPUF_SSE2] = data.flags[CPU_FEATURE_SSE2];
	cpuInfo->flags[CGCPUF_SSE3] = data.flags[CPU_FEATURE_PNI];
	cpuInfo->flags[CGCPUF_3DNOW] = data.flags[CPU_FEATURE_3DNOW];
	cpuInfo->flags[CGCPUF_AVX] = data.flags[CPU_FEATURE_AVX];
	cpuInfo->flags[CGCPUF_SSSE3] = data.flags[CPU_FEATURE_SSSE3];
	cpuInfo->flags[CGCPUF_SSE4_1] = data.flags[CPU_FEATURE_SSE4_1];
	cpuInfo->flags[CGCPUF_SSE4_2] = data.flags[CPU_FEATURE_SSE4_2];
	
	return cpuInfo;
}

void printCPUInfo(CGCPUInfo* cpuInfo){
	if(cpuInfo->infoAvailable == CGCPUINFO_NOT_AVAILABLE){
		printf("Could not detect CPU Infos.\n");
		return;
	}
	
	printf("CPU Vendor: %s\n", cpuInfo->vendor);
	printf("CPU Model: %s\n", cpuInfo->codeName);
	printf("CPU Brand: %s\n", cpuInfo->brand);
	printf("---\n");
	printf("Cores: %"PRId32"\n", cpuInfo->numCores);
	printf("Logical Units: %"PRId32"\n", cpuInfo->numLogicalCPUs);
	printf("Total Logical Units: %"PRId32"\n", cpuInfo->totalLogicalCPUs);
	printf("---\n");
	printf("L1 Data Cache: %"PRId32"\n", cpuInfo->l1DataCache);
	printf("L1 Inst Cache: %"PRId32"\n", cpuInfo->l1InstructionCache);
	printf("L2 Cache: %"PRId32"\n", cpuInfo->l1DataCache);
	printf("L3 Cache: %"PRId32"\n", cpuInfo->l1DataCache);
	printf("L4 Cache: %"PRId32"\n", cpuInfo->l1DataCache);
	printf("---\n");
	printf("MMX    : %s\n", cpuInfo->flags[CGCPUF_MMX]?"OK":"X");
	printf("MMXext : %s\n", cpuInfo->flags[CGCPUF_MMX_EXT]?"OK":"X");
	printf("SSE    : %s\n", cpuInfo->flags[CGCPUF_SSE]?"OK":"X");
	printf("SSE2   : %s\n", cpuInfo->flags[CGCPUF_SSE2]?"OK":"X");
	printf("SSE3   : %s\n", cpuInfo->flags[CGCPUF_SSE3]?"OK":"X");
	printf("3DNOW  : %s\n", cpuInfo->flags[CGCPUF_3DNOW]?"OK":"X");
	printf("AVX    : %s\n", cpuInfo->flags[CGCPUF_AVX]?"OK":"X");
	printf("SSSE3  : %s\n", cpuInfo->flags[CGCPUF_SSSE3]?"OK":"X");
	printf("SSE4.1 : %s\n", cpuInfo->flags[CGCPUF_SSE4_1]?"OK":"X");
	printf("SSE4.2 : %s\n", cpuInfo->flags[CGCPUF_SSE4_2]?"OK":"X");
	
}

#endif