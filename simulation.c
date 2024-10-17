//              Studentid   NAME
// student1     23103979    JOSHUA PATTON

//      IMPORTS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

//      CONSTANTS
#define RAM_SIZE 16
#define PAGE_SIZE 2
#define VMEMSIZE 32
#define PROCESS_COUNT 4
#define PAGE_COUNT 4
#define ON_DISC 99  // Value to indicate a page is in virtual memory (on disk)

//      GLOBAL
int timestep = 0;

// initialize page tables, allocate to disc
int page_tables[PROCESS_COUNT][PAGE_COUNT] = {
    {ON_DISC, ON_DISC, ON_DISC, ON_DISC},
    {ON_DISC, ON_DISC, ON_DISC, ON_DISC},
    {ON_DISC, ON_DISC, ON_DISC, ON_DISC},
    {ON_DISC, ON_DISC, ON_DISC, ON_DISC}
};

//      STRUCTURES
struct mem {
    int process_id;
    int page_num;   
    int last_accessed;  // using LRU
};

struct mem *ram[RAM_SIZE];
struct mem *vmem[VMEMSIZE];

//      FUNCTIONS
// ram init function
void init_ram(){
    for (int i = 0; i < RAM_SIZE; i++) {
        ram[i] = NULL;
    }
}

// vmem init function
void init_vmem(){
    for (int process_id = 0; process_id < PROCESS_COUNT; process_id++) {
        for (int page_num = 0; page_num < PAGE_COUNT; page_num++) {
            struct mem *page = malloc(sizeof(struct mem));  // Create a new page
            page->process_id = process_id;      // Set the process ID
            page->page_num = page_num;        // Set the page number
            page->last_accessed = 0;          // Initialize last accessed time to 0

            vmem[(process_id * 8) + (page_num * 2)] = page;
            vmem[(process_id * 8) + (page_num * 2) + 1] = page;
        }
    }
}

int ram2frame(int ram_index){
    return ram_index;
}

// Load page into RAM
void load_page_to_ram(int process_id, int page_num, int frame) {
    ram[frame] = vmem[(process_id * PAGE_COUNT * 2) + (page_num * 2)];  // Copy from virtual memory to RAM
    ram[frame]->last_accessed = timestep;  // Update last accessed time
    ram[frame+1] = vmem[(process_id * PAGE_COUNT * 2) + (page_num * 2)];  // Copy from virtual memory to RAM
    ram[frame+1]->last_accessed = timestep;  // Update last accessed time
    page_tables[process_id][page_num] = frame/2;  // Update page table
    timestep++;
}


int local_LRU(int process_id) {
    int oldest_time = INT_MAX;
    int oldest_frame = -1;

    for (int page_num = 0; page_num < PAGE_COUNT; page_num++) {
        int frame_num = page_tables[process_id][page_num];

        if (frame_num != ON_DISC && ram[frame_num] != NULL && ram[frame_num]->last_accessed < oldest_time) {
            oldest_time = ram[frame_num]->last_accessed;
            oldest_frame = frame_num;
        }
    }
    if (oldest_frame != -1){
        page_tables[ram[oldest_frame]->process_id][ram[oldest_frame]->page_num] = ON_DISC;
    }  
    return oldest_frame;
}


int global_LRU() {
    int oldest_time = INT_MAX;
    int oldest_frame = -1;

    for (int i = 0; i < RAM_SIZE; i++) {
        if (ram[i] != NULL && ram[i]->last_accessed < oldest_time) {
            oldest_time = ram[i]->last_accessed;
            oldest_frame = i/2;
        }
    }
    page_tables[ram[oldest_frame]->process_id][ram[oldest_frame]->page_num] = ON_DISC;  
    return oldest_frame;
}

int next_process_page(int process_id){
    for (int page_num = 0; page_num < PAGE_COUNT; page_num++){
        if (page_tables[process_id][page_num] == ON_DISC){
            return page_num;
        }
    }
    return -1;
}

void page_request(int process_id) {
    int page_num = next_process_page(process_id);
    int frame_num = page_tables[process_id][page_num];  // Get the current frame number from the page table

    // If the page is already in RAM, update the last accessed time
    if (frame_num != ON_DISC) {
        ram[frame_num]->last_accessed = timestep++;  // Update last accessed time and increment timestep
        return;
    }

    // Check for an empty frame in RAM
    int empty_frame = -1;
    for (int i = 0; i < RAM_SIZE; i++) {
        if (ram[i] == NULL) {
            empty_frame = ram2frame(i);
            break;
        }
    }

    if (empty_frame != -1) {
        // Empty frame found, load the page into the empty frame
        load_page_to_ram(process_id, page_num, empty_frame);
    } else {
        // No empty frames available, proceed to LRU eviction

        // Local LRU policy: Find the least recently used page for this process in RAM
        int local_lru_frame = local_LRU(process_id);

        if (local_lru_frame != -1) {
            // A local LRU page was found, evict that page
            load_page_to_ram(process_id, page_num, local_lru_frame);
        } else {
            // Global LRU policy: Find the least recently used page in all of RAM
            int global_lru_frame = global_LRU();

            // Evict the globally least recently used page
            load_page_to_ram(process_id, page_num, global_lru_frame);
        }
    }
}

// Simulation function (uses page_request)
void simulate(char *input_file) {
    // Open input file
    FILE* infile = fopen(input_file, "r");
    if (infile == NULL) {
        perror("Error opening input file");
        return;
    }

    int process_id;
    while (fscanf(infile, "%d ", &process_id) != EOF) {
        printf("%i\n",timestep);
        page_request(process_id);
    }

    fclose(infile);
}

// Output print function
void print_results(char *output_file) {
    FILE* outfile = fopen(output_file, "w");
    if (outfile == NULL) {
        perror("Error opening output file");
        return;
    }

    // Print page tables
    for (int process_id = 0; process_id < PROCESS_COUNT; process_id++) {
        for (int page_num = 0; page_num < PAGE_COUNT; page_num++) {
            fprintf(outfile, "%d ", page_tables[process_id][page_num]);
        }
        fprintf(outfile, "\n");
    }

    // Print RAM state
    for (int i = 0; i < RAM_SIZE; i++) {
        if (ram[i] != NULL) {
            fprintf(outfile, "%d,%d,%d; ", ram[i]->process_id, ram[i]->page_num, ram[i]->last_accessed);
        } else {
            fprintf(outfile, "empty; ");
        }
    }
    fprintf(outfile, "\n");

    fclose(outfile);
}

// Main function
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    // Initialize memory
    init_ram();
    init_vmem();

    // Simulate the memory management
    simulate(argv[1]);

    // Print the results
    print_results(argv[2]);

    return 0;
}
