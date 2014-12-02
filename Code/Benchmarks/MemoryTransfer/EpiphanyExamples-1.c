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
  ssize_t result;
  int i;
  int j;
  int c1_row = 1;
  int c1_col = 2;
  int c2_row = 0;
  int c2_col = 0;
  int mc_row = atoi(argv[1]);
  int mc_col = atoi(argv[2]);
  unsigned int flag = 0x00000000;

  if (sizeof(unsigned int) != 4) {
    printf("sizeof(unsigned int) = %d (not 4)\n", sizeof(unsigned int));
    exit(-1);
  }

  block = (unsigned int *) malloc(_32M);

  for (i = 0; i < _8M; ++i) {
    *(block + i) = (unsigned int) i;
  }

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

  result = e_write(&membuf, -1, -1, 0x01000000, block, _16M);
  printf("%lld bytes written\n", (long long int) result);

  e_load("EpiphanyExamples-1_core.srec", &EpiphanyGpu, c1_row, c1_col, E_FALSE);
  clock_gettime(CLOCK_MONOTONIC, &t_start);
  result = e_write(&EpiphanyGpu, c1_row, c1_col, 0x7000, (void *) &flag, sizeof(flag));
  printf("%lld bytes written\n", (long long int) result);
  usleep(1000);

  e_start(&EpiphanyGpu, c1_row, c1_col);
  usleep(1000);

  flag = 0xDDDDDDDD;
  result = e_write(&EpiphanyGpu, c1_row, c1_col, 0x7000, (void *) &flag, sizeof(flag));
  printf("%lld bytes written\n", (long long int) result);
  fflush(stdout);

  while (flag == 0xDDDDDDDD) {
    usleep(1000);
    e_read(&EpiphanyGpu, c1_row, c1_col, 0x7000, &flag, sizeof(flag));
  }
  printf("Result is: 0x%X\n", flag);

  clock_gettime(CLOCK_MONOTONIC, &t_end);
  if (t_end.tv_nsec < t_start.tv_nsec)
    printf("Time: %ld.%09ld\n", t_end.tv_sec - t_start.tv_sec - 1, 1000000000L + t_end.tv_nsec - t_start.tv_nsec);
  else
    printf("Time: %ld.%09ld\n", t_end.tv_sec - t_start.tv_sec, t_end.tv_nsec - t_start.tv_nsec);

  EXEC(e_free(&membuf));
  e_close(&EpiphanyGpu);
  e_finalize();

  EXEC(e_init(NULL));
  EXEC(e_reset_system());
  EXEC(e_get_platform_info(&EpiphanyPlatform));
  EXEC(e_open(&EpiphanyGpu, 0, 0, EpiphanyPlatform.rows, EpiphanyPlatform.cols));
  EXEC(e_alloc(&membuf, 0x00000000, 0x02000000));
  result = e_write(&membuf, -1, -1, 0x01000000, block, _16M);
  printf("%lld bytes written\n", (long long int) result);

  flag = 0x00000000;
  e_load("MemoryTransfer-DMA_core.srec", &EpiphanyGpu, c2_row, c2_col, E_FALSE);
  clock_gettime(CLOCK_MONOTONIC, &t_start);
  result = e_write(&EpiphanyGpu, c2_row, c2_col, 0x7000, (void *) &flag, sizeof(flag));
  printf("%lld bytes written\n", (long long int) result);
  usleep(1000);

  e_start(&EpiphanyGpu, c2_row, c2_col);
  usleep(1000);

  flag = 0xDDDDDDDD;
  result = e_write(&EpiphanyGpu, c2_row, c2_col, 0x7000, (void *) &flag, sizeof(flag));
  printf("%lld bytes written\n", (long long int) result);
  fflush(stdout);
  usleep(1000);

  while (flag == 0xDDDDDDDD) {
    usleep(1000);
    e_read(&EpiphanyGpu, c2_row, c2_col, 0x7000, &flag, sizeof(flag));
  }
  printf("Result for DMA is: 0x%X\n", flag);

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
  e_close(&EpiphanyGpu);
  e_finalize();

  return 0;
}
