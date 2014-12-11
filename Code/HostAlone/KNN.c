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
    char fname[7];
    /* create files for buckets */
    void* ZERO_BUCKET;
    ZERO_BUCKET=malloc(MAX_ITEM_PER_BUCKET*sizeof(unsigned int));
    if(ZERO_BUCKET == NULL){
	perror("Zero_Bucket");
	exit(-1);
    }
    memset(ZERO_BUCKET, 0, MAX_ITEM_PER_BUCKET*sizeof(unsigned int));
    if(DEBUG > 1){
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
    void *recordComplete;
    char fname[7];
    unsigned int i, id;
    unsigned int hashValue, filenum, bucketPosition, itemCount;
    unsigned int overflow;
    FILE* collisionLog;

    collisionLog = fopen("collisionLog.log", "w");
    if(collisionLog == NULL){
	perror("collisionLog");
    }
    recordComplete=malloc(sizeof(char)*BYTES_PER_RECORD);
    if(recordComplete==NULL){
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
	hfile[i] = fopen(fname, "rb+");
	if(hfile[i] == NULL){
	    perror("hfile");
	    exit(-1);
	}
    }
    overflow = 0;
    while(!feof(fin)){
	fread(recordComplete, BYTES_PER_RECORD, 1, fin);
	for(i = 0; i < MAX_HASH_FUNCTION; i++){
	    hashValue = HashValue(recordComplete, i);
	    filenum = hashValue & 7; /*eqv to %8*/
	    bucketPosition = hashValue/8;
	    /*seek to this bucket in the above file*/
	    if(fseek(hfile[filenum], sizeof(unsigned int)*MAX_ITEM_PER_BUCKET*(bucketPosition), SEEK_SET) < 0){
		perror("fseek in hfile");
	    }
            if(DEBUG){
		fprintf(collisionLog, "%u\n", hashValue);
	    }	    /*find out first empty spot for id of this record in this bucket*/
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
		/*if(DEBUG > 0){
		fprintf(stderr, "%u\t%u\n", *(unsigned int*)recordComplete, hashValue);
		fprintf(stdout, "Error: %s:%d: Out of memory for ID = %u with hashValue = %u\n", __FILE__, __LINE__, *(unsigned int*)recordComplete, hashValue);
		}
                */
		overflow++;
		continue;
	    }
	    /*you want to put in this read empty spot.. so go back 4 bytes*/
	    if(fseek(hfile[filenum], -sizeof(unsigned int), SEEK_CUR) < 0){
		perror("fseek in hfile2");
	    }
	    if(fwrite((unsigned int*)recordComplete, sizeof(unsigned int), 1, hfile[filenum]) != 1){
		perror("fwrite");
	    }
	    /*if(DEBUG){
		fprintf(stderr, "record ID=%u\t hashvalue=%u\t filenum=%u\t bucketposition=%u\t eleInBucket=%u\n", \
			*(unsigned int*)recordComplete, hashValue, filenum, bucketPosition, itemCount);
		fprintf(stderr, "%u\t %u\t %u\t %u\t %u\n", \
			*(unsigned int*)recordComplete, hashValue, filenum, bucketPosition, itemCount);
		fflush(stderr);
	    }
	    */
	}
    }
    fprintf(stdout, "number of overflows = %u\n",overflow);
    fclose(fin);
    fclose(collisionLog);
    free(recordComplete);
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

unsigned int getBuckets(void* queryCompleteRecord, unsigned int* temp){
    unsigned int i;
    unsigned int qhash;
    unsigned int tempid;
    unsigned int itemCount;
    unsigned int sofar;
    unsigned int queryID;
    char fname[7];
    FILE *fp;

    queryID = *(unsigned int*)queryCompleteRecord;
    sofar = 0;
    for(i = 0; i < MAX_HASH_FUNCTION; i++){
	qhash = HashValue(queryCompleteRecord, i);
	/*get all elements from this bucket
	 * but first figure out where is this **** - file look up*/
	memset(fname, '\0', 4);
	sprintf(fname, "%u.hash", qhash & 7);
	fp = fopen(fname, "rb");
	if(fp == NULL){
	    perror("qhash file");
	    exit(-1);
	}
	/*seek to qhash bucket*/
	if(fseek(fp, sizeof(unsigned int)*MAX_ITEM_PER_BUCKET*(qhash/8), SEEK_SET) < 0){
	    perror("fseek in fp");
	}
	/*read the bucket till you hit zero entry or reach to the end of bucket*/
	itemCount = 0;
	while(itemCount < MAX_ITEM_PER_BUCKET){
	    fread(&tempid, sizeof(unsigned int), 1, fp);
	    if(tempid == 0)
		break;
	    if(tempid == queryID)
		continue;
	    temp[sofar++] = tempid;
	    if(sofar >= MAX_LOOKUP){
		fprintf(stdout, "Max look up reached\n");
		fclose(fp);
		return sofar - 1;
	    }
	    itemCount++;
	}
	fclose(fp);
    }
    return sofar==0?0:(sofar-1);
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

unsigned int KNN(char* inputfile, void* queryCompleteRecord, unsigned int* ans, unsigned int k){
    unsigned int* lookupBucket;
    unsigned int lookupCount;
    unsigned int i,j;
    struct distRec *distances;
    void *recCompleteRecord;
    FILE *fin;
    unsigned int tempDist;
    unsigned int tempRec;

    if(k == 0 )
	return 0;

    fin = fopen(inputfile, "rb");
    if(fin == NULL){
	perror("fin in knn");
	exit(-1);
    }
    
    recCompleteRecord = malloc(sizeof(char)*BYTES_PER_RECORD);
    if(recCompleteRecord == NULL){
	perror("rec");
	exit(-1);
    }

    lookupBucket = malloc(sizeof(unsigned int)*MAX_LOOKUP);
    if(lookupBucket == NULL){
	perror("look up temp");
	exit(-1);
    }

    lookupCount = getBuckets(queryCompleteRecord, lookupBucket);
    if(DEBUG > 1){
	fprintf(stderr, "%u lookups need to be done\n", lookupCount);
	fflush(stderr);
    }
    distances = malloc(sizeof(struct distRec)*k);
    if(distances == NULL){
	perror("distances struct");
	exit(-1);
    }
    if(k > lookupCount){
	fprintf(stdout, "I don't have that many elements in buckets where I'm hashed. Sorry, I can give only %u elements\n", lookupCount);
	k = lookupCount;
    }
    for(i = 0; i < k; i++){
	/*get this record and compute distance to it*/
	if(fseek(fin, BYTES_PER_RECORD*(lookupBucket[i] - 1), SEEK_SET) < 0){
	    perror("fseek in fin");
	}
	fread(recCompleteRecord, BYTES_PER_RECORD, 1, fin);
	tempDist = HammingDist(recCompleteRecord, queryCompleteRecord);
	distances[i].record = lookupBucket[i];
	distances[i].dist = tempDist;
    }
    qsort(distances, k, sizeof(struct distRec), distcmpfn);
    for(i = k; i < lookupCount; i++){
	/*get this record and compute distance to it*/
	if(fseek(fin, BYTES_PER_RECORD*(lookupBucket[i] - 1), SEEK_SET) < 0){ 
	    perror("fseek in fin 2");
	}
	fread(recCompleteRecord, BYTES_PER_RECORD, 1, fin);
	tempDist = HammingDist(recCompleteRecord, queryCompleteRecord);
	if(distances[k-1].dist < tempDist){
	    continue;
	}
        /*insertion sort*/
	j = k - 1;
	distances[j].dist = tempDist;
	distances[j].record = lookupBucket[i];
	if(j == 0)
	    continue;
	while(distances[j-1].dist > distances[j].dist && j > 0){
	    tempDist = distances[j-1].dist;
	    tempRec = distances[j-1].record;
	    distances[j-1].dist = distances[j].dist;
	    distances[j-1].record = distances[j].record;
	    distances[j].dist = tempDist;
	    distances[j].record = tempRec;
	    j = j - 1;
	    if(j == 0)
		break;
	}
    }

    fclose(fin);

    for(i = 0; i < k; i++)
	ans[i] = distances[i].record;

    free(recCompleteRecord);
    free(lookupBucket);
    free(distances);

    return k;
}




