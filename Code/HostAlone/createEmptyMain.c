/*
 * Author:  Vivek B. Sardeshmukh 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "HashFunctions.h" 
#include "KNN.h"
#define MAX_RECORDS 997747 

int main(int argc, char* args[]){

    unsigned int i, k, ret, queryID;
    unsigned int* ans;
    void* queryCompleteRecord;
    FILE* fin;
    char choice;

    srandom(2);
    system("rm -f *.hash");
    CreateHashTableFiles();
    if(DEBUG){
	fprintf(stderr, "Creation of empty hashtables done\n");
	fflush(stderr);
    }
    return 0;
}

