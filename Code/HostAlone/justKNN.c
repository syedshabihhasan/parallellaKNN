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

    if(argc < 3){
	printf("usage: inputfile k\n");
	exit(-1);
    }
    if(atoi(args[2]) <=0 ){
	fprintf(stderr, "value of k should be non-zero positive\n");
	exit(-1);
    }
    k = (unsigned int)atoi(args[2]);
     ans = malloc(sizeof(unsigned int)*k);
    if(ans == NULL){
	perror("ans in main");
	exit(-1);
    }
    
   queryCompleteRecord = malloc(BYTES_PER_RECORD);
    if(queryCompleteRecord == NULL){
	perror("malloc q");
	exit(-1);
    }
    fin = fopen(args[1], "rb");
    if(fin == NULL){
	perror("fin in main");
	exit(-1);
    }
    srandom(2);
    for(queryID = 1; queryID < 5000; queryID++){
	//fseek(fin, (queryID-1)*BYTES_PER_RECORD, SEEK_SET);
	fread(queryCompleteRecord, BYTES_PER_RECORD, 1, fin);
	/*if(DEBUG){
	    fprintf(stderr, "ID found in file = %u\t ID generated = %u\n", *(unsigned int*)queryCompleteRecord,  queryID);
	    fflush(stderr);
	}
        */
	ret = KNN(args[1], queryCompleteRecord, ans, k);
        fprintf(stdout, "-------query = %u-------\n", queryID);
	for(i = 0; i < ret; i++){
	    fprintf(stdout, "%u : %u\n", i, ans[i]);
	}
        /*
	fprintf(stdout, "Want to shoot another query?(y/n):\n");
	while(1){
	    fscanf(stdin, "%c", &choice);
	    if(toupper(choice) == 'Y' || toupper(choice) == 'N')
		break;
	    else
		fprintf(stdout, "enter y or n\n");
	}
	if(toupper(choice) == 'Y')
	    continue;
	else
	    break;
        */
    }
    fclose(fin);
    return 0;
}

