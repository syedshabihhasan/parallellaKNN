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

tar:	KNN.h KNN.c ../Infrastructure/HashFunctions.c ../Infrastructure/HashFunctions.h mainHost.c Makefile
	tar -cvf HostAloneSrc.tar KNN.h KNN.c ../Infrastructure/HashFunctions.c ../Infrastructure/HashFunctions.h mainHost.c Makefile

clean:
	rm -f KNNBIN
	rm -f *.o
