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
    //HashFunctionsInit(10);
    srandom(1);
    for(i = 0; i < 256; i++)
    {
	    record[i] = random();
    }
    for(i = 0; i < HASH_FUNCTION_WIDTH; i++)
	printf("i = %d\t h = %u\t rec = %u\n", i, HASH_FUNC[1][i], record[HASH_FUNC[1][i]/32]);

    for(i = 0; i < 8192; i++) 
	printf("i = %d\t bit = %u\n", i, (*record & (1 << i)));

    printf("%u\n", HashValue(record, 1));
    return 0;
}

