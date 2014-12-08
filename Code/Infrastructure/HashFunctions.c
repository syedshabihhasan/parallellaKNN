/*
 * Author:  Vivek B. Sardeshmukh 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "HashFunctions.h"

/*
 * @func cmpfn()
 * comparator function for qsort
 */
int cmpfn(const void *a, const void *b) 
{ 
    const unsigned int *ia = (const unsigned int *)a; // casting pointer types 
    const unsigned int *ib = (const unsigned int *)b;
    return *ia  - *ib; 
}

unsigned int getRandomIndex(unsigned int min, unsigned int max){
    int r;
    const unsigned int range = max - min;
    const unsigned int buckets = RAND_MAX / range;
    const unsigned int limit = buckets * range;
    do
    {
	r = random();
    } while (r >= limit);

    return min + (r / buckets);
}

void HashFunctionsInit(unsigned int seed){
    unsigned int i, j;
    srandom(seed);
    for(i = 0; i < MAX_HASH_FUNCTION; i++){
	for(j = 0; j < HASH_FUNCTION_WIDTH; j++){
	    HASH_FUNC[i][j] = getRandomIndex(32, RECORD_SIZE);
	}
	qsort(HASH_FUNC[i], HASH_FUNCTION_WIDTH, sizeof(unsigned int), cmpfn);
    }
}

unsigned int HashValue(void *record, unsigned int functionNumber){
    unsigned int hashValue;
    int i;
    hashValue = 0;
    for(i = 0; i < HASH_FUNCTION_WIDTH; i++){
	if(*(unsigned int*)record & (1 << HASH_FUNC[functionNumber][i])){
	    hashValue |= 1 << i;
	}
    }
    return hashValue;
}


