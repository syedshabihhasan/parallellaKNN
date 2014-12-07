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
    return min+int((max-min+1)*rand()/(RAND_MAX+1.0));
}

void HashFunctionsInit(unsigned int seed){
    unsigned short int i, j;
    srand(seed);
    for(i = 0; i < MAX_HASH_FUNC; i++){
	for(j = 0; j < HASH_FUNC_WIDTH; j++){
	    HASH_FUNC[i][j] = getRandomIndex(0, RECORD_SIZE);
	}
    }
}

unsigned int HashValue(void *record, unsigned int functionNumber){
    unsigned int hashvalue;


    return hashvalue;
}


