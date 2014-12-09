/*
 * knnGroundTruth.c
 *
 *  Created on: Dec 8, 2014
 *      Author: Syed Shabih Hasan
 */

#include <stdio.h>
#include <stdlib.h>
#include "knnHeader.h"


unsigned int newRecord[MAX_RECORD*RECORD_LEN];

unsigned int distanceTable[MAX_RECORD][K];
int hammingDistance(unsigned int recordVal[RECORD_LEN], unsigned int queryVal[RECORD_LEN]);
void checkTopElements(int idx, int hammDist);
void writeDistanceTable(char *filename);
unsigned int topElements[K][2];

int main(){
	char filepath[] = ""; // file path goes here

	FILE *fp = fopen(filepath, "r");
	printf("Reading File\n");
	for(unsigned int i=ZERO; i<MAX_RECORD*RECORD_LEN; i++){
//		printf("*1*\n");
		unsigned int temp;
//		printf("*2*\n");
		fread(&temp,sizeof(unsigned int),1,fp);
//		printf("*3*\n");
		newRecord[i] = temp;
//		printf("*4*\n");
	}
	fclose(fp);
	printf("Done!\n");

	for(int i=ZERO; i<MAX_RECORD; i++){
		int recStart = i*RECORD_LEN;
		for(int j=recStart; j< recStart+RECORD_LEN; j++){
//			printf("%x ",newRecord[j]);
		}
//		printf("\n\n");
	}
	int qC = 0;
	for(int i=ZERO; i<MAX_RECORD; i++){
		int rC = 0;
		// get query record
		unsigned int queryRecord[RECORD_LEN];
		int recStart = i*RECORD_LEN;
		for(int j=recStart; j< recStart+RECORD_LEN; j++){
			queryRecord[j-recStart] = newRecord[j];
		}
//		printf("QC = %d\n",qC);
		qC++;
		//initialize the topElements matrix
		for(int i=ZERO; i<K; i++){
			topElements[i][0] = 9999;
			topElements[i][1] = 9999;
		}
		// start with the first element in the records and find the hamming distance
		for(int k=ZERO; k<MAX_RECORD; k++){
			int recStart_inner = k*RECORD_LEN;
			unsigned int recordVal[RECORD_LEN];
			for(int l=recStart_inner; l<recStart_inner+RECORD_LEN; l++){
				recordVal[l-recStart_inner] = newRecord[l];
			}
			int dist = hammingDistance(recordVal, queryRecord);
//			printf("RC = %d\n",rC);
//			printf("HDist = %d\n",dist);
			// once the hamming distance is found, see if it belongs to the top K elements
			if(dist < topElements[K-1][1]){
				checkTopElements(rC, dist);
			}
			rC++;
		}
		// after each query point has gone through all the data, write the results to the main table
		for(int i=ZERO; i<K; i++){
			distanceTable[qC-1][i] = topElements[i][0];
			//printf("%d ",topElements[i][1]);
		}

	}
	// after all query points have gone through all the data, write the table to a file
	writeDistanceTable("");
}

void writeDistanceTable(char *filename){
	printf("I started writing to file\n");
	FILE *fp = fopen(filename, "w");
	for(int i=ZERO; i<MAX_RECORD; i++){
		for(int j=ZERO; j<K; j++){
			printf("%d ",distanceTable[i][j]);
			unsigned int temp = distanceTable[i][j];
			fwrite(&temp,sizeof(unsigned int),1,fp);
		}
		printf("\n");
	}
	fclose(fp);
	printf("Done writing to file\n");
}

void checkTopElements(int idx, int hammDist){
	for(int i=0; i<K; i++){
		if(hammDist < topElements[i][1]){
			for(int j=K-1; j>i; j--){
				topElements[j][0] = topElements[j-1][0];
				topElements[j][1] = topElements[j-1][1];
			}
			topElements[i][0] = idx+1;
			topElements[i][1] = hammDist;
			break;
		}
	}
}

int hammingDistance(unsigned int recordVal[RECORD_LEN], unsigned int queryVal[RECORD_LEN]){
	int hDist = 0;
	for(int i=ONE; i<RECORD_LEN; i++){
		int xRes = recordVal[i] ^ queryVal[i];
//		printf("xRes = %x\n",xRes);
		for(int j=ZERO; j<THIRTY_TWO; j++){
			hDist+= xRes & ONE;
			xRes>>=1;
		}
	}
	return hDist;
}
