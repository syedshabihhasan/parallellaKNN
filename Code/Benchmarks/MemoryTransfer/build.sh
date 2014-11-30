#!/bin/bash

set -e

ESDK=$EPIPHANY_HOME
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/fast.ldf

gcc -Wall -o EpiphanyExamples-1.elf EpiphanyExamples-1.c -O0 -I $EINCS -L $ELIBS -le-hal
e-gcc -Wall -o EpiphanyExamples-1_core.elf EpiphanyExamples-1_core.c -O0 -T $ELDF -le-lib
e-objcopy --srec-forceS3 --output-target srec EpiphanyExamples-1_core.elf EpiphanyExamples-1_core.srec
e-gcc -Wall -o MemoryTransfer-DMA_core.elf MemoryTransfer-DMA_core.c -O0 -T $ELDF -le-lib
e-objcopy --srec-forceS3 --output-target srec MemoryTransfer-DMA_core.elf MemoryTransfer-DMA_core.srec
