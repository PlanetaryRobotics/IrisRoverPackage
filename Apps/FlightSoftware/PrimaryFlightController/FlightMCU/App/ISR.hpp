#include "hal_stdtypes.h"

#include "reg_sci.h"
#include "sys_dma.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void dmaCh0_ISR(dmaInterrupt_t inttype);
extern void dmaCh1_ISR(dmaInterrupt_t inttype);
extern void dmaCh2_ISR(dmaInterrupt_t inttype);
extern void dmaCh3_ISR(dmaInterrupt_t inttype);


inline void rtiISR(uint32_t notification) {}

inline void Update_DMA_Comp_Flag(dmaInterrupt_t inttype, enum dmaCHANNEL channel) {
    switch (channel) {
        case DMA_CH0:
            scilinREG->CLEARINT = (1 << 18) | (1 << 17);        /* Disable RX DMA Interrupt */
            dmaCh0_ISR(inttype);
            break;
        case DMA_CH1:
            scilinREG->CLEARINT = (1 << 16);                    /* Disable TX DMA Interrupt */
            dmaCh1_ISR(inttype);
            break;
        case DMA_CH2:
            sciREG->CLEARINT = (1 << 18) | (1 << 17);           /* Disable RX DMA Interrupt */
            dmaCh2_ISR(inttype);
            break;
        case DMA_CH3:
            sciREG->CLEARINT = (1 << 16);                       /* Disable RX DMA Interrupt */
            dmaCh3_ISR(inttype);
            break;
        default:
    }
}

#ifdef __cplusplus
}    // extern "C"
#endif
