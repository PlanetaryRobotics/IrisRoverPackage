#include "hal_stdtypes.h"

#include "reg_sci.h"
#include "sys_dma.h"

inline void rtiISR(uint32_t notification) {
    
}

extern uint32_t DMA_Comp_Flag;

inline void Update_DMA_Comp_Flag(dmaInterrupt_t inttype, uint32 channel) {
    /* Set the Flag to Done*/
    DMA_Comp_Flag = 0x55AAD09E;

    /* Disable TX DMA Interrupt */
    scilinREG->CLEARINT = (1 << 16);
}
