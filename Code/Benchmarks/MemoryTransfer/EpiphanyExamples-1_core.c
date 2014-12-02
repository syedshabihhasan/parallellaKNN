/*JWHSSHVBSGPLHERE*/
/*
 * EpiphanyExamples-1_core.c
 *
 * Author: James W Hegeman
 *
 */

#include "EE1.h"
#include <e_lib.h>

int main(void) {

  unsigned int *bufp = (unsigned int *) 0x01000000;
  unsigned int *flag = (unsigned int *) 0x7000;
  unsigned int word;
  int i;

  while (*flag != 0xDDDDDDDD);

  //for (i = 0; i < 262144; ++i) {
  for (i = 0; i < 256; ++i) {
    e_read(&e_emem_config, &word, E_SELF, E_SELF, bufp + i, sizeof(unsigned int));
    if (word != (unsigned int) i) {
      *flag = 0xE0E0E0E0;
      return 0;
    }
  }
  *flag = 0x11111111;

  return 0;
}
