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
e_mem_t membuf;
FILE *records_file;

int main(int argc, char *argv[]) {

  unsigned int *distances;
  unsigned int *identifiers;
  unsigned int count;
  unsigned int query;
  unsigned int core;
  //unsigned int readbuf;
  unsigned int i;
  unsigned row;
  unsigned col;
  unsigned rows;
  unsigned cols;
  struct timespec t_start;
  struct timespec t_end;

  count = 0x100;
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

  //usleep(100000);

/*
  for (core = 0; core < SIXTEEN; ++core) {
    row = core / 4;
    col = core % 4;
    e_read(&EpiphanyGpu, row, col, LOCAL_ID_ADDR, &readbuf, sizeof(unsigned int));
    printf("Successfully wrote ID to core %u\n", readbuf);
  }
  fflush(stdout);
*/

  e_alloc(&membuf, ZERO, 0x02000000);

  if ((records_file = fopen("records.bin", "rb")) == NULL) {
    fprintf(stderr, "Could not open records file.\n");
    fflush(stderr);
    exit(-1);
  }

  EXEC(e_start_group(&EpiphanyGpu));

  clock_gettime(CLOCK_MONOTONIC, &t_start);

  for (query = 0x1; query < 0x100; ++query) {
    ProcessRecords(distances, identifiers, count, query, ZERO);
    for (i = 0; i < count; ++i) printf("%u\n", *(distances + i));
      //printf("Distance between query record (%u) and record %u is %u\n", query, *(identifiers + i), *(distances + i));
  }
  query = 0x100;
  ProcessRecords(distances, identifiers, count, query, ONE);
  for (i = 0; i < count; ++i) printf("%u\n", *(distances + i));
    //printf("Distance between query record (%u) and record %u is %u\n", query, *(identifiers + i), *(distances + i));

  clock_gettime(CLOCK_MONOTONIC, &t_end);
  if (t_end.tv_nsec < t_start.tv_nsec)
    fprintf(stderr, "Time: %ld.%09ld\n", t_end.tv_sec - t_start.tv_sec - 1, 1000000000L + t_end.tv_nsec - t_start.tv_nsec);
  else
    fprintf(stderr, "Time: %ld.%09ld\n", t_end.tv_sec - t_start.tv_sec, t_end.tv_nsec - t_start.tv_nsec);

  fclose(records_file);

  EXEC(e_free(&membuf));
  EXEC(e_close(&EpiphanyGpu));
  EXEC(e_finalize());

  free(distances);
  free(identifiers);

  return 0;
}
