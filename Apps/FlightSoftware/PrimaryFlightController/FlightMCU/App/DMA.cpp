/** @file sys_main.c 
*   @brief Application main file
*   @date 17.Nov.2014
*   @version 04.02.00
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2014 Texas Instruments Incorporated - http://www.ti.com/ 
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
/*
 * HALCoGen Setup
 * 		File -> New -> Project
 * 		Family -> Hercules
 * 		Device -> <TMS570LS12/RM46x or Any>
 *		Ok
 *
 *		Driver Enable
 * 		- Enable GIO(optional), SCI2,  drivers
 *
 * 		Vim Channel 0-31
 * 		- Enable VIM channel 13 (LIN1 High) Interrupt (Optional)
 *
 * 		Vim Channel 32-63
 * 		- Enable VIM channel 40 (DMA BTCA) Interrupt
 *
 *		Configure SCI2
 *		- Baud rate 115.2 Kbps, 8 Data bits, 1 Stop bit, No Parity
 *
 * Configure Terminal
 * 		- Baud rate 115.2 Kbps, 8 Data bits, 1 Stop bit, No Parity, No Flow Control
 *
 * CCS Setup (http://www.ti.com/lit/an/spna121a/spna121a.pdf)
 * 		File -> New -> Project -> C/C++ -> CCS Project
 *
 *		Select the target <TMS570LS12/RM46x or Any>
 *
 *		Uncheck default file location and point to the HALCoGen project path
 *
 *		Use the same project name as the HALCoGen project.
 *
 *		Select empty project without main.c
 *
 *		Finish
 *
 *		Open the project properties
 *
 *		CCS Build -> ARM Compiler -> include option
 *
 *		Add a new #include search path "../include"
 *
 * DMA Complete Notification Setup
 *
 * 	Open notification.c
 *		Add extern Update_DMA_Comp_Flag(); to USER CODE BEGIN (0)
 *		Add Update_DMA_Comp_Flag(); to dmaGroupANotification()
 *
 *		Build and program the project into flash
 */
/* USER CODE END */

/* Include Files */

#include "sys_common.h"

/* USER CODE BEGIN (1) */

#include "system.h"
#include "sci.h"
#include "lin.h"
#include "gio.h"
#include "spi.h"
#include "sys_pmu.h"
#include "sys_dma.h"
#include "sys_vim.h"
#include <stdio.h>
#include <string.h>

/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */

const char Version[]      = "00.00.01.07";
const char Compile_Date[] = __DATE__;
const char Compile_Time[] = __TIME__;
const float32 System_Clock  = GCLK_FREQ;

/* dma control packet configuration stack */
g_dmaCTRL dma_config;

/* #define Enable_CIO      // Uncomment to enable Console CIO */
/* #define Enable_File_CIO // Uncomment to enable File CIO    */
#define Enable_SCI_Out

void main(void);
void sci_printf(char * output_string, ...);
void scidmaInit(sciBASE_t *sci);
void scidmaSend(char *source_address);
void Update_DMA_Comp_Flag();
void linsci2enableMBUFF();
void linsci2disableMBUFF();
void number_string(char *string, uint32 count);

/* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT;
uint32 DMA_Comp_Flag;

char buffer[4 * 500];
uint32 pmuCalibration;

/* USER CODE END */

void main(void)
{
/* USER CODE BEGIN (3) */
	uint32 IDLECOUNT = 0;

	sciInit();

	/* Print header on SCI2 */
	sci_printf("\033[2J"); // Clear terminal & return home commands
	sci_printf(" Hercule SCI DMA Example - Version %s\n\r", Version);
	sci_printf("*******************************************************************************\n\r\n\r");

	/* If pressed wait for LaunchXL2 User Switch A to be release before proceeding */
	while (gioGetBit(spiPORT3, 0U) == 0U);

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

/* USER CODE BEGIN (4) */


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


/** @fn void scidmaInit(short mode)
*   @brief Initialize the SCI and DMA to tranfer SCI data via DMA
*   @note This function configures the SCI to trigger a DMA request when the SCI TX is complete.
*/
void scidmaInit(sciBASE_t *sci)
{
	/* Enable DMA */
	dmaEnable();

	if (sci == scilinREG) {          			/* SCI2 is the default serial comport on LAUNCHXL2 launch pads*/

		/* Enable Interrupt after reception of data */
		dmaEnableInterrupt(DMA_CH0, BTC);		/* DMA_CH0 is highest priority */

		/* assigning dma request: channel-0 with request line - 1  - TX*/
		/* DMA Request 29 is for LIN ( SCI2) Transmit */
		/* Refer Data sheet - Default DMA Request Map section */
		dmaReqAssign(DMA_CH0,29);

	} else if (sci == sciREG) { 				/* SCI1 */
		return;					   				/* SCI1 is not supported at this time */

		/* Enable Interrupt after reception of data */
		/*dmaEnableInterrupt(DMA_CH1, BTC);		// DMA_CH0 is highest priority */

		/* assigning dma request: channel-0 with request line - 1  - TX*/
		/* DMA Request 31 is for SCI1 Transmit */
		/* Refer Data sheet - Default DMA Request Map section */
		/*dmaReqAssign(DMA_CH1,31); */

	} else {
		return;					 				/* Unknown register */
	}

    /* - Populate dma control packets structure */
	g_dmaCTRLPKT.CHCTRL    = 0;                 /* channel control            */
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

	/* Channel 40 - Enable the VIM channel in HalCoGen to include dmaBTCAInterrupt function */
	vimChannelMap(40, 40, &dmaBTCAInterrupt);

	/* Enable VIM DMA BTCA interrupt to CPU on SCI2 transfer complete */
	vimEnableInterrupt(40, SYS_IRQ);

} /* scidmaInit */


/** @fn void scidmaSend(char *source_address, short mode)
*   @brief Initialize the SCI and DMA to tranfer SCI data via DMA
*   @note This function configures the SCI to trigger a DMA request when the SCI TX is complete.
*
*   This function configures the DMA in single buffer or multibuffer mode.
*   In single buffer mode (0) the DMA moves each Byte to the SCI tranmit register when request is set.
*   In multi buffer mode (1)  the DMA moves 4 Bytes to the SCI transmit buffer when the request is set.
*/
void scidmaSend(char *source_address)
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

    if (((scilinREG->GCR1 >> 10U) & 1U) == 0U) {	 						/* SCI2 Multibuffer mode      */
    	g_dmaCTRLPKT.DADD   = (uint32)(&(scilinREG->TD))+dest_addr_offset;	/* In big endianism devices, the destination address needs to be adjusted */
        g_dmaCTRLPKT.RDSIZE = ACCESS_8_BIT;     							/* read size                  */
    	g_dmaCTRLPKT.WRSIZE = ACCESS_8_BIT;     							/* write size                 */
        g_dmaCTRLPKT.FRCNT  = strlen(source_address);    					/* frame count                */

    } else {
    	g_dmaCTRLPKT.DADD      = (uint32)(&(linREG->TDx));	 				/* In big endianism devices, the destination address needs to be adjusted
    	                                                  	  	  	  	  	  	   	* for byte access. The DMA is a big endian master. But the SCI Transmit buffer
    	                                                  	  	  	  	  	     	    * is accessible at the least significant byte.  */
    	g_dmaCTRLPKT.RDSIZE    = ACCESS_32_BIT;   		 					/* read size                  */
    	g_dmaCTRLPKT.WRSIZE    = ACCESS_32_BIT;    		 					/* write size                 */
        g_dmaCTRLPKT.FRCNT     = strlen(source_address)/4+8;	    			/* frame count                */
    }

	/* - setting dma control packets for transmit */
    dmaSetCtrlPacket(DMA_CH0,g_dmaCTRLPKT);

    /* - setting the dma channel to trigger on h/w request */
    dmaSetChEnable(DMA_CH0, DMA_HW);

	/* Enable TX DMA */
    scilinREG->SETINT = (1 << 16);

} /* scidmaSend */


/** @fn void Update_DMA_Comp_Flag()
*   @brief Switch the DMA complete flag to done and disable the SCI2 TX DMA interrupt
*   @note This function needs to be called by dmaGroupANotification in notification.c
*/
void Update_DMA_Comp_Flag()
{
    /* Set the Flag to Done*/
	DMA_Comp_Flag = 0x55AAD09E;

	/* Disable TX DMA Interrupt */
	scilinREG->CLEARINT = (1 << 16);
}


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
