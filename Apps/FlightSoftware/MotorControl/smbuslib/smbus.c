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
//
// Include files
//
#include <stdint.h>
#include <msp430.h>
#include "smbus.h"
#include "smbus_nwk.h"
#include "smbus_phy.h"

#if (SMB_MANUAL_ACK_ENABLE == 1)
#if !defined(__MSP430_HAS_DMAX_3__) && !defined(__MSP430_HAS_DMAX_6__)
#error "DMA is required to use Manual ACK "
#endif
#endif

void SMBus_processDone(SMBus *smbus)
{
    // Clear the State of SMBus slave
    smbus->state = SMBus_State_OK;
}

uint8_t SMBus_getRxPayloadAvailable(SMBus *smbus)
{
    return(SMBus_NWK_getRxPayloadSize(smbus));
}

SMBus_State SMBus_getState(SMBus *smbus)
{
    return(smbus->state);
}

#if (SMB_PEC_SUPPORTED == 1)
void SMBus_enablePEC(SMBus *smbus)
{
    smbus->ctrl.bits.pecEn = 1;
}

void SMBus_disablePEC(SMBus *smbus)
{
    smbus->ctrl.bits.pecEn = 0;
}

#endif  //SMB_PEC_SUPPORTED

void SMBus_slaveInit(SMBus *smbus,
                     uint16_t i2cAddr)
{
    smbus->nwk.eState = SMBus_NwkState_Idle;
    smbus->nwk.currentCmd = 0;
    smbus->nwk.currentAddr = 0;
    smbus->nwk.rxBuffPtr = NULL;
    smbus->nwk.rxSize = 0;
    smbus->nwk.rxLen = 0;
    smbus->nwk.rxIndex = 0;
    smbus->nwk.txBuffPtr = NULL;
    smbus->nwk.txSize = 0;
    smbus->nwk.txIndex = 0;
    smbus->nwk.txLen = 0;
    smbus->nwk.recByteTxPtr = &smbus->status.u8byte;
#if (SMB_PEC_SUPPORTED == 1)
    smbus->nwk.pec = 0;
#endif
    smbus->ctrl.u8byte = 0;
    smbus->status.u8byte = 0;
    smbus->state = SMBus_State_OK;
    smbus->ctrl.bits.master = 0;  // Update control flag;

    SMBus_PHY_slaveInit(smbus, i2cAddr);
}

void SMBus_slaveEnableInt(SMBus *smbus)
{
    SMBus_PHY_slaveEnableInt(smbus);
}

SMBus_State SMBus_slaveProcessInt(SMBus *smbus)
{
    SMBus_State New_State;
#if (SMB_MANUAL_ACK_ENABLE == 1)
    New_State = SMBus_PHY_slaveProcessIntDMA(smbus);
    if(New_State == SMBus_State_Slave_NTR)
#endif
    New_State = SMBus_PHY_slaveProcessInt(smbus);

    if(New_State != SMBus_State_Slave_NTR)
    {
        smbus->state = New_State;
    }

    return(New_State);
}

#ifdef __MSP430_HAS_USCI__
SMBus_State SMBus_slaveProcessTimeoutInt(SMBus *smbus)
{
    return(SMBus_PHY_slaveProcessTimeoutInt(smbus));
}

#endif

void SMBus_slaveSetAddress(SMBus *smbus,
                           uint8_t slaveAddr)
{
    // Set new Slave Address
    smbus->ownSlaveAddr = slaveAddr;

    // Restart I2C if it was already enabled
    if(smbus->ctrl.bits.phyEn == 1)
    {
        SMBus_PHY_enable(smbus);
    }
}

void SMBus_slaveSetRxBuffer(SMBus *smbus,
                            uint8_t *data,
                            uint8_t size)
{
    // Set pointer and size of RX Buffer
    smbus->nwk.rxBuffPtr = data;
    smbus->nwk.rxSize = size;
}

void SMBus_slaveSetTxBuffer(SMBus *smbus,
                            uint8_t *data,
                            uint8_t size)
{
    // Set pointer and size of TX Buffer
    smbus->nwk.txBuffPtr = data;
    smbus->nwk.txSize = size;
}

void SMBus_slaveReportError(SMBus *smbus,
                            SMBus_ErrorCode errorCode)
{
    switch(errorCode)
    {
    case SMBus_ErrorCode_Packet:
        smbus->status.bits.packErr = 1;
        break;
    case SMBus_ErrorCode_Cmd:
        smbus->status.bits.cmdErr = 1;
        break;
    default:
        return;
    }
    // Update flags in order to invalidate current packet
    smbus->nwk.eState = SMBus_NwkState_Error;
}

uint8_t SMBus_slaveGetCommand(SMBus *smbus)
{
    return (smbus->nwk.currentCmd);
}

uint8_t SMBus_slaveClearStatusReg(SMBus *smbus,
                                  uint8_t val)
{
    smbus->status.u8byte &= val;
    return(smbus->status.u8byte);
}

uint8_t SMBus_slaveWriteCtrlReg(SMBus *smbus,
                                uint8_t val)
{
    smbus->ctrl.writeableBits.writeBits = val;

    return(smbus->ctrl.u8byte);
}

#if (SMB_MANUAL_ACK_ENABLE == 1)
void SMBus_slaveEnableManualACK(SMBus *smbus)
{
    SMBus_PHY_slaveEnableManualACK(smbus);
}

void SMBus_slaveDisableManualACK(SMBus *smbus)
{
    SMBus_PHY_slaveDisableManualACK(smbus);
}

void SMBus_slaveSendACK(SMBus *smbus,
                        bool sendAck)
{
    SMBus_PHY_slaveSendACK(smbus, sendAck);
}

#endif

void SMBus_masterInit(SMBus *smbus,
                      uint16_t i2cAddr,
                      uint32_t busClk)
{
    smbus->nwk.eState = SMBus_NwkState_Idle;
    smbus->nwk.currentAddr = 0;
    smbus->nwk.rxBuffPtr = NULL;
    smbus->nwk.rxSize = 0;
    smbus->nwk.rxLen = 0;
    smbus->nwk.rxIndex = 0;
    smbus->nwk.txBuffPtr = NULL;
    smbus->nwk.txSize = 0;
    smbus->nwk.txIndex = 0;
    smbus->nwk.txLen = 0;
    smbus->nwk.recByteTxPtr = &smbus->status.u8byte;
#ifdef SMB_PEC_SUPPORTED
    smbus->nwk.pec = 0;
#endif
    smbus->ctrl.u8byte = 0;
    smbus->status.u8byte = 0;
    smbus->state = SMBus_State_OK;
    smbus->ctrl.bits.master = 1;  // Update control flag;

    SMBus_PHY_masterInit(smbus, i2cAddr, busClk);
}

void SMBus_masterEnableInt(SMBus *smbus)
{
    SMBus_PHY_masterEnableInt(smbus);
}

SMBus_State SMBus_masterProcessInt(SMBus *smbus)
{
    return(SMBus_PHY_masterProcessInt(smbus));
}

void SMBus_masterReset(SMBus *smbus)
{
    SMBus_NWK_masterReset(smbus);
}

#ifdef __MSP430_HAS_USCI__
SMBus_State SMBus_masterProcessTimeoutInt(SMBus *smbus)
{
    return(SMBus_PHY_masterProcessTimeoutInt(smbus));
}

#endif

int8_t SMBus_masterProcessCall(SMBus *smbus,
                               uint8_t targetAddr,
                               uint8_t command,
                               uint8_t *txData,
                               uint8_t *rxData)
{
    if(smbus->nwk.eState != SMBus_NwkState_Idle)
    {
        return(SMBUS_RET_ERROR);
    }

    smbus->nwk.currentCmd = command;
    smbus->nwk.rxLen = 2; // Read 2 bytes
    smbus->nwk.rxBuffPtr = rxData;
    smbus->nwk.txLen = 3; // 2 bytes + Cmd
    smbus->nwk.txBuffPtr = txData;
    smbus->nwk.eState = SMBus_NwkState_TX;
    smbus->nwk.currentAddr = targetAddr;

    SMBus_NWK_startTxTransfer(smbus);

    return(SMBUS_RET_OK);
}

int8_t SMBus_masterProcessCallBlock(SMBus *smbus,
                                    uint8_t targetAddr,
                                    uint8_t command,
                                    uint8_t *txData,
                                    uint8_t txSize,
                                    uint8_t *rxData)
{
    if(smbus->nwk.eState != SMBus_NwkState_Idle)
    {
        return(SMBUS_RET_ERROR);
    }

    if((txSize == 0) || (txSize > SMB_MAX_PAYLOAD_SIZE))
    {
        return(SMBUS_RET_ERROR);
    }

    smbus->nwk.currentCmd = command;
    smbus->nwk.rxLen = 3; // Minimal size until we get the real Byte count
    smbus->nwk.rxBuffPtr = rxData;
    smbus->nwk.txLen = txSize + 2; // Size includes Command and byte count
    smbus->nwk.txBuffPtr = txData;
    smbus->nwk.eState = SMBus_NwkState_TX_Block;
    smbus->nwk.currentAddr = targetAddr;

    SMBus_NWK_startTxTransfer(smbus);

    return(SMBUS_RET_OK);
}

int8_t SMBus_masterSendByte(SMBus *smbus,
                            uint8_t targetAddr,
                            uint8_t txData)
{
    if(smbus->nwk.eState != SMBus_NwkState_Idle)
    {
        return(SMBUS_RET_ERROR);
    }
    smbus->nwk.currentCmd = txData;
    smbus->nwk.rxLen = 0;  //No read
    smbus->nwk.txLen = 1; // Only Command(Data)
    smbus->nwk.eState = SMBus_NwkState_TX;
    smbus->nwk.currentAddr = targetAddr;

    SMBus_NWK_startTxTransfer(smbus);

    return(SMBUS_RET_OK);
}

int8_t SMBus_masterReceiveByte(SMBus *smbus,
                               uint8_t targetAddr,
                               uint8_t *rxData)
{
    if(smbus->nwk.eState != SMBus_NwkState_Idle)
    {
        return(SMBUS_RET_ERROR);
    }
    smbus->nwk.rxLen = 1;  //Read 1 byte
    smbus->nwk.rxBuffPtr = rxData;
    smbus->nwk.txLen = 0; // Only Command(Data)
    smbus->nwk.eState = SMBus_NwkState_RX;
    smbus->nwk.currentAddr = targetAddr;

    // Send Start and stop after address is sent
    SMBus_NWK_startRxTransfer(smbus);

    return(SMBUS_RET_OK);
}

int8_t SMBus_masterReadBlock(SMBus *smbus,
                             uint8_t targetAddr,
                             uint8_t command,
                             uint8_t *rxData)
{
    if(smbus->nwk.eState != SMBus_NwkState_Idle)
    {
        return(SMBUS_RET_ERROR);
    }

    smbus->nwk.currentCmd = command;
    smbus->nwk.rxLen = 3; // Minimal size until we get the real Byte count
    smbus->nwk.rxBuffPtr = rxData;
    smbus->nwk.txLen = 1; // Only send command
    smbus->nwk.eState = SMBus_NwkState_TX_Block;
    smbus->nwk.currentAddr = targetAddr;

    SMBus_NWK_startTxTransfer(smbus);

    return(SMBUS_RET_OK);
}

int8_t SMBus_masterWriteBlock(SMBus *smbus,
                              uint8_t targetAddr,
                              uint8_t command,
                              uint8_t *txData,
                              uint8_t txSize)
{
    if(smbus->nwk.eState != SMBus_NwkState_Idle)
    {
        return(SMBUS_RET_ERROR);
    }
    if((txSize == 0) || (txSize > SMB_MAX_PAYLOAD_SIZE))
    {
        return(SMBUS_RET_ERROR);
    }

    smbus->nwk.currentCmd = command;
    smbus->nwk.rxLen = 0;
    smbus->nwk.txLen = txSize + 2; // Size includes Command and byte count
    smbus->nwk.txBuffPtr = txData;
    smbus->nwk.eState = SMBus_NwkState_TX_Block;
    smbus->nwk.currentAddr = targetAddr;

    SMBus_NWK_startTxTransfer(smbus);

    return(SMBUS_RET_OK);
}

int8_t SMBus_masterReadByteWord(SMBus *smbus,
                                uint8_t targetAddr,
                                uint8_t command,
                                uint8_t *rxData,
                                uint8_t rxSize)
{
    if(smbus->nwk.eState != SMBus_NwkState_Idle)
    {
        return(SMBUS_RET_ERROR);
    }

    if((rxSize != 1) && (rxSize != 2))
    {
        return(SMBUS_RET_ERROR);
    }

    smbus->nwk.currentCmd = command;
    smbus->nwk.rxLen = rxSize; // Read 1 or 2 bytes
    smbus->nwk.rxBuffPtr = rxData;
    smbus->nwk.txLen = 1; // Only send command
    smbus->nwk.eState = SMBus_NwkState_TX;
    smbus->nwk.currentAddr = targetAddr;

    SMBus_NWK_startTxTransfer(smbus);

    return(SMBUS_RET_OK);
}

int8_t SMBus_masterWriteByteWord(SMBus *smbus,
                                 uint8_t targetAddr,
                                 uint8_t command,
                                 uint8_t *txData,
                                 uint8_t txSize)
{
    if(smbus->nwk.eState != SMBus_NwkState_Idle)
    {
        return(SMBUS_RET_ERROR);
    }

    if((txSize != 1) && (txSize != 2))
    {
        return(SMBUS_RET_ERROR);
    }

    smbus->nwk.currentCmd = command;
    smbus->nwk.rxLen = 0; // Read 1 or 2 bytes
    smbus->nwk.txLen = 1 + txSize; // Command + Size
    smbus->nwk.txBuffPtr = txData;
    smbus->nwk.eState = SMBus_NwkState_TX;
    smbus->nwk.currentAddr = targetAddr;

    SMBus_NWK_startTxTransfer(smbus);

    return(SMBUS_RET_OK);
}

int8_t SMBus_masterQuickCommand(SMBus *smbus,
                                uint8_t targetAddr,
                                bool write)
{
    if(smbus->nwk.eState != SMBus_NwkState_Idle)
    {
        return(SMBUS_RET_ERROR);
    }

    if(write != true)
    {
        return(SMBUS_RET_ERROR);
    }

    smbus->nwk.rxLen = 0;  //No read
    smbus->nwk.txLen = 0; // No Command
    smbus->nwk.eState = SMBus_NwkState_TXQuickCMD;
    smbus->nwk.currentAddr = targetAddr;

    if(write == true)
    {
        SMBus_NWK_startTxTransfer(smbus);
    }

    return(SMBUS_RET_OK);
}

int8_t SMBus_masterWaitUntilDone(SMBus *smbus,
                                 int32_t timeout)
{
    while((smbus->nwk.eState != SMBus_NwkState_Idle) && timeout)
    {
        timeout--;
    }

    if(!timeout)
    {
        return(SMBUS_RET_ERROR);
    }
    else
    {
        return(SMBUS_RET_OK);
    }
}
