#include <App/DMA.h>
#include "sys_common.h"

#include "sys_vim.h"
#include "os_portmacro.h"
#include <stdio.h>
#include <string.h>

#pragma SWI_ALIAS(prvRaisePrivilege, 1);
extern BaseType_t prvRaisePrivilege( void );
#define portRESET_PRIVILEGE( xRunningPrivileged ) if( xRunningPrivileged == 0 ) portSWITCH_TO_USER_MODE()

g_dmaCTRL g_dmaCTRLPKT;


/** @fn void scidmaInit()
*   @brief Initialize the SCI and DMA to tranfer SCI data via DMA
*   @note This function configures the SCI to trigger a DMA request when the SCI TX/RX is complete. Does not explicitly
*   enable DMA. Call dmaEnable prior to this.
*/
void scidmaInit()
{
    // DMA and VIM are only accessible in privileged mode!
    // Make sure dmaEnable() has been called

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

	vimChannelMap(BTCA_INT, BTCA_INT, dmaBTCAInterrupt);
	// vimChannelMap(FTCA_INT, FTCA_INT, dmaFTCAInterrupt);

	vimEnableInterrupt(BTCA_INT, SYS_IRQ);
	vimEnableInterrupt(FTCA_INT, SYS_IRQ);

	// See Hercules TRM (SPNU514C) Table 16-2 for DMA Request Line Connections
    dmaEnableInterrupt(SCILIN_RX_DMA_CH, BTC);
    // dmaEnableInterrupt(SCILIN_RX_DMA_CH, FTC);
    dmaReqAssign(SCILIN_RX_DMA_CH, 28);
    dmaSetPriority(SCILIN_RX_DMA_CH, HIGHPRIORITY);

    dmaEnableInterrupt(SCILIN_TX_DMA_CH, BTC);
    // dmaEnableInterrupt(SCILIN_TX_DMA_CH, FTC);
    dmaReqAssign(SCILIN_TX_DMA_CH, 29);

    dmaEnableInterrupt(SCI_RX_DMA_CH, BTC);
    // dmaEnableInterrupt(SCI_RX_DMA_CH, FTC);
    dmaReqAssign(SCI_RX_DMA_CH, 30);
    dmaSetPriority(SCI_RX_DMA_CH, HIGHPRIORITY);

    dmaEnableInterrupt(SCI_TX_DMA_CH, BTC);
    // dmaEnableInterrupt(SCI_TX_DMA_CH, FTC);
    dmaReqAssign(SCI_TX_DMA_CH, 31);


} /* scidmaInit */


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
        case SCILIN_TX_DMA_CH:
            g_dmaCTRLPKT.DADD   = (uint32)(&(scilinREG->TD));           /* Destination address        */
            break;
        case SCI_TX_DMA_CH:
            g_dmaCTRLPKT.DADD   = (uint32)(&(sciREG->TD));              /* Destination address        */
            break;
        default:
            return;     // Invalid DMA channel from mapping selected. Not assigned to SCI TX
    }

    BaseType_t privilege = prvRaisePrivilege();
    /* - setting dma control packets for transmit *//* - setting dma control packets for transmit */
    dmaSetCtrlPacket(channel, g_dmaCTRLPKT);
    /* - setting the dma channel to trigger on h/w request */
    dmaSetChEnable(channel, DMA_HW);
    portRESET_PRIVILEGE(privilege);

    /* Enable TX DMA */
    switch (channel) {
        case SCILIN_TX_DMA_CH:
            scilinREG->SETINT = (1 << 16);
            break;
        case SCI_TX_DMA_CH:
            sciREG->SETINT = (1 << 16);
            break;
        default:
            return;     // Invalid DMA channel from mapping selected. Not assigned to SCI TX
    }

} /* scidmaSend */

/** @fn void sciDMASendCleanup(enum dmaCHANNEL channel)
*   @brief Cleanup the SCI and DMA after transfer is complete
*   @note This function disables SCI DMA interrupt and may perform other clean up.
*/
void sciDMASendCleanup(enum dmaCHANNEL channel) {
    switch (channel) {
        case SCILIN_TX_DMA_CH:
            scilinREG->CLEARINT = (1 << 16);
            break;
        case SCI_TX_DMA_CH:
            sciREG->CLEARINT = (1 << 16);
            break;
        default:
            return;     // Invalid DMA channel from mapping selected. Not assigned to SCI RX
    }
}

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
        case SCILIN_RX_DMA_CH:
            g_dmaCTRLPKT.SADD   = (uint32)(&(scilinREG->RD));           /* Source address             */
            break;
        case SCI_RX_DMA_CH:
            g_dmaCTRLPKT.SADD   = (uint32)(&(sciREG->RD));              /* Source address             */
            break;
        default:
            return;     // Invalid DMA  int status =channel from mapping selected. Not assigned to SCI RX
    }

    BaseType_t privilege = prvRaisePrivilege();
    /* - setting dma control packets for transmit */
    dmaSetCtrlPacket(channel, g_dmaCTRLPKT);
    /* - setting the dma channel to trigger on h/w request */
    dmaSetChEnable(channel, DMA_HW);
    portRESET_PRIVILEGE(privilege);

    /* Enable RX DMA */
    switch (channel) {
        case SCILIN_RX_DMA_CH:
            scilinREG->SETINT = (1 << 18) | (1 << 17);
            break;
        case SCI_RX_DMA_CH:
            sciREG->SETINT = (1 << 18) | (1 << 17);
            break;
        default:
            return;     // Invalid DMA channel from mapping selected. Not assigned to SCI RX
    }

} /* scidmaRecv */

/** @fn void sciDMARecvCleanup(enum dmaCHANNEL channel)
*   @brief Cleanup the SCI and DMA after transfer is complete
*   @note This function disables SCI DMA interrupt and may perform other clean up.
*/
void sciDMARecvCleanup(enum dmaCHANNEL channel) {
    switch (channel) {
        case SCILIN_RX_DMA_CH:
            scilinREG->CLEARINT = (1 << 18) | (1 << 17);
            break;
        case SCI_RX_DMA_CH:
            sciREG->CLEARINT = (1 << 18) | (1 << 17);
            break;
        default:
            return;     // Invalid DMA channel from mapping selected. Not assigned to SCI RX
    }
}

/** @fn int getDMAIntStatus( enum dmaInterrupt intType)
 *  @brief Returns whether an interrupt is pending for the provided DMA channel and interrupt type.
 *
 *  Returns the offset corresponding to the interrupt status register. 0 means no interrupts pending.
 *  First 15 bits corespond to channels that have interrupts pending.
 *
 *  Shift right with DMA_CHx AND 0x01U to get boolean whether channel x had an interrupt pending.
 *
 *  This method clears the DMA interrupt pending flag (FTC/LFS/HBC/BTC) flag for the specified intType.
 *
 */
unsigned getDMAIntStatus(enum dmaInterrupt intType) {
    unsigned offset;
    // BaseType_t privilege = prvRaisePrivilege();
    switch (intType) {
        case FTC:
            offset = dmaREG->FTCAOFFSET & 0x3fU;
            break;
        case LFS:
            offset =  dmaREG->LFSAOFFSET & 0x3fU;
            break;
        case HBC:
            offset = dmaREG->HBCAOFFSET & 0x3fU;
            break;
        case BTC:
            offset = dmaREG->BTCAOFFSET & 0x3fU;
            break;
    }
    // portRESET_PRIVILEGE(privilege);
    return offset;
}


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
