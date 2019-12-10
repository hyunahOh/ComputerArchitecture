/*
 * cache.c
 *
 * 20493-02 Computer Architecture
 * Term Project on Implentation of Cache Mechanism
 *
 *
 */


#include <stdio.h>
#include <string.h>
#include "cache_impl.h"

extern int num_cache_hits;
extern int num_cache_misses;

extern int num_bytes;
extern int num_access_cycles;

extern int global_timestamp;

cache_entry_t cache_array[CACHE_SET_SIZE][DEFAULT_CACHE_ASSOC];
int memory_array[DEFAULT_MEMORY_SIZE_WORD];


/* DO NOT CHANGE THE FOLLOWING FUNCTION */
void init_memory_content() {
    unsigned char sample_upward[16] = {0x001, 0x012, 0x023, 0x034, 0x045, 0x056, 0x067, 0x078, 0x089, 0x09a, 0x0ab, 0x0bc, 0x0cd, 0x0de, 0x0ef};
    unsigned char sample_downward[16] = {0x0fe, 0x0ed, 0x0dc, 0x0cb, 0x0ba, 0x0a9, 0x098, 0x087, 0x076, 0x065, 0x054, 0x043, 0x032, 0x021, 0x010};
    int index, i=0, j=1, gap = 1;
    
    for (index=0; index < DEFAULT_MEMORY_SIZE_WORD; index++) {
        memory_array[index] = (sample_upward[i] << 24) | (sample_upward[j] << 16) | (sample_downward[i] << 8) | (sample_downward[j]);
        if (++i >= 16)
            i = 0;
        if (++j >= 16)
            j = 0;
        
        if (i == 0 && j == i+gap)
            j = i + (++gap);
            
        printf("mem[%d] = %#x\n", index, memory_array[index]);
    }
}   

/* DO NOT CHANGE THE FOLLOWING FUNCTION */
void init_cache_content() {
    int i, j;
    
    for (i=0; i<CACHE_SET_SIZE; i++) {
        for (j=0; j < DEFAULT_CACHE_ASSOC; j++) {
            cache_entry_t *pEntry = &cache_array[i][j];
            pEntry->valid = 0;
            pEntry->tag = -1;
            pEntry->timestamp = 0;
        }
    }
}

/* DO NOT CHANGE THE FOLLOWING FUNCTION */
/* This function is a utility function to print all the cache entries. It will be useful for your debugging */
void print_cache_entries() {
    int i, j, k;
    
    for (i=0; i<CACHE_SET_SIZE; i++) {
        printf("[Set %d] \n", i);
        for (j=0; j <DEFAULT_CACHE_ASSOC; j++) {
            cache_entry_t *pEntry = &cache_array[i][j];
            printf("V: %d \tTag: %#x \tTime: %d \tData: ", pEntry->valid, pEntry->tag, pEntry->timestamp);
            for (k=0; k<DEFAULT_CACHE_BLOCK_SIZE_BYTE; k++) {
                printf("%#x(%d) ", pEntry->data[k], k);
            }
            printf("\t");
        }
        printf("\n");
    }
}

int check_cache_data_hit(void *addr, char type) {		//address = [tag][index][offset]
	unsigned long int block_addr;
	int tag, index, offset;
	int i, data=0;
	int j;//
	cache_entry_t *cache_p;
	char *byte;
	int *word;
	short *half; 

	block_addr = (int) addr / DEFAULT_CACHE_BLOCK_SIZE_BYTE;	//[tag][index][------]
	offset = (int) addr % DEFAULT_CACHE_BLOCK_SIZE_BYTE;	//[---][-----][offset]
	tag = block_addr / CACHE_SET_SIZE;				//[tag][-----][------]
	index = block_addr % CACHE_SET_SIZE;			//[---][index][------]

	global_timestamp++;

	for(i = 0; i < DEFAULT_CACHE_ASSOC; i++) {
		if(tag == cache_array[index][i].tag && 1 == cache_array[index][i].valid) {	//hit check
		cache_array[index][i].timestamp = global_timestamp;	//update last modified time
			cache_p = &cache_array[index][i];
			switch (type) {										//get data as much as you want!
				case 'b' :
				byte = &cache_p->data[offset];
				data = *(byte);
				num_bytes += 1;
				break;
			case 'h' :
				half = (short*)&cache_p->data[offset];
				data = *(half);
				num_bytes += 2;
				break;
			case 'w' :
				word = (int*)&cache_p->data[offset];
				data = *(word);
				num_bytes += 4;
				break;
			default :
				printf("error!!! \n");
				break;
			}//switch
			return data;
		}//if
	}//hit

    return -1; //miss
}

int find_entry_index_in_set(int cache_index) {
    int entry_index=0, i;
	long int min_value = 999999, min_index=0;

	for (i = 0 ; i < DEFAULT_CACHE_ASSOC ; i++) {	//first fill out the cache which valid bit is '0'
		if ( cache_array[cache_index][i].valid == 0) {
			entry_index = i;
			return entry_index;
		}
	}

	for (i = 0; i < DEFAULT_CACHE_ASSOC; i++) {				
//if all valid bit equals '1', find least recently modified cache by checking min(timestamp) 
		if (cache_array[cache_index][i].timestamp < min_value) {
			min_value = cache_array[cache_index][i].timestamp;
			min_index = i;									//find minumum timestamp and return the index as entry_index
		}
	}

	entry_index = min_index;

    return entry_index;
}

int access_memory(void *addr, char type) {
    
	int mem_index, entry_index, i;
    int data = -1;
	char *mem_p;
	int *wp;
	short *hw; 
	unsigned long int block_addr;
	int tag, index, offset;
	int gl;
	block_addr = (int) addr / DEFAULT_CACHE_BLOCK_SIZE_BYTE;
	offset = (int)addr % DEFAULT_CACHE_BLOCK_SIZE_BYTE;
	tag = block_addr / CACHE_SET_SIZE;
	index = block_addr % CACHE_SET_SIZE;
														//mem_array is word size, 'mem_index' value will additionally needed!
	mem_index = block_addr * 2;										//[ block_addr ] << 1
	entry_index = find_entry_index_in_set(index);			//return cache entry index

	mem_p = (char*)memory_array + mem_index*4;

	for (i = 0; i < DEFAULT_CACHE_BLOCK_SIZE_BYTE; i++) {	// set data to cache 
		cache_array[index][entry_index].data[i] = mem_p[i];
	}

	cache_array[index][entry_index].valid = 1;			// update cache status
	cache_array[index][entry_index].tag = tag;
	cache_array[index][entry_index].timestamp = global_timestamp;
	mem_p += offset;

	switch (type) {										//get data as much as you want!
		case 'b' :
			data = *(mem_p);
			num_bytes += 1;
			break;
		case 'h' :
			hw = (short*)mem_p;
			data = *(hw);
			num_bytes += 2;
			break;
		case 'w' :
			wp = (int*)mem_p;
			data = *(wp);
			num_bytes += 4;
			break;
		default :
			printf("error!!! \n");
			break;
	}	

    return data;
}
