#include "systemm.h"

static MemoryBlock* allocations = NULL;
static CRITICAL_SECTION memory_lock;

void initialize_memory_tracking() {
    InitializeCriticalSection(&memory_lock);
}

void cleanup_memory_tracking() {
    DeleteCriticalSection(&memory_lock);
}

void* tracked_malloc(size_t size, const char* source) {
    void* ptr = malloc(size);
    if (ptr) {
        EnterCriticalSection(&memory_lock);
        MemoryBlock* block = malloc(sizeof(MemoryBlock));
        if (block) {
            block->address = ptr;
            block->size = size;
            block->allocation_source = source;
            block->next = allocations;
            allocations = block;
        } else {
            free(ptr);
            ptr = NULL;
        }
        LeaveCriticalSection(&memory_lock);
    }
    return ptr;
}

void tracked_free(void* ptr) {
    EnterCriticalSection(&memory_lock);
    MemoryBlock** current = &allocations;
    while (*current) {
        if ((*current)->address == ptr) {
            MemoryBlock* temp = *current;
            *current = (*current)->next;
            free(temp);
            free(ptr);
            LeaveCriticalSection(&memory_lock);
            return;
        }
        current = &((*current)->next);
    }
    LeaveCriticalSection(&memory_lock);
    fprintf(stderr, "Attempting to free untracked memory %p\n", ptr);
}

void visualize_memory_usage() {
    EnterCriticalSection(&memory_lock);
    printf("\n=== Memory Usage Visualization ===\n");
    printf("Each [*] represents ~%d bytes\n\n", MEMORY_VISUALIZATION_SCALE);

    MemoryBlock* current = allocations;
    int block_number = 1;
    while (current) {
        printf("Block %d:\n", block_number++);
        printf("  Address: %p\n", current->address);
        printf("  Size: %zu bytes\n", current->size);
        printf("  Source: %s (memory was allocated from)\n", current->allocation_source);
        printf("  Memory Map: [");
        size_t blocks_to_print = current->size / MEMORY_VISUALIZATION_SCALE;
        for (size_t i = 0; i < (blocks_to_print < MAX_VISUALIZATION_BLOCKS ? blocks_to_print : MAX_VISUALIZATION_BLOCKS); ++i) {
            printf("*");
        }
        if (blocks_to_print > MAX_VISUALIZATION_BLOCKS) printf("...");
        printf("]\n\n");
        current = current->next;
    }
    LeaveCriticalSection(&memory_lock);
}

void check_memory_leaks() {
    EnterCriticalSection(&memory_lock);
    size_t total_leaks = 0;
    MemoryBlock* current = allocations;
    int leak_number = 1;

    printf("\n=== Memory-Leak-Analysis ===\n");
    if (!current) {
        printf("No memory leaks\n");
    } else {
        printf("checking unfreed memory allocations...\n\n");
        while (current) {
            total_leaks += current->size;
            printf("Leak %d:\n", leak_number++);
            printf("  Address: %p\n", current->address);
            printf("  Size: %zu bytes\n", current->size);
            printf("  Source: %s (memory was allocated but not freed)\n", current->allocation_source);
            printf("\n");
            current = current->next;
        }
        printf("The leaked memory: %zu bytes\n", total_leaks);
    }
    LeaveCriticalSection(&memory_lock);
}

void simulate_cpu_load(int duration_seconds, float load_percentage) {
    clock_t start = clock();
    clock_t target_duration = duration_seconds * CLOCKS_PER_SEC;

    printf("\n=== CPU-Loading-Simulation ===\n");
    printf("Target load: %.1f%%\n", load_percentage);
    printf("Duration: %d seconds\n\n", duration_seconds);

    while ((clock() - start) < target_duration) {
        if (((float)(clock() - start) / target_duration) * 100 < load_percentage) {
            volatile unsigned int i;
            for (i = 0; i < 1000000; i++); // (Adjust for load force)
        } else {
            Sleep(1);
        }
    }
    printf("CPU load simulation done\n");
}

void format_memory_size(SIZE_T bytes) {
    double size = (double)bytes;
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int i = 0;

    while (size >= 1024 && i < 4) {
        size /= 1024;
        i++;
    }

    printf("%.2f %s", size, units[i]);
}

void monitor_process_resources() {
    HANDLE process = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS_EX pmc;
    FILETIME creation_time, exit_time, kernel_time, user_time;

    printf("\n=== -Process-Resource-Usage- ===\n");

    if (GetProcessMemoryInfo(process, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        printf("Memory Statistics:\n");
        printf("  Working Set Size: ");
        format_memory_size(pmc.WorkingSetSize);
        printf(" (Amount of physical memory currently in use by the process)\n");

        printf("  Peak Working Set: ");
        format_memory_size(pmc.PeakWorkingSetSize);
        printf(" (Maximum amount of physical memory used by the process)\n");

        printf("  Page File Usage: ");
        format_memory_size(pmc.PagefileUsage);
        printf(" (Amount of virtual memory used, including disk)\n\n");
    }

    if (GetProcessTimes(process, &creation_time, &exit_time, &kernel_time, &user_time)) {
        ULARGE_INTEGER k_time, u_time;
        k_time.LowPart = kernel_time.dwLowDateTime;
        k_time.HighPart = kernel_time.dwHighDateTime;
        u_time.LowPart = user_time.dwLowDateTime;
        u_time.HighPart = user_time.dwHighDateTime;

        printf("CPU Time-----:\n");
        printf("  Kernel Time: %.3f seconds (Time spent in kernel mode)\n", k_time.QuadPart / 10000000.0);
        printf("  User Time: %.3f seconds (Time spent in user mode)\n", u_time.QuadPart / 10000000.0);
    }
}

int main() {
    initialize_memory_tracking();

    printf("=== Windows-System-Monitor ===\n");
    printf("tool for system resource visualization\n");
    printf("<Enter> to start each simulation...\n");

    getchar(); 

    printf("\nRunning memory allocation simulation...\n");
    void* mem1 = tracked_malloc(1000, "Main Function");
    if (!mem1) { fprintf(stderr, "Allocation failed for mem1\n"); return 1; }
    void* mem2 = tracked_malloc(2000, "Data Processing");
    if (!mem2) { fprintf(stderr, "Allocation failed for mem2\n"); return 1; }
    void* mem3 = tracked_malloc(500, "GUI Module");
    if (!mem3) { fprintf(stderr, "Allocation failed for mem3\n"); return 1; }
    visualize_memory_usage();
    fflush(stdout);

    printf("\nRunning CPU scheduling simulation...\n");
    simulate_cpu_load(2, 75.0);
    fflush(stdout);

    printf("\nRunning process monitoring simulation...\n"); 
    monitor_process_resources();
    fflush(stdout);

    printf("\n(simulating a leak)...\n"); 
    tracked_free(mem1);
    tracked_free(mem3);
    check_memory_leaks();
    fflush(stdout);

    cleanup_memory_tracking();

    printf("\nSimulation over Press Enter to exit...\n");
    getchar(); 
    return 0;
}
