#include <App/DMA.h>
#include "sys_common.h"

#include "sys_vim.h"
#include <stdio.h>
#include <string.h>

g_dmaCTRL g_dmaCTRLPKT;


/** @fn void scidmaInit()
*   @brief Initialize the SCI and DMA to tranfer SCI data via DMA
*   @note This function configures the SCI to trigger a DMA request when the SCI TX/RX is complete.
*/
void scidmaInit()
{
	dmaEnable();

    dmaEnableInterrupt(DMA_CH0, BTC);	// DMA_CH0 is highest priority
    dmaReqAssign(DMA_CH0, 28);          // SCI2 (LIN) RX
    
    dmaEnableInterrupt(DMA_CH1, BTC);
    dmaReqAssign(DMA_CH1, 29);          // SCI2 (LIN) TX

    dmaEnableInterrupt(DMA_CH2, BTC);
    dmaReqAssign(DMA_CH2, 30);          // SCI RX

    dmaEnableInterrupt(DMA_CH3, BTC);
    dmaReqAssign(DMA_CH3, 31);          // SCI TX
    
    // Define Default DMA control packet for each channel
	g_dmaCTRLPKT.CHCTRL    = 0;                 /* channel control chain      */
	g_dmaCTRLPKT.ELCNT     = 1;                 /* element count              */
	g_dmaCTRLPKT.ELDOFFSET = 0;                 /* element destination offset */
	g_dmaCTRLPKT.ELSOFFSET = 0;                 /* element source offset      */
	g_dmaCTRLPKT.FRDOFFSET = 0;                 /* frame destination offset   */
	g_dmaCTRLPKT.FRSOFFSET = 0;                 /* frame source offset        */
	g_dmaCTRLPKT.PORTASGN  = 4;                 /* port b                     */
	g_dmaCTRLPKT.TTYPE     = FRAME_TRANSFER ;   /* transfer type              */
    g_dmaCTRLPKT.AUTOINIT  = AUTOINIT_OFF;      /* autoinit                   */

	vimChannelMap(33, 33, &dmaFTCAInterrupt);
	vimChannelMap(40, 40, &dmaBTCAInterrupt);

	vimEnableInterrupt(33, SYS_IRQ);
	vimEnableInterrupt(40, SYS_IRQ);

} /* scidmaInit */


/** @fn void betterdmaSetChEnable(uint32 channel,uint32 type)
*   @brief Enable channel
*   @param[in] channel DMA channel
*   @param[in] type Type of triggering
*                    - DMA_HW: Enables the selected DMA channel for hardware triggering
*                    - DMA_SW: Enables the selected DMA channel for software triggering
*
*   This function enables the DMA channel for hardware or software triggering
*/
void betterdmaSetChEnable(uint32 channel,uint32 type)
{
    if(type == (uint32)DMA_HW)
    {
     dmaREG->HWCHENAS |= (uint32)1U << channel;
    }
    else if(type == (uint32)DMA_SW)
    {
     dmaREG->SWCHENAS |= (uint32)1U << channel;
    }
    else
    {
    /* Empty  */
    }
}


/** @fn void sciDMASend(enum dmaCHANNEL channel, char *source_address, unsigned size, dmaACCESS_t access, volatile bool *busy)
*   @brief Initialize the SCI and DMA to transfer SCI data via DMA
*   @note This function configures the SCI to trigger a DMA request when the SCI TX is complete. See ISR.hpp
*         for interrupt handlers.
*
*
*   In big endianism devices, the destination address needs to be adjusted for byte access. The DMA
*   is a big endian master. But the SCI Transmit buffer is accessible at the least significant byte.
*
*/
void sciDMASend(enum dmaCHANNEL channel, char *source_address, unsigned size, dmaACCESS_t access, volatile bool *busy)
{
    while(*busy);
    *busy = true;

    g_dmaCTRLPKT.RDSIZE = access;                                       /* read size                  */
    g_dmaCTRLPKT.WRSIZE = access;                                       /* write size                 */
    g_dmaCTRLPKT.FRCNT  = size;                                         /* frame count                */
    g_dmaCTRLPKT.ADDMODERD = ADDR_INC1;                                 /* address mode read (source) */
    g_dmaCTRLPKT.ADDMODEWR = ADDR_FIXED;                                /* address mode write (dest)  */
    g_dmaCTRLPKT.SADD   = (uint32)source_address;                       /* source address             */
    switch (channel) {
        case DMA_CH1:
            g_dmaCTRLPKT.DADD   = (uint32)(&(scilinREG->TD));           /* Destination address        */
            break;
        case DMA_CH3:
            g_dmaCTRLPKT.DADD   = (uint32)(&(sciREG->TD));              /* Destination address        */
            break;
        default:
            return;     // Invalid DMA channel from mapping selected. Not assigned to SCI TX
    }

    /* - setting dma control packets for transmit */
    dmaSetCtrlPacket(channel, g_dmaCTRLPKT);

    /* - setting the dma channel to trigger on h/w request */
    dmaSetChEnable(channel, DMA_HW);

    /* Enable TX DMA */
    switch (channel) {
        case DMA_CH1:
            scilinREG->SETINT |= (1 << 16);
            break;
        case DMA_CH3:
            sciREG->SETINT |= (1 << 16);
            break;
        default:
            return;     // Invalid DMA channel from mapping selected. Not assigned to SCI TX
    }

} /* scidmaSend */


/** @fn void sciDMARecv(enum dmaCHANNEL channel, char *dest_address, unsigned size, dmaACCESS_t access, volatile bool *busy)
*   @brief Initialize the SCI and DMA to receive SCI data via DMA
*   @note This function configures the SCI to trigger a DMA request when the SCI TX is complete. See ISR.hpp
*         for interrupt handlers.
*
*
*   In big endianism devices, the destination address needs to be adjusted for byte access. The DMA
*   is a big endian master. But the SCI Transmit buffer is accessible at the least significant byte.
*
*/
void sciDMARecv(enum dmaCHANNEL channel, char *dest_address, unsigned size, dmaACCESS_t access, volatile bool *busy)
{
    while(*busy);
    *busy = true;

    g_dmaCTRLPKT.RDSIZE = access;                                       /* read size                  */
    g_dmaCTRLPKT.WRSIZE = access;                                       /* write size                 */
    g_dmaCTRLPKT.FRCNT  = size;                                         /* frame count                */
    g_dmaCTRLPKT.ADDMODERD = ADDR_FIXED;                                /* address mode read (source) */
    g_dmaCTRLPKT.ADDMODEWR = ADDR_INC1;                                 /* address mode write (dest)  */
    g_dmaCTRLPKT.DADD   = (uint32)dest_address;                         /* destination address        */
    switch (channel) {
        case DMA_CH0:
            g_dmaCTRLPKT.SADD   = (uint32)(&(scilinREG->RD));           /* Source address             */
            break;
        case DMA_CH2:
            g_dmaCTRLPKT.SADD   = (uint32)(&(sciREG->RD));              /* Source address             */
            break;
        default:
            return;     // Invalid DMA channel from mapping selected. Not assigned to SCI RX
    }

    /* - setting dma control packets for transmit */
    dmaSetCtrlPacket(channel, g_dmaCTRLPKT);

    /* - setting the dma channel to trigger on h/w request */
    dmaSetChEnable(channel, DMA_HW);

    /* Enable RX DMA */
    switch (channel) {
        case DMA_CH0:
            scilinREG->SETINT |= (1 << 18) | (1 << 17) | (1 << 9);
            break;
        case DMA_CH2:
            sciREG->SETINT |= (1 << 18) | (1 << 17) | (1 << 9);
            break;
        default:
            return;     // Invalid DMA channel from mapping selected. Not assigned to SCI RX
    }

} /* scidmaRecv */


/** @fn void sciEnableMBUFF(sciBASE_t *sci, uint8_t bitsPerChar, uint8_t charPerFrame)
*   @brief Switch SCI to single byte transfers
*   @note bitsPerChar and charPerFrame are 3 bit wide fields. The DMA interconnect does
*         not support 64bit unaligned accesses which limits transfers to a max of 4 bytes.
*
*   Enabling SCI's multibuffer mode can allow for fewer interrupts to be generated when
*   performing large transfers.
*
*/
void sciEnableMBUFF(sciBASE_t *sci, uint8_t bitsPerChar, uint8_t charPerFrame) {
	/* Enable Multi- Buffer Mode */
	sci->GCR1 |= (uint32)((uint32)1U << 10U);	 /* Enable MBUFMODE */

	/* Set transmission length in Bytes */
	sci->FORMAT = (uint32)((uint32)bitsPerChar  << 0U) | /* character length (bits) -1 */
				  (uint32)((uint32)charPerFrame << 16U); /* frame length (bytes) -1    */
} /* scinableMBUFF */


/** @fn void sciDisableMBUFF(sciBASE_t *, uint8_t bitsPerChar)
*   @brief Switch SCI to single byte transfers
*   @note This function requires scienableMBUFF() to be called first. bitsPerChar is a 3 bit
*         wide field.
*/
void sciDisableMBUFF(sciBASE_t *sci, uint8_t bitsPerChar) {
	/* Enable Multi- Buffer Mode */
	sci->GCR1 &= ~(uint32)((uint32)1U << 10U); /* Disable MBUFMODE (SCI2 Only) */

	/* Set transmission length in Bytes */
	sci->FORMAT = (uint32)((uint32)bitsPerChar << 0U) | /* character length (bits) -1 */
				  (uint32)((uint32)0U << 16U);          /* frame length (bytes) -1    */

} /* sciDisableMBUFF */



/* #define Enable_CIO      // Uncomment to enable Console CIO */
/* #define Enable_File_CIO // Uncomment to enable File CIO    */
#define Enable_SCI_Out
/** @fn void sci_printf(void)
*   @brief printf over sci function
*   @note This will transmit a formated variable argument string
*
*   This requires sciInit() to be called before to setup SCI2.
*   Length of resulting string is not to exceed length of local buffer variable.
*
*   Use compile time option Enable_CIO for debugging over JTAG to console or file.
*/
void sci_printf (char * format, ...) {
    uint8 cSCI_printfbuffer[256] = {0};
    uint32 length;

#ifdef Enable_CIO
    printf(format);
#endif

#ifdef Enable_SCI_Out
    va_list args;
    va_start (args, format);
    length = vsprintf ((char *)cSCI_printfbuffer, format, args);
    va_end (args);
    cSCI_printfbuffer[length+1] = 0;
    cSCI_printfbuffer[length+2] = 0;
    sciSend(scilinREG, strlen((const char *)cSCI_printfbuffer) + 3, cSCI_printfbuffer);
#endif

#ifdef Enable_File_CIO
    fprintf(fid, format);
#endif
} // sci_printf


/* USER CODE END */
