#ifndef DMA_H_
#define DMA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sys_dma.h"

void scidmaInit();
void scidmaSend(enum dmaCHANNEL channel, char *source_address, unsigned size);
void linsci2enableMBUFF();
void linsci2disableMBUFF();


g_dmaCTRL g_dmaCTRLPKT;

/* Guard for DMA access. Set when beginning transfer, cleared via interrupt */
uint32 DMA_Comp_Flag;

#ifdef __cplusplus
}    // extern "C"
#endif
#endif
