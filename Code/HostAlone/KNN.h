/*
 * $Id$
 */
#ifndef KNN_H
#define KNN_H

#define BYTES_PER_RECORD 1024
/*how many records per bucket (max #collisons)*/ 
#define MAX_ITEM_PER_BUCKET 64 
/*do at most these many look-ups*/
#define MAX_LOOKUP 500
/*how many total buckets - 2^width of hash function*/ 
#define MAX_NUMBER_BUCKETS  16777216

/*MUST CALL THESE 2 functions*/
void CreateHashTableFiles();
void PreprocessLSH(char* inputfile);
/*SHOOT THIS FUNCTION ONLY AFTER CALLING ABOVE 2 FUNCTIONS*/
unsigned int KNN(char* inputfile, void* queryCompleteRecord, unsigned int* ans, unsigned int k);
/*unsigned int getBuckets(unsigned int* query, unsigned int* ans);
unsigned int HammingDist(unsigned int* rec, unsigned int* query);*/

#endif /* ifndef KNN_H */

