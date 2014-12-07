#!/bin/bash

set -e

ESDK=$EPIPHANY_HOME
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/fast.ldf
INFRA=Infrastructure

gcc -Wall -o ParallellaKNN.elf ParallellaKNN.c -O0 -I $EINCS -L $ELIBS -le-hal
e-gcc -Wall -o ${INFRA}/RecordXfer_c.elf ${INFRA}/RecordXfer_c.c -O0 -T $ELDF -le-lib
e-objcopy --srec-forceS3 --output-target srec ${INFRA}/RecordXfer_c.elf ${INFRA}/RecordXfer_c.srec
