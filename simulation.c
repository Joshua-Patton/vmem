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

// LRU eviction function
int evict_LRU() {
    int oldest_time = INT_MAX;
    int oldest_frame = -1;

    for (int i = 0; i < RAM_SIZE; i++) {
        if (ram[i] != NULL && ram[i]->last_accessed < oldest_time) {
            oldest_frame = i;
            oldest_time = ram[i]->last_accessed;
        }
    }

    return oldest_frame;  // Return the frame number of the least recently used page
}

// Load page into RAM
void load_page_to_ram(int process_id, int page_num, int frame) {
    ram[frame] = vmem[(process_id * PAGE_COUNT * 2) + (page_num * 2)];  // Copy from virtual memory to RAM
    ram[frame]->last_accessed = timestep;  // Update last accessed time
    page_tables[process_id][page_num] = frame;  // Update page table
}

// Request page and bring it to RAM
void page_request(int process_id, int page_num) {
    int frame_num = page_tables[process_id][page_num];

    if (frame_num != ON_DISC) {
        // Page is already in RAM
        ram[frame_num]->last_accessed = timestep;  // Update last accessed time
        timestep++;  // Increment global time
        return;
    } else {
        // Page is not in RAM, find space or evict a page
        int empty_frame = -1;
        for (int i = 0; i < RAM_SIZE; i++) {
            if (ram[i] == NULL) {
                empty_frame = i;
                break;
            }
        }

        if (empty_frame != -1) {
            // We found an empty frame, load the page there
            load_page_to_ram(process_id, page_num, empty_frame);
        } else {
            // No empty frame, evict a page using LRU
            int evicted_frame = evict_LRU();
            load_page_to_ram(process_id, page_num, evicted_frame);
        }
        timestep++;  // Increment global time after loading a new page
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

    int process_id, page_num;
    while (fscanf(infile, "%d %d", &process_id, &page_num) != EOF) {
        page_request(process_id, page_num);
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
