/*
 * Author:  Vivek B. Sardeshmukh 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "HashFunctions.h"

int main(int argc, char* args[]){
    /*create a 10 byte record*/
    unsigned int record[256];
    int i;
    HashFunctionsInit(10);
    for(i = 0; i < 256; i++)
    {
	    record[i] = UINT_MAX;
    }
    for(i = 0; i < HASH_FUNCTION_WIDTH; i++)
	printf("i = %d\t h = %u\n", i, HASH_FUNC[1][i]);
    
    printf("%u\n", HashValue(record, 1));
    return 0;
}

