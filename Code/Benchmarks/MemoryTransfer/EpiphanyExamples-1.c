/*JWHSSHVBSGPLHERE*/
/*
 * EpiphanyExamples-1.c
 *
 * Author: James W Hegeman
 *
 */

#include "EE1.h"
#include <e-hal.h>

int main(int argc, char *argv[]) {

  e_platform_t EpiphanyPlatform;
  e_epiphany_t EpiphanyGpu;
  e_mem_t membuf;
  //struct timeval t_start;
  struct timespec t_start;
  //struct timeval t_end;
  struct timespec t_end;
  unsigned int *block;
  unsigned int *done_base;
  unsigned int *done;
  void *buffer;
  ssize_t result;
  unsigned int core;
  unsigned row;
  unsigned col;
  unsigned rows;
  unsigned cols;
  int i;
  //int j;

  if (sizeof(unsigned int) != 4) {
    printf("sizeof(unsigned int) = %d (not 4)\n", sizeof(unsigned int));
    exit(-1);
  }

  block = (unsigned int *) malloc(_16M);

  if (block == NULL) {
    printf("Unable to allocate memory block.\n");
    exit(-1);
  }

  for (i = 0; i < _4M; ++i) {
    *(block + i) = (unsigned int) i;
  }

/*
  EXEC(e_init(NULL));
  EXEC(e_reset_system());
  EXEC(e_get_platform_info(&EpiphanyPlatform));
  EXEC(e_open(&EpiphanyGpu, 0, 0, EpiphanyPlatform.rows, EpiphanyPlatform.cols));

  //e_set_host_verbosity(H_D2);

  printf("Platform Information:\n");
  printf("\ttype: 0x%X\n", EpiphanyPlatform.type);
  printf("\tversion: %s\n", EpiphanyPlatform.version);
  printf("\thal_ver: 0x%X\n", EpiphanyPlatform.hal_ver);
  printf("\tinitialized: %d\n", EpiphanyPlatform.initialized);
  printf("\tregs_base: 0x%X\n", EpiphanyPlatform.regs_base);
  printf("\tnum_chips: %d\n", EpiphanyPlatform.num_chips);
  printf("\trow: 0x%X\n", EpiphanyPlatform.row);
  printf("\tcol: 0x%X\n", EpiphanyPlatform.col);
  printf("\trows: 0x%X\n", EpiphanyPlatform.rows);
  printf("\tcols: 0x%X\n", EpiphanyPlatform.cols);
  printf("\tnum_emems: %d\n", EpiphanyPlatform.num_emems);

  printf("e_epiphany_t result:\n");
  printf("\tobjtype: 0x%X\n", EpiphanyGpu.objtype);
  printf("\ttype: 0x%X\n", EpiphanyGpu.type);
  printf("\tnum_cores: %u\n", EpiphanyGpu.num_cores);
  printf("\tbase_coreid: 0x%X\n", EpiphanyGpu.base_coreid);
  printf("\trow: %u\n", EpiphanyGpu.row);
  printf("\tcol: %u\n", EpiphanyGpu.col);
  printf("\trows: %u\n", EpiphanyGpu.rows);
  printf("\tcols: %u\n", EpiphanyGpu.cols);
  printf("\tmemfd: %d\n", EpiphanyGpu.memfd);
  fflush(stdout);

  EXEC(e_alloc(&membuf, 0x00000000, 0x02000000));
  printf("Memory Allocation:\n");
  printf("\tphy_base: 0x%X    (This is the phy_base of e_platform.emem[0])\n", (unsigned int) membuf.phy_base);
  printf("\tpage_base: 0x%X\n", (unsigned int) membuf.page_base);
  printf("\tpage_offset: 0x%X\n", (unsigned int) membuf.page_offset);
  printf("\tmap_size: 0x%X\n", membuf.map_size);
  printf("\tephy_base: 0x%X    (This is the ephy_base of e_platform.emem[0])\n", (unsigned int) membuf.ephy_base);
  printf("\temap_size: 0x%X\n", membuf.emap_size);
  printf("\tmapped_base: 0x%X\n", (unsigned int) membuf.mapped_base);
  printf("\tbase: 0x%X\n", (unsigned int) membuf.base);
  printf("\tmemfd: %d\n", membuf.memfd);
  fflush(stdout);

  for (i = 0; i < mc_row; ++i) {
    for (j = 0; j < mc_col; ++j) {
      printf("Core (%d, %d):\n", i, j);
      e_core_t *core = &EpiphanyGpu.core[i][j];
      e_mmap_t *sram = &EpiphanyGpu.core[i][j].mems;
      e_mmap_t *regmem = &EpiphanyGpu.core[i][j].regs;
      printf("\tid: 0x%X\t\trow: 0x%X\t\tcol: 0x%X\n", core->id, core->row, core->col);
      printf("\tSRAM:\n");
      printf("\t\tphy_base: 0x%X\n", (unsigned int) sram->phy_base);
      printf("\t\tpage_base: 0x%X\n", (unsigned int) sram->page_base);
      printf("\t\tpage_offset: 0x%X\n", (unsigned int) sram->page_offset);
      printf("\t\tmap_size: 0x%X\n", (unsigned int) sram->map_size);
      printf("\t\tmapped_base: 0x%X\n", (unsigned int) sram->mapped_base);
      printf("\t\tbase: 0x%X\n", (unsigned int) sram->base);
      printf("\tRegisters:\n");
      printf("\t\tphy_base: 0x%X\n", (unsigned int) regmem->phy_base);
      printf("\t\tpage_base: 0x%X\n", (unsigned int) regmem->page_base);
      printf("\t\tpage_offset: 0x%X\n", (unsigned int) regmem->page_offset);
      printf("\t\tmap_size: 0x%X\n", (unsigned int) regmem->map_size);
      printf("\t\tmapped_base: 0x%X\n", (unsigned int) regmem->mapped_base);
      printf("\t\tbase: 0x%X\n", (unsigned int) regmem->base);
    }
  }
*/
/**************************************************************************************/
  printf("-------- DMA TEST --------\n");
  EXEC(e_init(NULL));
  EXEC(e_reset_system());
  EXEC(e_get_platform_info(&EpiphanyPlatform));
  row = 0;
  col = 0;
  rows = EpiphanyPlatform.rows;
  cols = EpiphanyPlatform.cols;
  EXEC(e_open(&EpiphanyGpu, row, col, rows, cols));
  EXEC(e_reset_group(&EpiphanyGpu));
  EXEC(e_load("MemoryTransfer-DMA_core.srec", &EpiphanyGpu, row, col, E_FALSE));
  EXEC(e_alloc(&membuf, 0x00000000, 0x02000000));
  buffer = (void *) membuf.base + 0x01000000;
  result = e_write(&membuf, -1, -1, 0x01000000, block, _16M);
  printf("1st test setup: %lld bytes written\n", (long long int) result);
  done_base = (unsigned int *) ((void *) buffer + _15M);
  done = done_base;
  *done = 0x00000000;
  core = 0x00000000;
  result = e_write(&EpiphanyGpu, row, col, 0x6000, &core, sizeof(unsigned int));
  if (result != 4) printf("Uh oh.\n");
  clock_gettime(CLOCK_MONOTONIC, &t_start);
  EXEC(e_start(&EpiphanyGpu, row, col));
  while (*done == 0x00000000);
  printf("Result for single-core test is: 0x%X\n", *done);
  clock_gettime(CLOCK_MONOTONIC, &t_end);
  if (t_end.tv_nsec < t_start.tv_nsec)
    printf("Time: %ld.%09ld\n", t_end.tv_sec - t_start.tv_sec - 1, 1000000000L + t_end.tv_nsec - t_start.tv_nsec);
  else
    printf("Time: %ld.%09ld\n", t_end.tv_sec - t_start.tv_sec, t_end.tv_nsec - t_start.tv_nsec);
  EXEC(e_free(&membuf));
  EXEC(e_close(&EpiphanyGpu));
  EXEC(e_finalize());
/********************************************************************/
  EXEC(e_init(NULL));
  EXEC(e_reset_system());
  EXEC(e_get_platform_info(&EpiphanyPlatform));
  row = 0;
  col = 0;
  rows = EpiphanyPlatform.rows;
  cols = EpiphanyPlatform.cols;
  EXEC(e_open(&EpiphanyGpu, row, col, rows, cols));
  EXEC(e_reset_group(&EpiphanyGpu));
  EXEC(e_load_group("MemoryTransfer-DMA_core.srec", &EpiphanyGpu, row, col, rows, cols, E_FALSE));
  EXEC(e_alloc(&membuf, 0x00000000, 0x02000000));
  buffer = (void *) membuf.base + 0x01000000;
  result = e_write(&membuf, -1, -1, 0x01000000, block, _16M);
  printf("2nd test setup: %lld bytes written\n", (long long int) result);
  done_base = (unsigned int *) ((void *) buffer + _15M);
  done = done_base;
  for (i = 0; i < 16; ++i) {
    *done++ = 0x00000000;
  }
  done = done_base;
  for (core = 0x00000000; core < 0x00000010; ++core) {
    row = core / 4;
    col = core % 4;
    result = e_write(&EpiphanyGpu, row, col, 0x6000, &core, sizeof(unsigned int));
    usleep(100000);
    if (result != 4) printf("Uh oh.\n");
  }
  clock_gettime(CLOCK_MONOTONIC, &t_start);
  EXEC(e_start_group(&EpiphanyGpu));
  for (i = 0; i < 16; ++i) {
    while (*done == 0x00000000);
    printf("Result %d for multicore test is: 0x%X\n", i, *done++);
  }
  clock_gettime(CLOCK_MONOTONIC, &t_end);
  //gettimeofday(&t_start, NULL);
  //gettimeofday(&t_end, NULL);
  if (t_end.tv_nsec < t_start.tv_nsec)
    printf("Time for DMA: %ld.%09ld\n", t_end.tv_sec - t_start.tv_sec - 1, 1000000000L + t_end.tv_nsec - t_start.tv_nsec);
  else
    printf("Time for DMA: %ld.%09ld\n", t_end.tv_sec - t_start.tv_sec, t_end.tv_nsec - t_start.tv_nsec);
/*
  if (t_end.tv_usec < t_start.tv_usec)
    printf("Time: %ld.%06ld\n", t_end.tv_sec - t_start.tv_sec - 1, 1000000L + t_end.tv_usec - t_start.tv_usec);
  else
    printf("Time: %ld.%06ld\n", t_end.tv_sec - t_start.tv_sec, t_end.tv_usec - t_start.tv_usec);
*/
  EXEC(e_free(&membuf));
  EXEC(e_close(&EpiphanyGpu));
  EXEC(e_finalize());

  return 0;
}
