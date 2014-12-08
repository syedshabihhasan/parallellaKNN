/*
 * Author:  Vivek B. Sardeshmukh 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "HashFunctions.h"
#include "KNN.h"

void CreateHashTableFiles(){
    int i, j;
    FILE* fp;
    /* create files for buckets */
    char ZERO_BUCKET[BUCKET_SIZE];
    memset(ZERO_BUCKET, 0, BUCKET_SIZE);
    for(i = 0; i < MAX_NUMBER_BUCKETS; i++){
	fp = fopen(i & 7, "ba+");
	fwrite(&ZERO_BUCKET, sizeof(char), BUCKET_SIZE, fp);
	fclose(fp);
    }
}

unsigned int HammingDist(unsigned int *rec, unsigned int *query) {
  unsigned int d;
  unsigned int j;
  unsigned int dist = 0;

  rec++;
  query++;
  for (j = 1; j < WORDS_PER_RECORD; ++j) {
    d = *rec++ ^ *query++;
    while (d > 0) {
      dist += d & 1;
      d >>= 1;
    }
  }
  return dist;
}

void PreprocessLSH(char* inputFile){
    FILE* fin;
    FILE* fhash[MAX_HASH_FUNCTIONS];
    void *record;
    record=malloc(sizeof(char)*BYTES_PER_RECORD);
    if(record==NULL){
	perror("record in LSH");
	exit(-1);
    }
    fin=fopen(inputFile, "rb");
    if(fin == NULL){
	perror("fin");
	exit(-1);
    }
    fread(record, sizeof(char), BYTES_PER_RECORD, fin);
    for(i = 0; i < MAX_HASH_FUNCTIONS; i++){
	

    read a record *record;
    hash *record to 32 functions;
    store only ID (4 bytes of record);
}
int main(int argc, char* args[]){

    return 0;
}

