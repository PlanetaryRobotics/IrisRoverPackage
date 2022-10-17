#ifndef DMA_H_
#define DMA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sys_dma.h"
#include "sci.h"


#define FTCA_INT 33
#define BTCA_INT 40
#define SCILIN_RX_DMA_CH   DMA_CH0   // DMA CH0 Highest Priority
#define SCILIN_TX_DMA_CH   DMA_CH1
#define SCI_RX_DMA_CH      DMA_CH2
#define SCI_TX_DMA_CH      DMA_CH3

void scidmaInit();
void sciDMASend(enum dmaCHANNEL channel, char *source_address, unsigned size, dmaACCESS_t access, volatile bool *busy);
void sciDMASendCleanup(enum dmaCHANNEL channel);
void sciDMARecv(enum dmaCHANNEL channel, char *dest_address, unsigned size, dmaACCESS_t access, volatile bool *busy);
void sciDMARecvCleanup(enum dmaCHANNEL channel);
unsigned getDMAIntStatus(enum dmaInterrupt intType);
void sciEnableMBUFF(sciBASE_t *sci, uint8_t bitsPerChar, uint8_t charPerFrame);
void sciDisableMBUFF(sciBASE_t *sci, uint8_t bitsPerChar);

#ifdef __cplusplus
}    // extern "C"
#endif
#endif
