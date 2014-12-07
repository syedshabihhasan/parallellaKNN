/*
 * Author:  Vivek B. Sardeshmukh 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "HashFunctions.h"


unsigned int getRandomIndex(int min, int max){
    return min + (int)((max-min+1)*rand()/(RAND_MAX+1.0));
}

void HashFunctionsInit(unsigned int seed){
    unsigned short int i, j;
    srand(seed);
    mask[0] = 1;
    for(i = 1; i < RECORD_SIZE; i++)
	mask[i] = 2*mask[i-1]; 
    for(i = 0; i < MAX_HASH_FUNCTION; i++){
	for(j = 0; j < HASH_FUNCTION_WIDTH; j++){
	    HASH_FUNC[i][j] = getRandomIndex(32, RECORD_SIZE);
	}
    }
}

unsigned int HashValue(void *record, unsigned int functionNumber){
    unsigned int hashValue;
    unsigned char maskThis;
    int i;
    maskThis = 0;
    for(i = 0; i < HASH_FUNCTION_WIDTH; i++){
	maskThis = maskThis + mask[HASH_FUNC[functionNumber][i]];
    }
    hashValue = (unsigned int)(*(unsigned int*)record & maskThis);
    return hashValue;
}


