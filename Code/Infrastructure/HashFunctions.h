/*
 * Author: Vivek B. Sardeshmukh
 */
#ifndef HASHFUNCTIONS_H
#define HASHFUNCTIONS_H

/*Number of hash functions*/
#define MAX_HASH_FUNCTION 32
/*Widht of a hash function - these many random index are used*/
#define HASH_FUNCTION_WIDTH 24
/*Array holding random index for each hash function*/
unsigned int HASH_FUNC[MAX_HASH_FUNC][HASH_FUNC_WIDTH];

/*init function needs to be called at the begining*/
void HashFunctionsInit(unsigned int seed);

/*returns hashed value of records*/
unsigned int HashValue(void *record, unsigned int functionNumber);
#endif /* ifndef HASHFUNCTIONS_H */

