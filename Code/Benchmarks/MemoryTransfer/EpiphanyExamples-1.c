#include "EE1.h"
#include <e-hal.h>

int main(int argc, char *argv[]) {

  e_platform_t EpiphanyPlatform;
  e_epiphany_t EpiphanyGpu;
  e_mem_t membuf;
  //struct timeval t_start;
  //struct timeval t_end;
  unsigned int *block[2];
  ssize_t result;
  //unsigned int a;
  //unsigned int flag;
  int i;
  int j;
  int mc_row = atoi(argv[1]);
  int mc_col = atoi(argv[2]);

  if (sizeof(unsigned int) != 4) {
    printf("sizeof(unsigned int) = %d\n", sizeof(unsigned int));
    exit(-1);
  }

/*
  block[0] = (unsigned int *) malloc(4096 * sizeof(unsigned int));
  block[1] = block[0] + 2048;
*/
  block[0] = (unsigned int *) malloc(33554432);

  for (i = 0; i < (33554432 / sizeof(unsigned int)); ++i) {
    *(block[0] + i) = 0xAAAAAAAA;
  }

  EXEC(e_init(NULL));
  EXEC(e_reset_system());
  EXEC(e_get_platform_info(&EpiphanyPlatform));
  EXEC(e_open(&EpiphanyGpu, 0, 0, EpiphanyPlatform.rows, EpiphanyPlatform.cols));

  e_set_host_verbosity(H_D2);

  printf("Platform Information:\n");
  printf("\ttype: %X\n", EpiphanyPlatform.type);
  printf("\tversion: %s\n", EpiphanyPlatform.version);
  printf("\thal_ver: %X\n", EpiphanyPlatform.hal_ver);
  printf("\tinitialized: %d\n", EpiphanyPlatform.initialized);
  printf("\tregs_base: %X\n", EpiphanyPlatform.regs_base);
  printf("\tnum_chips: %d\n", EpiphanyPlatform.num_chips);
  printf("\trow: %X\n", EpiphanyPlatform.row);
  printf("\tcol: %X\n", EpiphanyPlatform.col);
  printf("\trows: %X\n", EpiphanyPlatform.rows);
  printf("\tcols: %X\n", EpiphanyPlatform.cols);
  printf("\tnum_emems: %d\n", EpiphanyPlatform.num_emems);

  //EXEC(e_alloc(&membuf, 0x00000000, 0xB6D67FFE));
  EXEC(e_alloc(&membuf, 0x01000000, 0x01000000));
  printf("Memory Allocation:\n");
  printf("\tphy_base: %X    (This is the phy_base of e_platform.emem[0])\n", (unsigned int) membuf.phy_base);
  printf("\tpage_base: %X\n", (unsigned int) membuf.page_base);
  printf("\tpage_offset: %X\n", (unsigned int) membuf.page_offset);
  printf("\tmap_size: %X\n", membuf.map_size);
  printf("\tephy_base: %X    (This is the ephy_base of e_platform.emem[0])\n", (unsigned int) membuf.ephy_base);
  printf("\temap_size: %X\n", membuf.emap_size);
  printf("\tmapped_base: %X\n", (unsigned int) membuf.mapped_base);
  printf("\tbase: %X\n", (unsigned int) membuf.base);
  printf("\tmemfd: %d\n", membuf.memfd);
  //EXEC(e_free(&membuf));

  printf("e_epiphany_t result:\n");
  printf("\tobjtype: %X\n", EpiphanyGpu.objtype);
  printf("\ttype: %X\n", EpiphanyGpu.type);
  printf("\tnum_cores: %u\n", EpiphanyGpu.num_cores);
  printf("\tbase_coreid: %X\n", EpiphanyGpu.base_coreid);
  printf("\trow: %u\n", EpiphanyGpu.row);
  printf("\tcol: %u\n", EpiphanyGpu.col);
  printf("\trows: %u\n", EpiphanyGpu.rows);
  printf("\tcols: %u\n", EpiphanyGpu.cols);
  printf("\tmemfd: %d\n", EpiphanyGpu.memfd);
  fflush(stdout);

  for (i = 0; i < mc_row; ++i) {
    for (j = 0; j < mc_col; ++j) {
      printf("Core (%d, %d):\n", i, j);
      e_core_t *core = &EpiphanyGpu.core[i][j];
      e_mmap_t *sram = &EpiphanyGpu.core[i][j].mems;
      e_mmap_t *regmem = &EpiphanyGpu.core[i][j].regs;
      printf("\tid: %X\t\trow: %X\t\tcol: %X\n", core->id, core->row, core->col);
      printf("\tSRAM:\n");
      printf("\t\tphy_base: %X\n", (unsigned int) sram->phy_base);
      printf("\t\tpage_base: %X\n", (unsigned int) sram->page_base);
      printf("\t\tpage_offset: %X\n", (unsigned int) sram->page_offset);
      printf("\t\tmap_size: %X\n", (unsigned int) sram->map_size);
      printf("\t\tmapped_base: %X\n", (unsigned int) sram->mapped_base);
      printf("\t\tbase: %X\n", (unsigned int) sram->base);
      printf("\tRegisters:\n");
      printf("\t\tphy_base: %X\n", (unsigned int) regmem->phy_base);
      printf("\t\tpage_base: %X\n", (unsigned int) regmem->page_base);
      printf("\t\tpage_offset: %X\n", (unsigned int) regmem->page_offset);
      printf("\t\tmap_size: %X\n", (unsigned int) regmem->map_size);
      printf("\t\tmapped_base: %X\n", (unsigned int) regmem->mapped_base);
      printf("\t\tbase: %X\n", (unsigned int) regmem->base);
    }
  }

  e_load("EpiphanyExamples-1_core.srec", &EpiphanyGpu, mc_row, mc_col, E_FALSE);
  //gettimeofday(&t_start, NULL);
  e_start(&EpiphanyGpu, mc_row, mc_col);

  //result = e_write(&membuf, 0, 0, 0, block[0], 33554432);
  if (*((char *) block[0]) == 0xAA) printf("Yep.\n");
  fflush(stdout);
  result = e_write(&membuf, 0, 0, 0, block[0], 1024);
  printf("\n");
  printf("%lld bytes written\n", (long long int) result);
  fflush(stdout);
  usleep(1000000);

/*
  flag = 0x1;
  e_write(&EpiphanyGpu, mc_row, mc_col, 0x7000, (void *) &flag, sizeof(flag));

  for (a = 0x0; a < A_UPPER_BOUND; a = a + 0x1) {
    for (i = 0; i < 4096; ++i) {
      *(block[0] + i) = a;
    }

    e_write(&EpiphanyGpu, mc_row, mc_col, 0x2000, (void *) block[0], 8192);
    e_write(&EpiphanyGpu, mc_row, mc_col, 0x4000, (void *) block[1], 8192);

    flag = 0x0;
    e_write(&EpiphanyGpu, mc_row, mc_col, 0x7000, (void *) &flag, sizeof(flag));
    while (flag == 0x0) {
      e_read(&EpiphanyGpu, mc_row, mc_col, 0x7000, &flag, sizeof(flag));
    }
    if (flag != 0x3) printf("Result is: %d\n", flag);
  }
*/
  EXEC(e_free(&membuf));
  e_close(&EpiphanyGpu);
  e_finalize();
  //gettimeofday(&t_end, NULL);
/*
  if (t_end.tv_usec < t_start.tv_usec)
    printf("Time: %ld.%06ld\n", t_end.tv_sec - t_start.tv_sec - 1, 1000000 + t_end.tv_usec - t_start.tv_usec);
  else
    printf("Time: %ld.%06ld\n", t_end.tv_sec - t_start.tv_sec, t_end.tv_usec - t_start.tv_usec);
*/
  return 0;
}
