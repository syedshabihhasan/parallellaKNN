/*JWHSSHVBSGPLHERE*/
/*
 * RecordXfer_c.c
 *
 * Authors: James W Hegeman
 *          Syed Shabih Hasan
 *          Vivek B Sardeshmukh
 *
 */

#include "ParallellaKNN_core.h"

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
#define	ONES				0x7FFFFFFF
#define	H0				0x0000
#define	DW				0x0008
#define	DMA0				E_DMA_0
#define	DMA1				E_DMA_1
#define	DMA_CONFIG			E_DMA_ENABLE | E_DMA_MASTER | E_DMA_DWORD;
#define	DMA_SET_0(X, Y, Z, W)		e_dma_set_desc(DMA0, DMA_CONFIG, H0, DW, DW, X, Y, DW, DW, Z, W, &dma_desc[0])
#define	DMA_SET_1(X, Y, Z, W)		e_dma_set_desc(DMA1, DMA_CONFIG, H0, DW, DW, X, Y, DW, DW, Z, W, &dma_desc[1])
#define	DMA_START_0			e_dma_start(&dma_desc[0], DMA0)
#define	DMA_START_1			e_dma_start(&dma_desc[1], DMA1)
#define	DMA_WAIT_0			e_dma_wait(DMA0)
#define	DMA_WAIT_1			e_dma_wait(DMA1)
*/

unsigned int hamming_dist(unsigned int *rec);

int main(void) {

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

  unsigned int *record;
  unsigned int *countp;
  unsigned int *distp;
  unsigned int *dummy;
  void *buffer;
  void *heap_addr;
  void *dist_addr;
  void *dflag;
  unsigned int i;
  unsigned int count;
  unsigned int ID;
  unsigned emem_base;
  e_dma_desc_t dma_desc[NUM_DMA_CHANNELS];

  /* Expect ARM to write this prior to starting the core */
  emem_base = e_emem_config.base;
  ID = *((unsigned int *) LOCAL_ID_ADDR);

  buffer = (void *) ((void *) emem_base + HEAP_BUFFER_ADDR);
  heap_addr = (void *) ((void *) emem_base + HEAP_BUFFER_ADDR + ID * 0x40000);
  dist_addr = (void *) ((void *) emem_base + DISTANCE_ARRAYS_BASE + ID * 0x40);
  countp = (unsigned int *) ((void *) emem_base + COUNTS_BASE + ID * UNSIGNED_INT_SIZE);
  dflag = (void *) ((void *) emem_base + DONE_FLAGS_BASE + ID * UNSIGNED_INT_SIZE);

  while (*((unsigned int *) LOCAL_START_FLAG_ADDR) == ZERO);
  *((unsigned int *) LOCAL_START_FLAG_ADDR) == ZERO;

  count = *countp;

  /* Get query record */
  DMA_SET_0(0x0020, 0x0004, (void *) QUERY_RECORD_ADDR, (void *) LOCAL_QUERY_RECORD_ADDR);
  DMA_START_0;
  DMA_WAIT_0;

  /* Process records 16 at a time */
  while (count > SIXTEEN) {
    DMA_SET_0(0x0020, 0x0020, heap_addr, (void *) LOCAL_BANK_1_ADDR);
    DMA_START_0;
    DMA_SET_1(0x0020, 0x0020, heap_addr + BANK_SIZE, (void *) LOCAL_BANK_2_ADDR);
    DMA_START_1;

    distp = (unsigned int *) LOCAL_DISTANCE_ARRAY_ADDR;
    record = (unsigned int *) LOCAL_BANK_1_ADDR;
    DMA_WAIT_0;
    for (i = ZERO; i < EIGHT; ++i) {
      *distp++ = hamming_dist(record);
      record += WORDS_PER_RECORD;
    }

    record = (unsigned int *) LOCAL_BANK_2_ADDR;
    DMA_WAIT_1;
    for (i = ZERO; i < EIGHT; ++i) {
      *distp++ = hamming_dist(record);
      record += WORDS_PER_RECORD;
    }

    DMA_SET_0(0x0008, 0x0001, (void *) LOCAL_DISTANCE_ARRAY_ADDR, dist_addr);
    DMA_START_0;
    DMA_WAIT_0;

    *dflag = ONE;
    count -= SIXTEEN;

    while (*((unsigned int *) LOCAL_START_FLAG_ADDR) == ZERO);
    *((unsigned int *) LOCAL_START_FLAG_ADDR) == ZERO;
  }

  if (count > EIGHT) {
    count -= EIGHT;
    DMA_SET_0(0x0020, 0x0020, heap_addr, (void *) LOCAL_BANK_1_ADDR);
    DMA_START_0;
    DMA_SET_1(0x0020, count * 0x0004, heap_addr + BANK_SIZE, (void *) LOCAL_BANK_2_ADDR);
    DMA_START_1;

    distp = (unsigned int *) LOCAL_DISTANCE_ARRAY_ADDR;
    record = (unsigned int *) LOCAL_BANK_1_ADDR;
    DMA_WAIT_0;
    for (i = ZERO; i < EIGHT; ++i) {
      *distp++ = hamming_dist(record);
      record += WORDS_PER_RECORD;
    }

    record = (unsigned int *) LOCAL_BANK_2_ADDR;
    DMA_WAIT_1;
    for (i = ZERO; i < count; ++i) {
      *distp++ = hamming_dist(record);
      record += WORDS_PER_RECORD;
    }

    DMA_SET_0(0x0008, 0x0001, (void *) LOCAL_DISTANCE_ARRAY_ADDR, dist_addr);
    DMA_START_0;
    DMA_WAIT_0;

    *dflag = ONE;

  } else if (count > ZERO) {
    DMA_SET_0(0x0020, count * 0x0004, heap_addr, (void *) LOCAL_BANK_1_ADDR);
    DMA_START_0;

    distp = (unsigned int *) LOCAL_DISTANCE_ARRAY_ADDR;
    record = (unsigned int *) LOCAL_BANK_1_ADDR;
    DMA_WAIT_0;
    for (i = ZERO; i < count; ++i) {
      *distp++ = hamming_dist(record);
      record += WORDS_PER_RECORD;
    }

    DMA_SET_0(0x0008, 0x0001, (void *) LOCAL_DISTANCE_ARRAY_ADDR, dist_addr);
    DMA_START_0;
    DMA_WAIT_0;

    *dflag = ONE;
  }

  while (*((unsigned int *) LOCAL_START_FLAG_ADDR) == ZERO);
  *((unsigned int *) LOCAL_START_FLAG_ADDR) == ZERO;

  *dflag = 0x600D600D;

  return 0;
}

unsigned int hamming_dist(unsigned int *rec) {

  unsigned int *query = (unsigned int *) LOCAL_QUERY_RECORD_ADDR;
  register unsigned int d;
  register unsigned int j;
  register unsigned int dist = ZERO;

  rec++;
  query++;
  for (j = ONE; j < WORDS_PER_RECORD; ++j) {
    d = *rec++ ^ *query++;
    while (d > ZERO) {
      dist += d & ONE;
      d >>= 1;
    }
  }

  return dist;
}
