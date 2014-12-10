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
#define MAX_RECORDS 256

int main(int argc, char* args[]){

    unsigned int i, k, ret, queryID;
    unsigned int* ans;
    void* query;
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

    srandom(2);
    system("rm -f *.hash");
    CreateHashTableFiles();
    PreprocessLSH(args[1]);

    if(DEBUG){
	fprintf(stderr, "Preprocess complete\n\n\n");
	fflush(stderr);
    }

    query = malloc(BYTES_PER_RECORD);
    if(query == NULL){
	perror("malloc q");
	exit(-1);
    }
    fin = fopen(args[1], "rb");
    if(fin == NULL){
	perror("fin in main");
	exit(-1);
    }
   while(1){
	queryID = getRandomIndex(1, MAX_RECORDS-1);
	fseek(fin, (queryID-1)*sizeof(unsigned int), SEEK_SET);
	fread(&query, sizeof(unsigned int), 1, fin);
	if(DEBUG){
	    fprintf(stderr, "ID found in file = %u\t ID generated = %u\n", *(unsigned int*)query,  queryID);
	    fflush(stderr);
	}
	ret = KNN(args[1], query, ans, k);
	fprintf(stdout, "-------query = %u-------\n", queryID);
	for(i = 0; i < ret; i++){
	    fprintf(stdout, "%u : %u\n", i, ans[i]);
	}
	fprintf(stdout, "Want to shoot another query?(y/n):\n");
	fscanf(stdin, "%c", &choice);
	if(toupper(choice) == 'Y')
	    continue;
	else
	    break;
    }
    fclose(fin);
    return 0;
}

