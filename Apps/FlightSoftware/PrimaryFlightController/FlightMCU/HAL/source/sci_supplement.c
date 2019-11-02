#include "sci.h"
#include "sys_vim.h"
#include "math.h"

int32 sciReceive(sciBASE_t *sci, uint32 length, uint8 * data, uint32_t timeoutMs)
{
    int32 bytesRead = 0;
    uint32_t tries = 0xffffffff;

    if ((sci->SETINT & (uint32)SCI_RX_INT) == (uint32)SCI_RX_INT)
    {
        /* we are in interrupt mode */
        uint32 index = (sci == sciREG) ? 0U : 1U;
        
        /* clear error flags */
        sci->FLR = ((uint32) SCI_FE_INT | (uint32) SCI_OE_INT | (uint32) SCI_PE_INT);

        g_sciTransfer_t[index].rx_length = length;
        g_sciTransfer_t[index].rx_data   = data;
	}
    else
    {   
        while (length > 0U)
        {
	        /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
            while ((sci->FLR & (uint32)SCI_RX_INT) == 0U && --tries > 0){ 
            } /* Wait */

            if(!tries)
                return 0; // timeout

		    *data = (uint8)(sci->RD & 0x000000FFU);
			data++;
			length--;
            bytesRead++;

            if(sciRxError(sci)){
                return -1; // error
            }
        }
    }

    return bytesRead;
}
