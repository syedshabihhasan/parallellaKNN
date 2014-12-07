#!/bin/bash

set -e

ESDK=$EPIPHANY_HOME
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/fast.ldf
INFRA=Infrastructure

gcc -Wall -c ${INFRA}/ProcessRecords.c -O0 -I $EINCS -L $ELIBS -le-hal
gcc -Wall -c ParallellaKNN.c -O0 -I $EINCS -L $ELIBS -le-hal
gcc -Wall -o ParallellaKNN.elf ParallellaKNN.o ProcessRecords.o -O0 -I $EINCS -L $ELIBS -le-hal
e-gcc -Wall -o RecordXfer_c.elf ${INFRA}/RecordXfer_c.c -O0 -T $ELDF -le-lib
e-objcopy --srec-forceS3 --output-target srec RecordXfer_c.elf RecordXfer_c.srec
