#ifndef DMA_H_
#define DMA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sys_dma.h"
#include "sci.h"

void scidmaInit();
void sciDMASend(enum dmaCHANNEL channel, char *source_address, unsigned size, dmaACCESS_t access, volatile bool *busy);
void sciDMARecv(enum dmaCHANNEL channel, char *dest_address, unsigned size, dmaACCESS_t access, volatile bool *busy);
unsigned getDMAIntStatus(enum dmaInterrupt intType);
void sciEnableMBUFF(sciBASE_t *sci, uint8_t bitsPerChar, uint8_t charPerFrame);
void sciDisableMBUFF(sciBASE_t *sci, uint8_t bitsPerChar);

#ifdef __cplusplus
}    // extern "C"
#endif
#endif
