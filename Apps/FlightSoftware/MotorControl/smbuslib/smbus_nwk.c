/* --COPYRIGHT--,BSD
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/

#include <stdint.h>
#include <msp430.h>
#include "smbus.h"
#include "smbus_nwk.h"
#include "smbus_phy.h"

//*****************************************************************************
//
//! \brief Local function that validates a Received packet
//
//! \param *smbus         Pointer to SMBus structure
//! \param check_pec    - 1:validate PEC,
//!                     - 0: don't validate PEC (restart)
//
//! \return  SMBUS_RET_ERROR if an error ocurred, SMBUS_RET_OK if OK
//
//*****************************************************************************
static int8_t validate_packet(SMBus *smbus,
                              uint8_t check_pec)
{
    // Process only if no problem was found with packet
    if(smbus->nwk.eState == SMBus_NwkState_Error)
    {
        return(SMBUS_RET_ERROR);
    }

#if (SMB_PEC_SUPPORTED == 1)
    if((smbus->ctrl.bits.pecEn == 1) && (check_pec))
    {
        // If PEC is enabled, it should be 0x00 after the last byte
        if(smbus->nwk.pec != 0x00)
        {
            smbus->status.bits.pecErr = 1;
            return(SMBUS_RET_ERROR);
        }
        else
        {
            // PEC OK
            smbus->nwk.rxIndex--;  // Remove the PEC byte from packet
        }
    }
#endif

    return(SMBUS_RET_OK);
}

uint8_t SMBus_NWK_getRxPayloadSize(SMBus *smbus)
{
    uint8_t data_size = smbus->nwk.rxIndex;

#if (SMB_PEC_SUPPORTED == 1)
    if(smbus->ctrl.bits.pecEn == 1)
    {
        data_size--;   // Remove 1 byte for PEC
    }
#endif
    return(data_size);
}

#if (SMB_CRC8_USE_LOOKUP_TABLE == 0)
void SMBus_NWK_addCrc8(uint8_t *crc,
                       uint8_t newData)
{
    uint8_t i;  // Counter for 8 shifts

    *crc ^= newData;        // Initial XOR

    i = 8;
    do
    {
        if(*crc & 0x80)
        {
            *crc <<= 1;
            *crc ^= CRC8_POLY;
        }
        else
        {
            *crc <<= 1;
        }
    }
    while(--i);
}

#else
void SMBus_NWK_addCrc8(uint8_t *crc,
                       uint8_t newData)
{
    // Constant lookup table used to calculate CRC
    static const uint8_t lookup_table[256] =
    {
        0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
        0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
        0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
        0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
        0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
        0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
        0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
        0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
        0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
        0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
        0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
        0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
        0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
        0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
        0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
        0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
        0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
        0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
        0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
        0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
        0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
        0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
        0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
        0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
        0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
        0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
        0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
        0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
        0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
        0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
        0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
        0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3U
    };
    *crc = lookup_table[*crc ^ newData];
}

#endif

void SMBus_NWK_startTxTransfer(SMBus *smbus)
{
    uint8_t targetAddr = smbus->nwk.currentAddr;

    smbus->state = SMBus_State_OK;
    smbus->nwk.rxIndex = 0;
    smbus->nwk.txIndex = 0;
#ifdef SMB_PEC_SUPPORTED
    if(smbus->ctrl.bits.pecEn == 1)
    {
        smbus->nwk.pec = 0x00;
        SMBus_NWK_addCrc8(&smbus->nwk.pec, targetAddr << 1); // Add addr to PEC
    }
#endif

    if(smbus->nwk.eState == SMBus_NwkState_TXQuickCMD)
    {
        SMBus_PHY_masterStartTx(smbus, targetAddr, SMBus_Stop_Immediate);
    }
    else
    {
        SMBus_PHY_masterStartTx(smbus, targetAddr, SMBus_Stop_No);
    }
}

void SMBus_NWK_startRxTransfer(SMBus *smbus)
{
    smbus->nwk.rxIndex = 0;
    smbus->state = SMBus_State_OK;

#ifdef SMB_PEC_SUPPORTED
    if(smbus->ctrl.bits.pecEn == 1)
    {
        smbus->nwk.rxLen++;
        if(smbus->nwk.eState == SMBus_NwkState_RX)
        {
            smbus->nwk.pec = 0x00;    // Start PEC if this is a Read-only transaction
        }
        // Add addr+R to PEC
        SMBus_NWK_addCrc8(&smbus->nwk.pec,
                          (smbus->nwk.currentAddr << 1) + I2C_READ);
    }
#endif
    // Send Restart+R in order to read expected response
    if(smbus->nwk.rxLen == 0)
    {
        SMBus_PHY_masterStartRx(smbus, smbus->nwk.currentAddr,
                                SMBus_Stop_Immediate);
    }
    else if(smbus->nwk.rxLen == 1)
    {
        SMBus_PHY_masterStartRx(smbus, smbus->nwk.currentAddr, SMBus_Stop_Addr);
    }
    else if(smbus->nwk.rxLen == 2)
    {
        SMBus_PHY_masterStartRx(smbus, smbus->nwk.currentAddr,
                                SMBus_Stop_PreRead);
    }
    else
    {
        SMBus_PHY_masterStartRx(smbus, smbus->nwk.currentAddr, SMBus_Stop_No);
    }

    if(smbus->nwk.eState == SMBus_NwkState_TX_Block)
    {
        smbus->nwk.eState = SMBus_NwkState_RX_Block;
    }
    else
    {
        smbus->nwk.eState = SMBus_NwkState_RX;
    }
}

SMBus_State SMBus_NWK_slaveProcessStart(SMBus *smbus,
                                        uint8_t addrRw)
{
    SMBus_State ret_state = SMBus_State_OK;

    // Init counters and variables
    smbus->nwk.txIndex = 0;
    smbus->nwk.currentAddr = addrRw;

    if((smbus->state == SMBus_State_Slave_QCMD) ||
       (smbus->state == SMBus_State_Slave_CmdComplete))
    {
        // If the previous packet wasn't cleared by applicaiton, flag overrun
        smbus->status.bits.packOvrErr = 1;
    }

    if(addrRw & I2C_READ)
    {
        // Host is requesting to read data
        if(smbus->nwk.eState == SMBus_NwkState_RX)
        {
            smbus->nwk.txLen = 0;  // Nothing to respond by default
            // Previous Start was a Write. This is a restart condition.
            // Process packet and prepare response
            if(validate_packet(smbus, 0) == SMBUS_RET_OK)
            {
                ret_state = SMBus_State_Slave_CmdComplete; // Command is ready to process
                smbus->nwk.eState = SMBus_NwkState_TX_Resp;  // Responding
            }
            else
            {
                ret_state = SMBus_State_PECError;
                smbus->nwk.eState = SMBus_NwkState_Error; // Packet was invalid (PEC Error)
            }
        }
        else if(smbus->nwk.eState == SMBus_NwkState_Idle)
        {
            // If the previous state was not Write_Req, then we need to respond
            // to a receive byte or QUICK_COMMAND(R)
            // We can't recognize QUICK_COMMAND(R) because of the double-buffer
            // mechanism. By default, we will try to respond as a RECEIVE_BYTE
            // sending the TxPtr
            smbus->nwk.txLen = 1;
            smbus->nwk.eState = SMBus_NwkState_TX;
#ifdef SMB_PEC_SUPPORTED
            if(smbus->ctrl.bits.pecEn == 1)
            {
                smbus->nwk.pec = 0x00;
            }
#endif
        }
    }
    else
    {
        // Host is requesting to Write data
        if(smbus->nwk.eState == SMBus_NwkState_Idle)
        {
            // Default packet is QUICK_COMMAND(W) unless data is received
            smbus->nwk.rxIndex = 0;
            smbus->nwk.eState = SMBus_NwkState_RX;
#ifdef SMB_PEC_SUPPORTED
            if(smbus->ctrl.bits.pecEn == 1)
            {
                smbus->nwk.pec = 0x00;
            }
#endif
        }
    }

#ifdef SMB_PEC_SUPPORTED
    if(smbus->ctrl.bits.pecEn == 1)
    {
        SMBus_NWK_addCrc8(&smbus->nwk.pec, addrRw);    // Add addrRw to PEC
    }
#endif

    return(ret_state);
}

SMBus_State SMBus_NWK_slaveProcessRx(SMBus *smbus,
                                     uint8_t data)
{
    SMBus_State ret_state = SMBus_State_OK;

    // Check if the SMBus is expecting reception
    if(smbus->nwk.eState != SMBus_NwkState_RX)
    {
        smbus->status.bits.packErr = 1;
        smbus->nwk.eState = SMBus_NwkState_Error;
        return(SMBus_State_Slave_Error);
    }
    // Check packet size
    if((smbus->nwk.rxIndex >= SMB_MAX_PACKET_SIZE) ||
       (smbus->nwk.rxIndex >= smbus->nwk.rxSize))
    {
        smbus->status.bits.packErr = 1;
        smbus->nwk.eState = SMBus_NwkState_Error;
        return(SMBus_State_DataSizeError);
    }
    if((smbus->nwk.rxBuffPtr == NULL) || (smbus->nwk.rxSize == 0))
    {
        // Buffer hasn't been initialized
        smbus->nwk.eState = SMBus_NwkState_Error;
        return(SMBus_State_Slave_NotReady);
    }

    // If no errors, continue
    if((smbus->state == SMBus_State_Slave_FirstByte) ||
       (smbus->state == SMBus_State_Slave_ByteReceived))
    {
        // Previous byte wasn't processed, flag byte overrun
        smbus->status.bits.byteOvrErr = 1;
    }

    smbus->nwk.rxBuffPtr[smbus->nwk.rxIndex] = data;
    // Pass the packet to application network, note that the SMBus spec
    // requires to validate the cmd+data and ACK/NACK appropiately and
    // immediately depending on its validity.
    if(smbus->nwk.rxIndex == 0)
    {
        // First byte (command) was received. Application should use this
        // state in order to validate the command
        ret_state = SMBus_State_Slave_FirstByte;
        smbus->nwk.currentCmd = data;
    }
    else
    {
        // Data byte was received. Application can use this to process each
        // byte
        ret_state = SMBus_State_Slave_ByteReceived;
    }
#ifdef SMB_PEC_SUPPORTED
    if(smbus->ctrl.bits.pecEn == 1)
    {
        SMBus_NWK_addCrc8(&smbus->nwk.pec, data); // Add data to PEC
    }
#endif
    smbus->nwk.rxIndex++;

    return(ret_state);
}

SMBus_State SMBus_NWK_slaveProcessTx(SMBus *smbus,
                                     uint8_t *data)
{
    SMBus_State ret_state = SMBus_State_OK;
    *data = RESPONSE_NTR;   // Default data if nothing to report

    if((smbus->nwk.eState != SMBus_NwkState_TX_Resp) &&
       (smbus->nwk.eState != SMBus_NwkState_TX))
    {
        // Invalid state
        smbus->status.bits.packErr = 1;
        smbus->nwk.eState = SMBus_NwkState_Error;
        return(SMBus_State_Slave_Error);
    }

    if((smbus->nwk.txIndex > SMB_MAX_PACKET_SIZE) ||
       (smbus->nwk.txIndex > smbus->nwk.txSize))
    {
        smbus->status.bits.packErr = 1;
        smbus->nwk.eState = SMBus_NwkState_Error;
        return(SMBus_State_DataSizeError);
    }

    if((smbus->nwk.txBuffPtr == NULL) || (smbus->nwk.txSize == 0))
    {
        // Buffer hasn't been initialized
        smbus->nwk.eState = SMBus_NwkState_Error;
        return(SMBus_State_Slave_NotReady);
    }

    // If no errors, continue
    if(smbus->nwk.txIndex < smbus->nwk.txLen)
    {
        if(smbus->nwk.eState == SMBus_NwkState_TX)
        {
            // Send the TX Pointer if this is a RECEIVE_BYTE
            *data = *smbus->nwk.recByteTxPtr;
        }
        else
        {
            // Send buffer if this is a Response
            *data = smbus->nwk.txBuffPtr[smbus->nwk.txIndex];
        }
        smbus->nwk.txIndex++;
#ifdef SMB_PEC_SUPPORTED
        if(smbus->ctrl.bits.pecEn == 1)
        {
            SMBus_NWK_addCrc8(&smbus->nwk.pec, *data); // Add data to PEC
        }
#endif
    }
#ifdef SMB_PEC_SUPPORTED
    else if((smbus->ctrl.bits.pecEn == 1)  &&
            (smbus->nwk.txIndex == smbus->nwk.txLen))
    {
        // Send the calculated PEC now
        *data = smbus->nwk.pec;
        smbus->nwk.txIndex++;
    }
#endif

    return(ret_state);
}

SMBus_State SMBus_NWK_slaveProcessStop(SMBus *smbus)
{
    SMBus_State ret_state = SMBus_State_OK;

    // Quick command is detected when a TX is detected with no data
    // We can't detect Quick_Command(R) because of double-buffer mechanism
    if(smbus->nwk.eState == SMBus_NwkState_RX)
    {
        if(smbus->nwk.rxIndex == 0)
        {
            // Process quick command
            ret_state = SMBus_State_Slave_QCMD;
        }
        else if(validate_packet(smbus, 1) == SMBUS_RET_OK)
        {
            // The received packet is valid and ready to process by Application
            ret_state = SMBus_State_Slave_CmdComplete;
        }
        else
        {
            // PEC error, report
            ret_state = SMBus_State_PECError;
        }
    }
    else if((smbus->nwk.eState == SMBus_NwkState_Idle) &&
            (smbus->nwk.rxIndex == 0))
    {
        // case when Quick command is detected but Stop flag cleared Start
        // before it was set
        ret_state = SMBus_State_Slave_QCMD;
    }
    else
    {
        // If the state is SMBus_State_Respond or SMBus_State_ReadReq, then we
        // don't need to do anything during a stop
    }

    // Set the network state machine to idle in order to get new packet
    smbus->nwk.eState = SMBus_NwkState_Idle;
    return(ret_state);
}

SMBus_State SMBus_NWK_slaveProcessTimeout(SMBus *smbus)
{
    // Signal the error
    smbus->status.bits.toErr = 1;

    // Restart the SMBus Interface
    SMBus_PHY_disable(smbus);
    SMBus_PHY_slaveEnable(smbus);
    // After restart, be ready to accept new packet
    smbus->nwk.eState = SMBus_NwkState_Idle;

    return(SMBus_State_TimeOutError);
}

SMBus_State SMBus_NWK_masterProcessRx(SMBus *smbus,
                                      uint8_t data)
{
    uint8_t data_len = smbus->nwk.rxLen;

    if((smbus->nwk.eState != SMBus_NwkState_RX) &&
       (smbus->nwk.eState != SMBus_NwkState_RX_Block))
    {
        SMBus_PHY_masterSendStop(smbus);
        smbus->nwk.eState = SMBus_NwkState_Error;
        smbus->state = SMBus_State_Master_Error;
        return(smbus->state);
    }
    if((smbus->nwk.rxIndex >= smbus->nwk.rxLen) ||
       (smbus->nwk.rxIndex >= SMB_MAX_PACKET_SIZE))
    {
        SMBus_PHY_masterSendStop(smbus);
        smbus->status.bits.packErr = 1;
        smbus->nwk.eState = SMBus_NwkState_Error;
        smbus->state = SMBus_State_DataSizeError;
        return(smbus->state);
    }

    if((smbus->nwk.rxIndex == 0) &&
       (smbus->nwk.eState == SMBus_NwkState_RX_Block))
    {
        // Update the lenght based on received Byte count
        if((data == 0) || (data > 32))
        {
            SMBus_PHY_masterSendStop(smbus);
            smbus->status.bits.packErr = 1;
            smbus->nwk.eState = SMBus_NwkState_Error;
            smbus->state = SMBus_State_DataSizeError;
            return(smbus->state);
        }
        else
        {
            smbus->nwk.rxLen = data + 1; // One extra byte due to Byte count
#ifdef SMB_PEC_SUPPORTED
            if(smbus->ctrl.bits.pecEn == 1)
            {
                smbus->nwk.rxLen++;
            }
#endif
        }
    }
    // If no error continue

#ifdef SMB_PEC_SUPPORTED
    if(smbus->ctrl.bits.pecEn == 1)
    {
        SMBus_NWK_addCrc8(&smbus->nwk.pec, data); // Add data to PEC
        data_len--;
    }
#endif

    // Store the received byte
    if(smbus->nwk.rxIndex < data_len)
    {
        smbus->nwk.rxBuffPtr[smbus->nwk.rxIndex] = data;
    }

    smbus->nwk.rxIndex++;

    if(smbus->nwk.rxIndex == (smbus->nwk.rxLen))
    {
        // We are done
        SMBus_PHY_masterSendStop(smbus);
    }
    else if(smbus->nwk.rxIndex == (smbus->nwk.rxLen - 2))
    {
        // We need to send stop at next byte
        SMBus_PHY_masterSendPreStop(smbus);
    }

    return(smbus->state);
}

SMBus_State SMBus_NWK_masterProcessTx(SMBus *smbus,
                                      uint8_t *data)
{
    // Check current state of the State Machine
    if((smbus->nwk.eState != SMBus_NwkState_TX_Block) &&
       (smbus->nwk.eState != SMBus_NwkState_TX) &&
       (smbus->nwk.eState != SMBus_NwkState_TXQuickCMD))
    {
        smbus->nwk.eState = SMBus_NwkState_Error;
        smbus->state = SMBus_State_Master_Error;
        return(smbus->state);
    }

    if(smbus->nwk.txIndex < smbus->nwk.txLen)
    {
        if(smbus->nwk.txIndex == 0)
        {
            // Start by sending the command (1st byte)
            *data = smbus->nwk.currentCmd;
        }
        else if((smbus->nwk.txIndex == 1) &&
                (smbus->nwk.eState == SMBus_NwkState_TX_Block))
        {
            *data = smbus->nwk.txLen - 2;    // Send Byte count
        }
        else
        {
            if(smbus->nwk.eState == SMBus_NwkState_TX_Block)
            {
                *data = smbus->nwk.txBuffPtr[smbus->nwk.txIndex - 2];
            }
            else
            {
                *data = smbus->nwk.txBuffPtr[smbus->nwk.txIndex - 1];
            }
        }
#ifdef SMB_PEC_SUPPORTED
        if(smbus->ctrl.bits.pecEn == 1)
        {
            SMBus_NWK_addCrc8(&smbus->nwk.pec, *data); // Add data to PEC
        }
#endif
    }
    else if(smbus->nwk.txIndex >= smbus->nwk.txLen)
    {
        // Check if we need to Send a restart or Stop transfer
        if(smbus->nwk.rxLen > 0)
        {
            SMBus_NWK_startRxTransfer(smbus);
        }
        else
        {
            // No RX, just send stop or PEC
#ifdef SMB_PEC_SUPPORTED
            if((smbus->nwk.txIndex == (smbus->nwk.txLen)) &&
               (smbus->ctrl.bits.pecEn == 1))
            {
                *data = smbus->nwk.pec;
            }
            else
#endif
            {
                // Last byte, send a stop
                smbus->nwk.eState = SMBus_NwkState_Ending;
                SMBus_PHY_masterSendStop(smbus);
            }
        }
    }

    smbus->nwk.txIndex++;

    return(smbus->state);
}

SMBus_State SMBus_NWK_masterProcessStop(SMBus *smbus)
{
#ifdef SMB_PEC_SUPPORTED
    if((smbus->nwk.eState == SMBus_NwkState_RX) ||
       (smbus->nwk.eState == SMBus_NwkState_RX_Block))
    {
        if(smbus->ctrl.bits.pecEn == 1)
        {
            if(smbus->nwk.pec != 0x00)
            {
                smbus->status.bits.pecErr = 1;
                smbus->nwk.eState = SMBus_NwkState_Idle;
                smbus->state = SMBus_State_PECError;
            }
        }
    }
#endif

    // Set machine to Idle in order to set next packet
    smbus->nwk.eState = SMBus_NwkState_Idle;
    return(smbus->state);
}

void SMBus_NWK_masterReset(SMBus *smbus)
{
    // Restart the SMBus Interface
    SMBus_PHY_disable(smbus);
    SMBus_PHY_masterEnable(smbus);
    // After restart, be ready to accept new packet
    smbus->nwk.eState = SMBus_NwkState_Idle;
}

SMBus_State SMBus_NWK_masterProcessTimeout(SMBus *smbus)
{
    // Signal the error
    smbus->status.bits.toErr = 1;

    SMBus_NWK_masterReset(smbus);

    smbus->state = SMBus_State_TimeOutError;

    return(smbus->state);
}

SMBus_State SMBus_NWK_masterProcessNACK(SMBus *smbus)
{
    if(smbus->nwk.eState != SMBus_NwkState_Ending)
    {
        SMBus_PHY_masterSendStop(smbus);
        smbus->nwk.eState = SMBus_NwkState_Error;
        smbus->state = SMBus_State_Master_NACK;
    }
    else
    {
        smbus->state = SMBus_State_OK;
    }

    return(smbus->state);
}
