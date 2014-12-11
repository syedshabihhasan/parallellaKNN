/*JWHSSHVBSGPLHERE*/
/*
 * MemoryTransfer-DMA_core.c
 *
 * Author: James W Hegeman
 *
 */

#include "EE1.h"
#include <e_lib.h>

int main(void) {

  e_dma_desc_t dma_desc[2];
  void *buffer_base;
  void *buffer;
  unsigned int ID;
  unsigned int *done;
  unsigned int *bank[4];
  unsigned int error_code = 0xE0E0E0E0;
  unsigned int global_i;
  int i;
  int b;
  int j;
  int k;
  int ret;

  bank[0] = (unsigned int *) 0x0000;
  bank[1] = (unsigned int *) 0x2000;
  bank[2] = (unsigned int *) 0x4000;
  bank[3] = (unsigned int *) 0x6000;

  ID = *((unsigned int *) 0x6000);
  buffer_base = (void *) ((void *) e_emem_config.base + 0x01000000 + ID * 0x4000);
  done = (unsigned int *) ((void *) e_emem_config.base + 0x01F00000 + ID * 4);

  for (i = 0; i < 128; ++i) {
    for (b = 1; b < 3; ++b) {
      for (j = 0; j < 2048; ++j) {
        *(bank[b] + j) = (unsigned int) 0x00000000;
      }
    }
/*
    e_reg_write(E_REG_DMA0CONFIG, 0x00000000);
    e_reg_write(E_REG_DMA0STRIDE, 0x00000000);
    e_reg_write(E_REG_DMA0COUNT, 0x00000000);
    e_reg_write(E_REG_DMA0SRCADDR, 0x00000000);
    e_reg_write(E_REG_DMA0DSTADDR, 0x00000000);
    e_reg_write(E_REG_DMA0AUTODMA0, 0x00000000);
    e_reg_write(E_REG_DMA0AUTODMA1, 0x00000000);
    e_reg_write(E_REG_DMA0STATUS, 0x00000000);
*/
    buffer = buffer_base;
    global_i = ID * 0x1000;
    for (k = 0; k < 60; ++k) {
      e_dma_wait(E_DMA_0);
      e_dma_set_desc(E_DMA_0, (E_DMA_ENABLE | E_DMA_MASTER | E_DMA_DWORD), 0x0000, 0x0008, 0x0008, 0x0040, 0x0020, 0x0008, 0x0008, buffer, (void *) bank[1], &dma_desc[0]);
      /* The outer stride is not what you think it should be.
       * It's actually the distance jumped after the last transfer
       * of the inner loop. So, in effect, it is the 'true outer stride',
       * minus the inner width, plus 1 * the inner stride.
       */
      ret = e_dma_start(&dma_desc[0], E_DMA_0);
      if (ret != E_OK) {
        *done = error_code;
        return 0;
      }
      e_dma_wait(E_DMA_0);

      buffer += 0x00040000;

      for (b = 1; b < 3; ++b) {
        for (j = 0; j < 2048; ++j) {
          if (*(bank[b] + j) != global_i++) {
            *done = global_i;
            return 0;
          }
        }
      }

      global_i += 0x0000F000;
    }
  }

  *done = 0x600D0000 | ID;

  return 0;
}
