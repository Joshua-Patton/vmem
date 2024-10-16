
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

//      STRUCTURES
struct mem {
    int process_id;
    int page_num;   
    int last_accessed;  // using LRU
};

struct mem *ram[RAM_SIZE];
struct mem *vmem[VMEMSIZE];


//      FUNCTIONS
// initialize page tables, allocate to disc
int page_tables[PROCESS_COUNT][PAGE_COUNT] = {
    {ON_DISC, ON_DISC, ON_DISC, ON_DISC},
    {ON_DISC, ON_DISC, ON_DISC, ON_DISC},
    {ON_DISC, ON_DISC, ON_DISC, ON_DISC},
    {ON_DISC, ON_DISC, ON_DISC, ON_DISC}
};

// ram init function
void init_ram(){
    for (int i = 0; i < RAM_SIZE; i++) {
        ram[i] = NULL;
    }
}

// vmem init func
void init_vmem(){
    for (int process_id = 0;process_id<PROCESS_COUNT;process_id++){
        for (int page_num = 0;page_num<PAGE_COUNT;page_num++){

            struct mem *page = malloc(sizeof(struct mem));  // Create a new page
            page->process_id = process_id;      // Set the process ID
            page->page_num = page_num;        // Set the page number
            page->last_accessed = 0;   // Initialize last accessed time to 0

            vmem[(process_id * 8) + (page_num * 2)] = page;
            vmem[(process_id * 8) + (page_num * 2) + 1] = page;
        }
    }
}

// request page and take it to RAM func
// will need LRU function as helper
void page_request(int process_id, int page_num, int timestep){
    int frame_num = page_tables[process_id][page_num];
    if (frame_num != 99) {
        ram[frame_num]->last_accessed = timestep;  // Update last accessed time
        (timestep)++;  // Increment global time
        return;
    }
}

// LRU func using local and global
// returns an int so we know which frame we are going to overwrite
int LRU(int process_id){
}


// simulation func
// uses page_request
void simulate(char *input_file){
    // Open input file
    FILE* infile = fopen(input_file, "r");
    if (infile == NULL) {
        perror("Error opening input file");
        return 1;
    }


}

// output print func
void print_results(char *output_file){
    FILE* outfile = fopen(output_file, "r");
    if (outfile == NULL) {
        perror("Error opening input file");
        return 1;
    }


}

int main(int argc, char *argv[]){
    if (argc != 3){
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    // initialise memory 
    init_ram();
    init_vmem(); 

    int time = 0;
    simulate(argv[1]);

    print_results(argv[2]);

    return 0;
}