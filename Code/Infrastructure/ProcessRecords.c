/*JWHSSHVBSGPLHERE*/
/*
 * ProcessRecords.c
 *
 * Authors: James W Hegeman
 *          Shabih Hasan
 *          Vivek B Sardeshmukh
 *
 */

#include "ParallellaKNN.h"

/*
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
*/

void memcpy_w(void *dest, const void *src, size_t count);

void ProcessRecords(unsigned int *distances, unsigned int *identifiers, unsigned int count) {

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

  FILE *records_file;
  unsigned int *distp;
  unsigned int *done_flags;
  unsigned int *dflag;
  unsigned int *countp;
  unsigned int *id;
  void *heap_addr;
  off_t record_offset;
  unsigned int i;
  unsigned int start = ONE;
  unsigned int core;
  unsigned row;
  unsigned col;
  unsigned int counts[16];
  unsigned int divcount;
  unsigned int modcount;

  id = identifiers;

  if ((records_file = fopen(RECORDS_FILE_NAME, "rb")) == NULL) {
    fprintf(stderr, "Could not open records file.\n");
    fflush(stderr);
    exit(-1);
  }

  e_alloc(&membuf, ZERO, 0x02000000);

  done_flags = (unsigned int *) ((void *) membuf.base + DONE_FLAGS_BASE);
  countp = (unsigned int *) ((void *) membuf.base + COUNTS_BASE);
  heap_addr = (void *) membuf.base + HEAP_BUFFER_ADDR;

  for (core = ZERO; core < SIXTEEN; ++core) {
    dflag = done_flags + core;
    *dflag = ZERO;
  }

  divcount = count / SIXTEEN;
  core = count % SIXTEEN;
  for (i = ZERO; i < core; ++i) {
    counts[i] = divcount + ONE;
  }
  for (i = core; i < SIXTEEN; ++i) {
    counts[i] = divcount;
  }
  for (core = ZERO; core < SIXTEEN; ++core) {
    *countp++ = counts[core];
  }

  while (count > TWOFIFTYSIX) {
    for (core = ZERO; core < SIXTEEN; ++core) {
      for (i = ZERO; i < SIXTEEN; ++i) {
        if (i == ZERO) printf("Reading record %d, destined for core %d\n", i, core);
        record_offset = *id++ * 0x400;
        lseek(records_file, record_offset, SEEK_SET);
        fread(heap_addr, 0x400, ONE, records_file);
        heap_addr += 0x400;
      }

      printf("Starting core %d\n", core);
      row = core / 4;
      col = core % 4;
      e_write(&EpiphanyGpu, row, col, LOCAL_START_FLAG_ADDR, &start, sizeof(unsigned int));
    }

    distp = (unsigned int *) ((void *) membuf.base + DISTANCE_ARRAYS_BASE);
    for (core = ZERO; core < SIXTEEN; ++core) {
      dflag = done_flags + core;
      while (*dflag == ZERO);
      *dflag = ZERO;
      printf("Recording results from core %d\n", core);
      for (i = ZERO; i < SIXTEEN; ++i) {
        *distances++ = *distp++;
      }
    }

    count -= 256;
  }

  divcount = count / SIXTEEN;
  modcount = count % SIXTEEN;
  for (core = ZERO; core < modcount; ++core) {
    for (i = ZERO; i <= divcount; ++i) {
      record_offset = *id++ * 0x400;
      lseek(records_file, record_offset, SEEK_SET);
      fread(heap_addr, 0x400, ONE, records_file);
      heap_addr += 0x400;
    }
    heap_addr += (SIXTEEN - divcount - ONE) * 0x400;

    row = core / 4;
    col = core % 4;
    e_write(&EpiphanyGpu, row, col, LOCAL_START_FLAG_ADDR, &start, sizeof(unsigned int));
  }
  for (core = modcount; core < SIXTEEN; ++core) {
    for (i = ZERO; i < divcount; ++i) {
      record_offset = *id++ * 0x400;
      lseek(records_file, record_offset, SEEK_SET);
      fread(heap_addr, 0x400, ONE, records_file);
      heap_addr += 0x400;
    }
    heap_addr += (SIXTEEN - divcount) * 0x400;

    row = core / 4;
    col = core % 4;
    e_write(&EpiphanyGpu, row, col, LOCAL_START_FLAG_ADDR, &start, sizeof(unsigned int));
  }

  distp = (unsigned int *) ((void *) membuf.base + DISTANCE_ARRAYS_BASE);
  for (core = ZERO; core < SIXTEEN; ++core) {
    dflag = done_flags + core;
    while (*dflag == ZERO);
    *dflag = ZERO;
    printf("Recording results from core %d\n", core);
    if (core < modcount) {
      for (i = ZERO; i <= divcount; ++i) {
        *distances++ = *distp++;
      }
    } else {
      for (i = ZERO; i < divcount; ++i) {
        *distances++ = *distp++;
      }
  }

  for (core = ZERO; core < SIXTEEN; ++core) {
    row = core / 4;
    col = core % 4;
    e_write(&EpiphanyGpu, row, col, LOCAL_START_FLAG_ADDR, &start, sizeof(unsigned int));
  }

  usleep(100000);

  for (core = ZERO; core < SIXTEEN; ++core) {
    dflag = done_flags + core;
    printf("Core %u: 0x%X\n", core, *dflag);
  }

  return;
}

void memcpy_w(void *dest, const void *src, size_t count) {

  unsigned int *tmp_dest = (unsigned int *) dest;
  unsigned int *tmp_src = (unsigned int *) src;

  while (count > 0) {
    *tmp_dest++ = *tmp_src++;
    count -= sizeof(unsigned int);
  }

  return;
}
