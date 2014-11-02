#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#define MB_TO_WRITE 10
#define N 30

int main(int argc, char *argv[]){
	int totalBytesToWrite = MB_TO_WRITE*1024*1024;
	int totalInts = totalBytesToWrite/sizeof(int);
	int valsToWrite[] = {0xffffffff, 0x00000000, 0xaaaaaaaa,0x55555555};
	long int timeTaken[4][N];
	long int timeTakenRead[4][N];
	for(int j=0; j<N; j++){
		for(int i=0; i<4; i++){
			int *toWrite = (int*)malloc(sizeof(int)*totalInts);
			for(int k=0; k<totalInts; k++){
				toWrite[k] = valsToWrite[i];
			}
			struct timeval startTime, endTime, startRead, endRead;
			FILE *fp;
			// write time
			gettimeofday(&startTime, NULL);
			fp = fopen("./test.bin","wb");
			fwrite(toWrite,sizeof(int),totalInts,fp);
			fclose(fp);
			gettimeofday(&endTime, NULL);
			timeTaken[i][j] = ((endTime.tv_sec*1000000 + endTime.tv_usec) - (startTime.tv_sec*1000000 + startTime.tv_usec));
			free(toWrite);
			// read time
			toWrite = (int*)malloc(sizeof(int)*totalInts);
			gettimeofday(&startRead, NULL);
			fp = fopen("./test.bin","rb");
			fread(toWrite,sizeof(int),totalInts,fp);
			fclose(fp);
			gettimeofday(&endRead, NULL);
			timeTakenRead[i][j] = ((endRead.tv_sec*1000000 + endRead.tv_usec) - (startRead.tv_sec*1000000 + startRead.tv_usec));
			free(toWrite);
			int rem_status = remove("./test.bin");
		}
	}
	FILE *opF = fopen("./timeDistWrite.csv","w");
	FILE *opFR = fopen("./timeDistRead.csv","w");
	for(int i=0;i<4;i++){
		float sum=0;
		float sumR = 0;
		for(int j=0;j<N-1;j++){
			fprintf(opF, "%ld,",timeTaken[i][j]);
			fprintf(opFR,"%ld,",timeTakenRead[i][j]);
			sum+=timeTaken[i][j];
			sumR+=timeTakenRead[i][j];
		}
		fprintf(opF,"%ld\n",timeTaken[i][N-1]);
		fprintf(opFR,"%ld\n",timeTakenRead[i][N-1]);
		sum+= timeTaken[i][N-1];
		sumR+= timeTakenRead[i][N-1];
		float mean = sum/N;
		float meanR = sumR/N;
		mean = mean/1000000;
		meanR = meanR/1000000;
		float meanWrSp = MB_TO_WRITE/mean;
		float meanRdSp = MB_TO_WRITE/meanR;
		printf("++++++++\n***WRITING***\nMean time for writing 0x%x = %f sec \nmean write speed = %f MBps\n",valsToWrite[i],mean,meanWrSp);
		printf("***READING***\nMean time for reading 0x%x = %f sec \nmean read speed = %f MBps\n",valsToWrite[i],meanR,meanRdSp);
	}
	fclose(opF);
	fclose(opFR);
	return 0;
}
