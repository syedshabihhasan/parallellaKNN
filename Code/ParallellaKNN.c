/*JWHSSHVBSGPLHERE*/
/*
 * ParallellaKNN.c
 *
 * Authors: James W Hegeman
 *          Syed Shabih Hasan
 *          Vivek B Sardeshmukh
 *
 */

#include "ParallellaKNN.h"
#include "Infrastructure/ProcessRecords.h"

e_platform_t EpiphanyPlatform;
e_epiphany_t EpiphanyGpu;

int main(int argc, char *argv[]) {

  unsigned int *distances;
  unsigned int *identifiers;
  unsigned int count;
  unsigned int query;
  unsigned int core;
  unsigned int readbuf;
  unsigned int i;
  unsigned row;
  unsigned col;
  unsigned rows;
  unsigned cols;

  count = 0xA;
  distances = (unsigned int *) malloc(count * 0x4);
  identifiers = (unsigned int *) malloc(count * 0x4);

  for (i = 0; i < count; ++i) {
    *(identifiers + i) = i + 0x1;
  }

  EXEC(e_init(NULL));
  EXEC(e_reset_system());
  EXEC(e_get_platform_info(&EpiphanyPlatform));

  row = 0;
  col = 0;
  rows = EpiphanyPlatform.rows;
  cols = EpiphanyPlatform.cols;

  EXEC(e_open(&EpiphanyGpu, row, col, rows, cols));
  EXEC(e_reset_group(&EpiphanyGpu));

  //e_set_host_verbosity(H_D2);

  EXEC(e_load_group("RecordXfer_c.srec", &EpiphanyGpu, row, col, rows, cols, E_FALSE));

  for (core = 0; core < SIXTEEN; ++core) {
    row = core / 4;
    col = core % 4;
    e_write(&EpiphanyGpu, row, col, LOCAL_ID_ADDR, &core, sizeof(unsigned int));
  }

  usleep(100000);

  for (core = 0; core < SIXTEEN; ++core) {
    row = core / 4;
    col = core % 4;
    e_read(&EpiphanyGpu, row, col, LOCAL_ID_ADDR, &readbuf, sizeof(unsigned int));
    printf("Successfully wrote ID to core %u\n", readbuf);
  }
  fflush(stdout);

  EXEC(e_start_group(&EpiphanyGpu));

  for (query = 0x1; query < 0xA; ++query) {
    ProcessRecords(distances, identifiers, count, query, ZERO);
  }
  query = 0xA;
  ProcessRecords(distances, identifiers, count, query, ONE);

  EXEC(e_close(&EpiphanyGpu));
  EXEC(e_finalize());

  for (i = 0; i < count; ++i) {
    printf("Distance between query record (%u) and record %u is %u.\n", query, *(identifiers + i), *(distances + i));
  }

  free(distances);
  free(identifiers);

  return 0;
}
