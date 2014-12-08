/*
 * Author: Vivek B. Sardeshmukh
 */
#ifndef HASHFUNCTIONS_H
#define HASHFUNCTIONS_H

#define DEBUG 1
/*Length of a record in bits*/
#define RECORD_SIZE 8192
/*Number of hash functions*/
#define MAX_HASH_FUNCTION 32
/*Widht of a hash function - these many random index are used*/
#define HASH_FUNCTION_WIDTH 24
/*Array holding random index for each hash function*/
extern unsigned int HASH_FUNC[MAX_HASH_FUNCTION][HASH_FUNCTION_WIDTH]; 
/*init function needs to be called at the begining*/
void HashFunctionsInit(unsigned int seed);

/*returns hashed value of records*/
unsigned int HashValue(void *record, unsigned int functionNumber);


#endif /* ifndef HASHFUNCTIONS_H */

