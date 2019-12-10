/*
 * main.c
 *
 * 20493-02 Computer Architecture
 * Term Project on Implentation of Cache Mechanism
 *
 *
 */
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif // _MSC_VER

#include <stdio.h>
#include "cache_impl.h"

int num_cache_hits = 0;
int num_cache_misses = 0;

int num_bytes = 0;
int num_access_cycles = 0;

int global_timestamp = 0;

int retrieve_data(void *addr, char data_type) {	//get data
    int value_returned = -1;
	int i = 0;
    value_returned = check_cache_data_hit(addr, data_type);	//if(cache hit) {return data} else {return null}
	num_access_cycles += CACHE_ACCESS_CYCLE;					//cache cycle++!
	if(-1 != (void*)value_returned) {							//cache hit!
		num_cache_hits++;
	} else {													//cache miss! access_memory()
		num_cache_misses++;
		value_returned = access_memory(addr, data_type);		//get data from memory
		num_access_cycles += MEMORY_ACCESS_CYCLE;						//mem cycle++!
	}

    return value_returned;    
}

int main(void) {
    FILE *ifp = NULL, *ofp = NULL;
    unsigned long int access_addr;	/* byte address (located at 1st column) in "access_input.txt" */
    char access_type;
/* 'b'(byte), 'h'(halfword), or 'w'(word) (located at 2nd column) in "access_input.txt" */
    int accessed_data;
/* This is the data that you want to retrieve first from cache, and then from memory */ 
    
init_memory_content();
    init_cache_content();

    print_cache_entries(); 
    ifp = fopen("access_input2.txt", "r");	//input file name
    if (ifp == NULL) {
        printf("Can't open input file\n");
        return -1;
    }
    ofp = fopen("input2_output3.txt", "w");	//output file name
    if (ofp == NULL) {
        printf("Can't open output file\n");
        fclose(ifp);
        return -1;
    }
    
	if (ifp != NULL) {				//ifp is file read pointer
		fprintf(ofp, "[Accessed Data] \n");
		while(!feof(ifp)) {			//feof returns if ifp is pointing FILE END or not
			fscanf( ifp, "%d", &access_addr);
			while(1) {				//throw ' ' or '\t'
				fscanf( ifp, "%c", &access_type);
				if (' ' != access_type && '\t' != access_type) break;
			}
			fscanf( ifp, "\n");			//read by line
			accessed_data = retrieve_data((void*)access_addr, access_type); //get data
			fprintf(ofp, "%d %c \t 0x%x \n", access_addr, access_type, accessed_data);
//printf to file
		}
	}


	fprintf(ofp, "-----------------------------------------\n");
	switch (DEFAULT_CACHE_ASSOC)
	{
	case 1:
		fprintf(ofp, "[Direct mapped cache performance] \n");
		break;
	case 2:
		fprintf(ofp, "[2-way set accociative cache performance] \n");
		break;
	case 4:
		fprintf(ofp, "[Fully associative cache performance] \n");
		break;
	default:
		break;
	}
	fprintf(ofp, "Hit ratio = %.2f (%d/%d)\n", (float)num_cache_hits/(num_cache_hits + num_cache_misses), num_cache_hits, num_cache_hits + num_cache_misses);
	fprintf(ofp, "Bandwidth = %.2f (%d/%d)\n", (float)num_bytes / num_access_cycles, num_bytes, num_access_cycles);


    fclose(ifp);
    fclose(ofp);
	
    print_cache_entries();
    return 0;
}
