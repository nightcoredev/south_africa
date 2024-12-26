#ifndef SYSMON_H
#define SYSMON_H

#include <windows.h>
#include <psapi.h>
#include <time.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>


#define MEMORY_VISUALIZATION_SCALE 10
#define MAX_VISUALIZATION_BLOCKS 50

// Memory-Tracking
typedef struct MemoryBlock {
    void* address;
    size_t size;
    const char* allocation_source;
    struct MemoryBlock* next;
} MemoryBlock;

// tracking memory functions
void initialize_memory_tracking();
void cleanup_memory_tracking();
void* tracked_malloc(size_t size, const char* source);
void tracked_free(void* ptr);
void visualize_memory_usage();
void check_memory_leaks();


void simulate_cpu_load(int duration_seconds, float load_percentage);


void format_memory_size(SIZE_T bytes);
void monitor_process_resources();


#endif 
