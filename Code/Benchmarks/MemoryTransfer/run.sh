#!/bin/bash

set -e

ESDK=$EPIPHANY_HOME
ELIBS=${ESDK}/tools/host/lib:$LD_LIBRARY_PATH
EHDF=$EPIPHANY_HDF
MAXROW=$1
MAXCOL=$2

if [ -z "$MAXROW" ]
then
  MAXROW=0
fi

if [ -z "$MAXCOL" ]
then
  MAXCOL=0
fi

sudo -E LD_LIBRARY_PATH=$ELIBS EPIPHANY_HDF=$EHDF ./EpiphanyExamples-1.elf $MAXROW $MAXCOL
