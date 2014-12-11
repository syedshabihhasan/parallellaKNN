# Makefile

############################################################
# Authors:  Shabih Hasan
# 	    James W Hegeman                                #
#           Vivek B Sardeshmukh                            #
#                                                          #
# Instructions:                                            #
#          make       -- Build hostalone project           #
#          make clean -- Delete object files               #
#                                                          #
############################################################

CC=gcc
LIBS=-lpthread -lm
OPTS=-Wall -g
CFLAGS=-I

KNNBIN:	KNN.o HashFunctions.o mainHost.o KNN.c mainHost.c ../Infrastructure/HashFunctions.c 
	$(CC) $(OPTS) -o KNNBIN mainHost.o HashFunctions.o KNN.o 	

mainHost.o: KNN.h ../Infrastructure/HashFunctions.h mainHost.c
	$(CC) $(OPTS) -c mainHost.c -I . -I ../Infrastructure  

KNN.o:	KNN.h ../Infrastructure/HashFunctions.h KNN.c
	$(CC) $(OPTS) -c KNN.c -I . -I ../Infrastructure

HashFunctions.o: ../Infrastructure/HashFunctions.h ../Infrastructure/HashFunctions.c
	$(CC) $(OPTS) -c ../Infrastructure/HashFunctions.c -I . -I ../Infrastructure

tar:	KNN.h KNN.c ../Infrastructure/HashFunctions.c ../Infrastructure/HashFunctions.h mainHost.c Makefile createEmptyMain.c
	tar -cvf HostAloneSrc.tar KNN.h KNN.c ../Infrastructure/HashFunctions.c ../Infrastructure/HashFunctions.h mainHost.c Makefile createEmptyMain.c

createEmptyMain.o:	KNN.h ../Infrastructure/HashFunctions.h  createEmptyMain.c
	$(CC) $(OPTS) -c createEmptyMain.c -I . -I ../Infrastructure/

Empty:	KNN.o HashFunctions.o createEmptyMain.o KNN.c createEmptyMain.c ../Infrastructure/HashFunctions.c 
	$(CC) $(OPTS) -o Empty -D DEBUG=1 createEmptyMain.o KNN.o HashFunctions.o

clean:
	rm -f KNNBIN
	rm -f Empty
	rm -f *.o
