/*
*	This is a single header, cross-platform profiler
*
*	Copyright (c) 2016, Johan Yngman
*
*	Permission is hereby granted, free of charge, to any person obtaining a copy
*	of this software and associated documentation files (the "Software"), to deal
*	in the Software without restriction, including without limitation the rights to
*	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
*	the Software, and to permit persons to whom the Software is furnished to do so,
*	subject to the following conditions:
*
*	The above copyright notice and this permission notice shall be included in all
*	copies or substantial portions of the Software.
*
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
*	FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
*	COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
*	IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
*	WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
* Usage: 
*
*	#define PROFILER_DEFINE
*	#include "profiler.h"
*
*	call profiler_initialize() on startup. This function will measure the performance
*	of your cpu for PROFILER_MEASURE_MILLISECONDS milliseconds. This measurement is
*	later used to convert the total cycle count to seconds.
*
*	Use PROFILER_START(name) to start the profiler and PROFILER_STOP(name) to stop.
*	
*	Several start/stop calls can be nestled and the time for all blocks with the
*	same name are combined.
*
*	Call profiler_dump_file(const char* filename) to dump a performance log to file
*	Call profiler_dump_console(const char* filename) to dump a performance log to console
*
*	You can define PROFILER_DISABLE to disable all macros and functions to remove
*	all profiler overhead.
*
*	Author: Johan Yngman (johan.yngman@gmail.com)
*/

#ifndef _PROFILER_
#define _PROFILER_

#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#define PROFILER_NODES_MAX 256
#define PROFILER_NAME_MAXLEN 256
#define PROFILER_BUFFER_SIZE 16384
#define PROFILER_MEASURE_MILLISECONDS 100
#define PROFILER_MEASURE_SECONDS ((float)PROFILER_MEASURE_MILLISECONDS / 1000.0f)

#ifdef PROFILER_DISABLE
#define profiler_initialize()
#define profiler_reset()
#define profiler_get_results(buffer)
#define profiler_dump_file(filename)
#define profiler_dump_console()
#else
void _profiler_initialize();
void _profiler_reset();
void _profiler_get_results(char* buffer);
void _profiler_dump_file(const char* filename);
void _profiler_dump_console();
void _profiler_node_setup(int id, const char* name);

#define profiler_initialize()			_profiler_initialize()
#define profiler_reset()				_profiler_reset()
#define profiler_get_results(buffer)	_profiler_get_results(buffer)
#define profiler_dump_file(filename)	_profiler_dump_file(filename)
#define profiler_dump_console()			_profiler_dump_console()
#endif // PROFILER_DISABLE

#ifdef _WIN32
#include <Windows.h>

#ifdef __MINGW32__
#include <x86intrin.h>
#else
#include <intrin.h>
#endif
static uint64_t get_cycles()
{
	return __rdtsc();
}
static unsigned long get_milliseconds()
{
	LARGE_INTEGER timestamp;
	LARGE_INTEGER frequency;

	QueryPerformanceCounter(&timestamp);
	QueryPerformanceFrequency(&frequency);

	return (unsigned long)(timestamp.QuadPart / (frequency.QuadPart / 1000));
}
#else
#include <sys/time.h>
static uint64_t get_cycles()
{
	unsigned int lo, hi;
	__asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
	return ((uint64_t)hi << 32) | lo;
}
static unsigned long get_milliseconds()
{
	struct timeval time; 
	gettimeofday(&time, NULL);
	unsigned long milliseconds = time.tv_sec * 1000LL + time.tv_usec / 1000;
	return milliseconds;
}
#endif

struct profiler_node
{
	char name[PROFILER_NAME_MAXLEN];
	uint64_t total_cycles;
	int parent_id;
	char is_setup;
};

#ifdef PROFILER_DEFINE
int profiler_current_id = 0;
int profiler_current_parent = -1;
struct profiler_node profiler_nodes[PROFILER_NODES_MAX];

static uint64_t profiler_cycles_measure = 0;
static char buffer[PROFILER_BUFFER_SIZE];

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif

void _profiler_initialize()
{
	profiler_reset();

	unsigned long milliseconds = get_milliseconds();
	uint64_t cycles_start = get_cycles();

	while (get_milliseconds() - milliseconds < PROFILER_MEASURE_MILLISECONDS)
		;

	profiler_cycles_measure = get_cycles() - cycles_start;
}

void _profiler_reset()
{
	int i;
	for (i = 0; i < PROFILER_NODES_MAX; i++)
	{
		profiler_nodes[i].total_cycles = 0;
		profiler_nodes[i].parent_id = -1;
		profiler_nodes[i].is_setup = 0;
		strncpy(profiler_nodes[i].name, "", 1);
	}
}

void _profiler_dump_file(const char* filename)
{
	profiler_get_results(buffer);
	FILE* file = fopen(filename, "w");
	fputs(buffer, file);
	fclose(file);
}

static void profiler_get_results_sorted(char* buffer, int parent_id, float seconds_total, int level)
{
	char buffer_name[PROFILER_NAME_MAXLEN];

	uint64_t max_cycles_ceil = UINT64_MAX;

	int i;
	for (i = 0; i < PROFILER_NODES_MAX; i++)
	{
		uint64_t max_cycles = 0;
		int max_index = -1;

		int j;
		for (j = 0; j < PROFILER_NODES_MAX; j++)
		{
			if (profiler_nodes[j].parent_id == parent_id)
			{
				if (profiler_nodes[j].total_cycles > max_cycles &&
					profiler_nodes[j].total_cycles < max_cycles_ceil)
				{
					max_cycles = profiler_nodes[j].total_cycles;
					max_index = j;
				}
			}
		}

		max_cycles_ceil = max_cycles;

		if (max_index != -1)
		{
			strncpy(buffer_name, "", 1);

			int j;
			for (j = 0; j < level; j++)
				strcat(buffer_name, "    ");

			strcat(buffer_name, profiler_nodes[max_index].name);

			int parent_id = profiler_nodes[max_index].parent_id;
			float seconds = (float)profiler_nodes[max_index].total_cycles / ((float)profiler_cycles_measure / PROFILER_MEASURE_SECONDS);
			float seconds_parent = 0.0f;

			if (parent_id == -1)
			{
				seconds_parent = seconds;
				seconds_total = seconds;
			}
			else
			{
				seconds_parent = (float)profiler_nodes[parent_id].total_cycles / ((float)profiler_cycles_measure / PROFILER_MEASURE_SECONDS);
			}

			float percent_total = 100.0f * (seconds / seconds_total);
			float percent_local = 100.0f * (seconds / seconds_parent);

			sprintf(buffer + strlen(buffer), 
					"%-40s%-7.2f : %-7.2f : %f : %" PRIu64 "\n", 
					buffer_name,
					percent_total,
					percent_local,
					seconds, 
					profiler_nodes[max_index].total_cycles);

			profiler_get_results_sorted(buffer, max_index, seconds_total, level + 1);
		}
		else
		{
			return;
		}
	}
}

void _profiler_get_results(char* buffer)
{
	sprintf(buffer, 
			"%-40s%s : %s : %-8s : %s\n", 
			"Name",
			"\%-total",
			"\%-local",
			"Seconds", 
			"CPU Cycles");

	sprintf(buffer + strlen(buffer), "----------------------------------------------------------------------------------\n");
	profiler_get_results_sorted(buffer, -1, 0.0f, 0);
}

void _profiler_dump_console()
{
	profiler_get_results(buffer);
	printf("%s", buffer);
}

void _profiler_node_setup(int id, const char* name)
{
	strncpy(profiler_nodes[id].name, name, strlen(name) + 1);
	profiler_nodes[id].parent_id = profiler_current_parent;
	profiler_nodes[id].is_setup = 1;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#else
extern int profiler_current_id;
extern int profiler_current_parent;
extern uint64_t profiler_cycles_measure;
extern profiler_node profiler_nodes[PROFILER_NODES_MAX];
#endif // PROFILER_DEFINE

#ifdef PROFILER_DISABLE
#define PROFILER_START(NAME)
#define PROFILER_STOP(NAME)
#else

#ifdef __cplusplus
#define PROFILER_CREATE_ID profiler_current_id++
#else
#define PROFILER_CREATE_ID __COUNTER__
#endif

#define PROFILER_START(NAME) \
	static int __profiler_id_##NAME = PROFILER_CREATE_ID; \
	if( !profiler_nodes[__profiler_id_##NAME].is_setup ) \
		_profiler_node_setup( __profiler_id_##NAME, #NAME ); \
	profiler_current_parent = __profiler_id_##NAME; \
	uint64_t __profiler_start_##NAME = get_cycles(); \

#define PROFILER_STOP(NAME) \
	profiler_nodes[__profiler_id_##NAME].total_cycles += get_cycles() - __profiler_start_##NAME; \
	profiler_current_parent = profiler_nodes[__profiler_id_##NAME].parent_id; \

#endif

#endif //_PROFILER_
