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

mainHost.o: KNN.h HashFunctions.h mainHost.c
	$(CC) $(OPTS) -c mainHost.c -I .

KNN.o:	KNN.h HashFunctions.h KNN.c
	$(CC) $(OPTS) -c KNN.c -I .

HashFunctions.o: HashFunctions.h ../Infrastructure/HashFunctions.c
	$(CC) $(OPTS) -c ../Infrastructure/HashFunctions.c -I .


clean:
	rm -f KNNBIN
	rm -f *.o
