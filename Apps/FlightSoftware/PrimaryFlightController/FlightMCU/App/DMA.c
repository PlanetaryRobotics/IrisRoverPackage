#include <App/DMA.h>
#include "sys_common.h"

#include "lin.h"
#include "sci.h"
#include "sys_vim.h"
#include <stdio.h>
#include <string.h>

/* #define Enable_CIO      // Uncomment to enable Console CIO */
/* #define Enable_File_CIO // Uncomment to enable File CIO    */
#define Enable_SCI_Out


char buffer[4 * 500];
uint32 pmuCalibration;

/* USER CODE END */
#if 0
static void main(void)
{
/* USER CODE BEGIN (3) */
	uint32 IDLECOUNT = 0;

	sciInit();

	/* Print header on SCI2 */
	sci_printf("\033[2J"); // Clear terminal & return home commands
	sci_printf(" Hercule SCI DMA Example\n\r");
	sci_printf("*******************************************************************************\n\r\n\r");

	/* Setup null terminated string to transmit */
	number_string((char *) buffer, 500);

	/* Init SCI2 for DMA transfers */
	scidmaInit(scilinREG);			/* Requires sciInit to be called first */

	/* Enable CPU Interrupts */
	_enable_IRQ();

	sci_printf("scidmaSend Example - DMA to transfer single Bytes from RAM to the SCI\n\r");

    scidmaSend(buffer);

    /* Enter Code Here but do not call scidmaSend again */

    /* Wait for the DMA interrupt ISR to set the Flag   */
    while(DMA_Comp_Flag != 0x55AAD09E){
    	IDLECOUNT++;
    }

    /* scidmaSend is complete and can be called again   */

    sci_printf("\n\r\n\r");
    sci_printf("scidmaSend Example Complete, %d characters sent\n\r", strlen(buffer));
    sci_printf("     CPU idle count: %d \n\r", IDLECOUNT);
    sci_printf("\n\r\n\r");


    sci_printf("scidmaSend Example - DMA to transfer single Bytes from RAM to  SCI\n\r");

    /* Enable SCI2 Multi-buffer (4 Byte) transfers */
    linsci2enableMBUFF();

    scidmaSend(buffer);

    /* Enter Code Here but do not call scidmaSend again */

    /* Wait for the DMA interrupt ISR to set the Flag   */
    while(DMA_Comp_Flag != 0x55AAD09E){
    	IDLECOUNT++;
    }

    /* scidmaSend is complete and can be called again   */

    /* Restore the SCI to single buffer mode */
    linsci2disableMBUFF();

	sci_printf("\n\r\n\r");
    sci_printf("scidmaSend Example Complete, %d characters sent\n\r", strlen(buffer));
    sci_printf("     CPU idle count: %d \n\r", IDLECOUNT);
    sci_printf("\n\r\n\r");

	sci_printf(" sci transmit time @ 115200bps 81NN\n\r\t\t\t ((%d Bytes) * 10bits per byte ) / 115200:\t %1.3f S\n\r", strlen(buffer), (float32)strlen(buffer)*10/115200);

	sci_printf("\n\r Demo Complete\n\r");
	sci_printf("*******************************************************************************\n\r\n\r");

	while (1);
/* USER CODE END */
}
#endif


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


/** @fn void scidmaInit()
*   @brief Initialize the SCI and DMA to tranfer SCI data via DMA
*   @note This function configures the SCI to trigger a DMA request when the SCI TX/RX is complete.
*/
void scidmaInit()
{
	/* Enable DMA */
	dmaEnable();

    dmaEnableInterrupt(DMA_CH0, BTC);	// DMA_CH0 is highest priority
    dmaReqAssign(DMA_CH0, 28);          // SCI2 (LIN) RX
    
    dmaEnableInterrupt(DMA_CH1, BTC);
    dmaReqAssign(DMA_CH1, 29);          // SCI2 (LIN) TX

    dmaEnableInterrupt(DMA_CH2, BTC);
    dmaReqAssign(DMA_CH2, 30);           // SCI RX

    dmaEnableInterrupt(DMA_CH3, BTC);
    dmaReqAssign(DMA_CH3, 31);           // SCI TX
    
    // Define Default DMA control packet for each channel
	g_dmaCTRLPKT.CHCTRL    = 0;                 /* channel control chain      */
	g_dmaCTRLPKT.ELCNT     = 1;                 /* element count              */
	g_dmaCTRLPKT.ELDOFFSET = 0;                 /* element destination offset */
	g_dmaCTRLPKT.ELSOFFSET = 0;                 /* element source offset      */
	g_dmaCTRLPKT.FRDOFFSET = 0;                 /* frame destination offset   */
	g_dmaCTRLPKT.FRSOFFSET = 0;                 /* frame source offset        */
	g_dmaCTRLPKT.PORTASGN  = 4;                 /* port b                     */
    g_dmaCTRLPKT.RDSIZE    = ACCESS_8_BIT;      /* read size                  */
	g_dmaCTRLPKT.WRSIZE    = ACCESS_8_BIT;      /* write size                 */
	g_dmaCTRLPKT.TTYPE     = FRAME_TRANSFER ;   /* transfer type              */
	g_dmaCTRLPKT.ADDMODERD = ADDR_INC1;         /* address mode read          */
	g_dmaCTRLPKT.ADDMODEWR = ADDR_FIXED;        /* address mode write         */
    g_dmaCTRLPKT.AUTOINIT  = AUTOINIT_OFF;      /* autoinit                   */

    /* Reset the Flag */
    DMA_Comp_Flag = 0x55AAD09E;

	vimChannelMap(33, 33, &dmaFTCAInterrupt);
	vimChannelMap(40, 40, &dmaBTCAInterrupt);

	vimEnableInterrupt(33, SYS_IRQ);
	vimEnableInterrupt(40, SYS_IRQ);

} /* scidmaInit */


/** @fn void scidmaSend(char *source_address, int size)
*   @brief Initialize the SCI and DMA to tranfer SCI data via DMA
*   @note This function configures the SCI to trigger a DMA request when the SCI TX is complete.
*
*   This function configures the DMA in single buffer or multibuffer mode.
*   In single buffer mode (0) the DMA moves each Byte to the SCI tranmit register when request is set.
*   In multi buffer mode (1)  the DMA moves 4 Bytes to the SCI transmit buffer when the request is set.
*   The size is in bytes since the DMA element transfer size is 1 byte
*/
void scidmaSend(enum dmaCHANNEL channel, char *source_address, unsigned size)
{
#if ((__little_endian__ == 1) || (__LITTLE_ENDIAN__ == 1))
	uint8 dest_addr_offset=0;		/* 0 for LE */
#else
	uint8 dest_addr_offset=3;		/* 3 for BE */
#endif

	/* Wait for the DMA to complete any existing transfers */
    while(DMA_Comp_Flag != 0x55AAD09E);

    /* Reset the Flag to not Done*/
    DMA_Comp_Flag = ~0x55AAD09E;

	/* - Populate dma control packets structure */
	g_dmaCTRLPKT.SADD       = (uint32)source_address;						/* source address             */

    // if (((scilinREG->GCR1 >> 10U) & 1U) == 0U) {	 						/* SCI2 Multibuffer mode      */
    g_dmaCTRLPKT.DADD   = (uint32)(&(scilinREG->TD))+dest_addr_offset;	/* In big endianism devices, the destination address needs to be adjusted */
    g_dmaCTRLPKT.RDSIZE = ACCESS_8_BIT;     							/* read size                  */
    g_dmaCTRLPKT.WRSIZE = ACCESS_8_BIT;     							/* write size                 */
    g_dmaCTRLPKT.FRCNT  = size;    					                    /* frame count                */

	/* - setting dma control packets for transmit */
    dmaSetCtrlPacket(channel ,g_dmaCTRLPKT);

    /* - setting the dma channel to trigger on h/w request */
    dmaSetChEnable(channel , DMA_HW);

	/* Enable TX DMA */
    scilinREG->SETINT = (1 << 16);

} /* scidmaSend */


/** @fn void linsci2enableMBUFF()
*   @brief Switch SCI2 to multi-buffer (4 Byte) transfers
*   @note This function requires scidmaInit(scilinREG) and _enable_IRQ() to be called first.
*
*   In multi buffer mode (1)  the DMA moves 4 Bytes to the SCI transmit buffer when the request is set.
*   The DMA interconnect does not support 64bit unaligned accesses which limits transfers to a max of 4 bytes.
*/
void linsci2enableMBUFF() {
	/* Enable Multi- Buffer Mode */
	scilinREG->GCR1 |= (uint32)((uint32)1U << 10U);	 /* Enable MBUFMODE (SCI2 Only) */

	/* Set transmission length in Bytes */
	scilinREG->FORMAT = (uint32)((uint32)7U << 0U)   /* character length (bits) -1 */
					  | (uint32)((uint32)3U << 16U); /* frame length (bytes) -1    */
} /* linsci2enableMBUFF */


/** @fn void linsci2disableMBUFF()
*   @brief Switch SCI2 to single byte transfers
*   @note This function requires linsci2enableMBUFF() to be called first.
*
*   In multi buffer mode (1)  the DMA moves 4 Bytes to the SCI transmit buffer when the request is set.
*   The DMA interconnect does not support 64bit unaligned accesses which limits transfers to a max of 4 bytes.
*/
void linsci2disableMBUFF() {
	/* Enable Multi- Buffer Mode */
	scilinREG->GCR1 &= ~(uint32)((uint32)1U << 10U); /* Disable MBUFMODE (SCI2 Only) */

	/* Set transmission length in Bytes */
	scilinREG->FORMAT = (uint32)((uint32)7U << 0U)   /* character length (bits) -1 */
					  | (uint32)((uint32)0U << 16U); /* frame length (bytes) -1    */

} /* linsci2enableMBUFF */


/** @fn void number_string(char *string, uint32 count)
*   @brief Generate null ternimated string of characters starting at zero to count.
*   @note This function pads the end with extra null characters and overrun warning
*/
void number_string(char *string, uint32 count) {
	uint32 i, temp, offset = 0;

	for (i = 0; i < count; i++){
		temp = sprintf (&buffer[i + offset],"%d ", i);
		if (temp != -1U) {
			offset += temp - 1; // increment buffer by the number of characters written minus the null character.
		}
	}

	/* For debug purposes add extra characters to identify overrun */
	sprintf (&buffer[i + offset],"\0\0\0\0\0\0\0\0 !!! Overrun !!! ");

} /* number_string */

/* USER CODE END */
