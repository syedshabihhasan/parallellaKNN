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

struct distRec 
{
    unsigned int record;
    unsigned int dist;
};

void CreateHashTableFiles(){
    unsigned int i;
    FILE* fp;
    char fname[4];
    /* create files for buckets */
    void* ZERO_BUCKET;
    ZERO_BUCKET=malloc(MAX_ITEM_PER_BUCKET*sizeof(unsigned int));
    if(ZERO_BUCKET == NULL){
	perror("Zero_Bucket");
	exit(-1);
    }
    memset(ZERO_BUCKET, 0, MAX_ITEM_PER_BUCKET*sizeof(unsigned int));
    if(DEBUG){
	for(i = 0; i < MAX_ITEM_PER_BUCKET; i++)
	    fprintf(stderr, "%x\n", *(unsigned int*)(ZERO_BUCKET + i*(sizeof(unsigned int))));
	fflush(stderr);
    }
    for(i = 0; i < MAX_NUMBER_BUCKETS; i++){
	memset(fname, '\0', 4);
	sprintf(fname, "%u.hash", i & 7);
	fp = fopen(fname, "ab+");
	if(fp == NULL){
	    perror("zero file");
	    exit(-1);
	}
	fwrite(ZERO_BUCKET, sizeof(unsigned int), MAX_ITEM_PER_BUCKET, fp);
	fclose(fp);
    }
    free(ZERO_BUCKET);
    return;
}


/* read input file
 * for each record - hash the record to 32 buckets by using 32 hash functions
 * store ID of record in corresponding file and location
 */
void PreprocessLSH(char* inputFile){
    FILE* fin;
    FILE* hfile[8];
    void *record;
    char fname[4];
    unsigned int i, id;
    unsigned int hashValue, filenum, bucketPosition, itemCount;
    unsigned int overflow;

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
    for(i = 0; i < 8; i++){
	memset(fname, '\0', 4);
	sprintf(fname, "%u.hash", i);
	hfile[i] = fopen(fname, "ab+");
	if(hfile[i] == NULL){
	    perror("hfile");
	    exit(-1);
	}
    }
    overflow = 0;
    while(!feof(fin)){
	fread(record, sizeof(char), BYTES_PER_RECORD, fin);
	for(i = 0; i < MAX_HASH_FUNCTION; i++){
	    hashValue = HashValue(record, i);
	    filenum = hashValue & 7; /*eqv to %7*/
	    bucketPosition = hashValue/8;
	    /*seek to this bucket in the above file*/
	    fseek(hfile[filenum], sizeof(unsigned int)*MAX_ITEM_PER_BUCKET*(bucketPosition), SEEK_SET);
	    /*find out first empty spot for id of this record in this bucket*/
	    itemCount = 1;
	    while(itemCount <= MAX_ITEM_PER_BUCKET){
		fread(&id, sizeof(unsigned int), 1, hfile[filenum]);
		if(id == 0){
		    /*empty record id*/
		    break;
		}
		itemCount++;
	    }
	    if(id != 0){
		fprintf(stderr, "Error: %s:%d: Out of memory for %u th item in a bucket\n", __FILE__, __LINE__, *(unsigned int*)record);
		overflow++;
		continue;
	    }
	    /*you want to put in this read empty spot.. so go back 4 bytes*/
	    fseek(hfile[filenum], -sizeof(unsigned int), SEEK_CUR);
	    fwrite(record, sizeof(unsigned int), 1, hfile[filenum]); 
	}
    }
    fprintf(stderr, "number of overflows = %u\n",overflow); 
}

unsigned int HammingDist(unsigned int *rec, unsigned int *query) {
  unsigned int d;
  unsigned int j;
  unsigned int dist = 0;

  rec++;
  query++;
  for (j = 1; j < BYTES_PER_RECORD; ++j) {
    d = *rec++ ^ *query++;
    while (d > 0) {
      dist += d & 1;
      d >>= 1;
    }
  }
  return dist;
}

unsigned int getBuckets(unsigned int* query, unsigned int* temp){
    unsigned int i;
    unsigned int qhash;
    unsigned int tempid;
    unsigned int itemCount;
    unsigned int sofar;
    char fname[4];
    FILE *fp;

    sofar = 0;
    for(i = 0; i < MAX_HASH_FUNCTION; i++){
	qhash = HashValue(query, i);
	/*get all elements from this bucket
	 * but first figure out where is this **** - file look up*/
	memset(fname, '\0', 4);
	sprintf(fname, "%u.hash", qhash & 7);
	fp = fopen(fname, "ab+");
	if(fp == NULL){
	    perror("qhash file");
	    exit(-1);
	}
	/*seek to qhash bucket*/
	fseek(fp, sizeof(unsigned int)*MAX_ITEM_PER_BUCKET*(qhash/8), SEEK_SET);
	/*read the bucket till you hit zero entry or reach to the end of bucket*/
	itemCount = 0;
	while(itemCount < MAX_ITEM_PER_BUCKET){
	    fread(&tempid, sizeof(unsigned int), 1, fp);
	    if(tempid == 0)
		break;
	    if(tempid == *query)
		continue;
	    temp[sofar++] = tempid;
	    if(sofar >= MAX_LOOKUP){
		fprintf(stderr, "Max look up reached\n");
		break;
	    }
	    itemCount++;
	}
	fclose(fp);
    }
    return (sofar - 1);
}
/*
 * @func distcmpfn()
 * comparator function for qsort
 */
int distcmpfn(const void *a, const void *b) 
{ 
    const struct distRec *sa = a; // casting pointer types 
    const struct distRec *sb = b;
    return (sa->dist - sb->dist);  
}

unsigned int KNN(char* inputfile, unsigned int* query, unsigned int* ans, unsigned int k){
    unsigned int* lookupBucket;
    unsigned int lookupCount;
    unsigned int i;
    struct distRec *distances;
    void *rec;
    FILE *fin;

    if(k == 0)
	return 0;

    fin = fopen(inputfile, "rb");
    if(fin == NULL){
	perror("fin in knn");
	exit(-1);
    }
    
    rec = malloc(sizeof(char)*BYTES_PER_RECORD);
    if(rec == NULL){
	perror("rec");
	exit(-1);
    }

    lookupBucket = malloc(sizeof(unsigned int)*MAX_LOOKUP);
    if(lookupBucket == NULL){
	perror("look up temp");
	exit(-1);
    }

    lookupCount = getBuckets(query, lookupBucket);
    distances = malloc(sizeof(struct distRec)*lookupCount);
    if(distances == NULL){
	perror("distances struct");
	exit(-1);
    }
    for(i = 0; i < lookupCount; i++){
	distances[i].record = lookupBucket[i];
	/*get this record and compute distance to it*/
	fseek(fin, BYTES_PER_RECORD*(lookupBucket[i] - 1), SEEK_CUR);
	fread(rec, BYTES_PER_RECORD, 1, fin);
	distances[i].dist = HammingDist(rec, query);
    }
    fclose(fin);
    qsort(distances, lookupCount, sizeof(struct distRec), distcmpfn);
    
    if(k > lookupCount){
	fprintf(stderr, "I don't have that many elements in buckets where I'm hashed. Sorry, I can give only %u elements\n", lookupCount);
	k = lookupCount;
    }
    for(i = 0; i < k; i++)
	ans[i] = distances[i].record;

    return k;
}

	


