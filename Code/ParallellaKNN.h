/*JWHSSHVBSGPLHERE*/
/*
 * ParallellaKNN.h
 *
 * Authors: James W Hegeman
 *          Syed Shabih Hasan
 *          Vivek B Sardeshmukh
 *
 */

/* This is the host-side include file */

#ifndef __PARALLELLA_KNN_H__
#define __PARALLELLA_KNN_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <e-hal.h>

#define	NUM_BANKS			4
#define	NUM_DMA_CHANNELS		2
#define	BANK_SIZE			0x2000
#define	WORDS_PER_RECORD		0x0100
#define	LOCAL_BANK_0_ADDR		0x0000
#define	LOCAL_BANK_1_ADDR		0x2000
#define	LOCAL_BANK_2_ADDR		0x4000
#define	LOCAL_BANK_3_ADDR		0x6000
#define	LOCAL_QUERY_RECORD_ADDR		0x6000
#define	LOCAL_DISTANCE_ARRAY_ADDR	0x6400
#define	LOCAL_START_FLAG_ADDR		0x6440
#define	LOCAL_ID_ADDR			0x6444
#define	HEAP_BUFFER_ADDR		0x01000000
#define	QUERY_RECORD_ADDR		0x01FFF000
#define	DISTANCE_ARRAYS_BASE		0x01FFF400
#define	COUNTS_BASE			0x01FFF800
#define	DONE_FLAGS_BASE			0x01FFF840
#define	ZERO				0x00000000
#define	ONE				0x00000001
#define	EIGHT				0x00000008
#define	FIFTEEN				0x0000000F
#define	SIXTEEN				0x00000010
#define	TWOFIFTYSIX			0x00000100
#define	ONES				0x7FFFFFFF
#define	H0				0x0000
#define	DW				0x0008
#define	EXEC(f)				if (f != E_OK) printf("Uh oh.\n")

/*
 * ----------------------------- On the ARM SoC ------------------------------
 * DRAM heap region layout:
 *
 * This is the higher 16 MB of shared dram (0x3e000000-0x3fffffff and 0x8e000000-0x8fffffff).
 * The higher 16 MB are at 0x3f000000-0x3fffffff and 0x8f000000-0x8fffffff.
 * Do not use the lower 16 MB. Some Code/Stack stuff gets put there.
 *
 * All addresses listed below are offsets in shared dram:
 * (i.e. since we are using the higher 16 MB, they are in 0x01000000-0x01ffffff)
 *
 * We have 16MB of (heap) space to do with what we please.
 *
 * Shared Heap Memory:  (Have 16 MB of usable space)
 * Records to be processed (after doing hash lookups)   : 0x01000000-0x01ffefff
 * This is space for (16 MB - 4 KB) / 1 KB = (16 K - 4) = 16380 records.
 * Need space for 16 * 16 = 256 records in shared heap memory at a time.
 * This is only 256 KB.
 *
 * Metadata: (Need 4 KB of space)
 * Query Record    : 1024 B                     = 0x400 : 0x01fff000-0x01fff3ff
 * Distance arrays : 16 cores * 16 * 4 = 1024 B = 0x400 : 0x01fff400-0x01fff7ff
 * Counts          : 16 cores * 4      = 64 B   = 0x040 : 0x01fff800-0x01fff83f
 * Done flags      : 16 cores * 4      = 64 B   = 0x040 : 0x01fff840-0x01fff87f
 * Free space      :                                    : 0x01fff880-0x01ffffff
 *
 *
 * -------------------------- In each Epiphany Core --------------------------
 * SRAM layout:
 *
 * 0x0000-0x1fff : 8 KB : Bank 0, used for IVT, Code. Do not use.       :
 * 0x2000-0x3fff : 8 KB : Bank 1, used for input data on DMA channel 0. : (Input, Pulled by core via DMA)
 * 0x4000-0x5fff : 8 KB : Bank 2, used for input data on DMA channel 1. : (Input, Pulled by core via DMA)
 * 0x6000-0x7fff : 8 KB : Bank 3, used for metadata and the Stack.      :
 * 0x6000-0x63ff : 1 KB : The query record. 1024 Bytes.                 : (Input, Pulled by core via DMA)
 * 0x6400-0x643f : 64 B : The distance array (as 16 unsigned ints).     : (Output, Pushed by core via DMA)
 * 0x6440-0x6443 :      : Start flag (unsigned int).                    : (Input, Written by ARM)
 * 0x6444-0x6447 :      : ID (unsigned int).                            : (Input, Written by ARM)
 *
 */

extern e_platform_t EpiphanyPlatform;
extern e_epiphany_t EpiphanyGpu;
extern e_mem_t membuf;
extern FILE *records_file;

#endif
