#include "sci.h"
#include "sci_supplement.h"
#include "sys_vim.h"
#include "math.h"

extern volatile struct g_sciTransfer
{
    uint32   mode;         /* Used to check for TX interrupt Enable */
    uint32   tx_length;    /* Transmit data length in number of Bytes */
    uint32   rx_length;    /* Receive data length in number of Bytes */
    uint8    * tx_data;    /* Transmit data pointer */
    uint8    * rx_data;    /* Receive data pointer */
} g_sciTransfer_t[2U];

int32_t sciReceiveWithTimeout(sciBASE_t *sci, uint32 length, uint8 * data, uint32_t timeoutMs)
{
    int32_t bytesRead = 0;

    if ((sci->SETINT & (uint32)SCI_RX_INT) == (uint32)SCI_RX_INT)
    {
        /* we are in interrupt mode */
        uint32_t index = (sci == sciREG) ? 0U : 1U;
        
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
            while ((sci->FLR & (uint32)SCI_RX_INT) == 0U && --timeoutMs > 0){ 
            } /* Wait */

            if(!timeoutMs)
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
