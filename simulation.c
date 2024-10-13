#include <stdio.h>
#include <stdlib.h>

#define RAM_SIZE 16
#define PAGE_SIZE 2
#define VMEMSIZE 32
#define PROCESS_COUNT 4
#define PAGE_COUNT 4
#define ON_DISC 99  // Value to indicate a page is in virtual memory (on disk)

struct mem {
    int process_id;
    int page_num;
    int last_accessed;  // using LRU
};

struct mem *ram[RAM_SIZE];
struct mem *vmem[VMEMSIZE] 

// initialize page tables, allocate to disc
int page_tables[PROCESS_COUNT][PAGE_COUNT] = {
    {ON_DISC, ON_DISC, ON_DISC, ON_DISC},
    {ON_DISC, ON_DISC, ON_DISC, ON_DISC},
    {ON_DISC, ON_DISC, ON_DISC, ON_DISC},
    {ON_DISC, ON_DISC, ON_DISC, ON_DISC}
};


// vmem init func
void init_vmem(){}

// request page and take it to RAM func
// will need LRU function as helper
void page_request(int process_id, int page_num, int timestep){}

// LRU func using local and global
// returns an int so we know which frame we are going to overwrite
int LRU(int process_id){}

// simulation func
// uses page_request
void simulate(char *input_file){}

// output print func
void print_results(char *output_file){}

int main(int argc, char *argv[]){
    if (argc != 3){
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    init_vmem();
    simulate(argv[1]);
    print_results(argv[2]);

    return 0;
}