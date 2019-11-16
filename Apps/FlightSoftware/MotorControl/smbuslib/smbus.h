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
#ifndef __SMBUS_H__
#define __SMBUS_H__

//*****************************************************************************
//
//! \addtogroup smbus Application API layer
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//
// Include files
//
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

//*****************************************************************************
// defines
//*****************************************************************************

//*****************************************************************************
//
//! Define to support Packet Error Checking (PEC)
//
//*****************************************************************************
#define SMB_PEC_SUPPORTED (1)

//*****************************************************************************
//
//! Define to enable Manual ACK workaround
//
//! The MSP403 eUSCI doesn't support tesing a received byte and NACK/ACK immediately.
//! This workaround uses DMA to stretch SCL low immediately after a byte is
//! received. The DMA ISR is attended and the application then decides if it
//! needs to ACK or NACK the byte
//
//! This capability is not supported on devices that do not have DMA (e.g.
//! MSP430G2xxx.
//
//*****************************************************************************
#define SMB_MANUAL_ACK_ENABLE  (0)

//*****************************************************************************
//
//! Define method used to calculate CRC8:
//!     - \b SMB_CRC8_USE_LOOKUP_TABLE == 1 - Use a 256B lookup table (faster but
//!       takes more memory)
//!     - \b SMB_CRC8_USE_LOOKUP_TABLE == 0 - Calculate CRC8 manually (slower
//!       but takes less memory)
//
//*****************************************************************************
#define SMB_CRC8_USE_LOOKUP_TABLE (1)

//*****************************************************************************
//
//! Maximum payload as specified by SMBus Spec
//
//*****************************************************************************
#define SMB_MAX_PAYLOAD_SIZE                    (32)

//*****************************************************************************
//
//! Max packet size = Payload+PEC+CMD+Len
//
//*****************************************************************************
#define SMB_MAX_PACKET_SIZE                     (SMB_MAX_PAYLOAD_SIZE + 3)

//*****************************************************************************
//
//! Default response when there's nothing to send
//
//*****************************************************************************
#define RESPONSE_NTR                             0x00

//*****************************************************************************
//
//! Return value when successful
//
//*****************************************************************************
#define SMBUS_RET_OK                            (1)

//*****************************************************************************
//
//! Return value when an error ocurred
//
//*****************************************************************************
#define SMBUS_RET_ERROR                         (-1)

//*****************************************************************************
// typedefs
//*****************************************************************************

//*****************************************************************************
//
//! List of stop codes used within the NWK and PHY layers
//
//*****************************************************************************
typedef enum
{
    SMBus_Stop_No = 0,        //! No Stop send
    SMBus_Stop_Immediate,     //! Stop is send immediately with Start
    SMBus_Stop_Addr,          //! Stop is send after Start
    SMBus_Stop_PreRead,       //! Stop sent before reading next byte
    SMBus_Stop_Sent,          //! Stop sent
} SMBus_Stop;

//*****************************************************************************
//
//! SMBus control register
//
//*****************************************************************************
typedef union
{
    //*****************************************************************************
    //
    //! Defines the control bit fields
    //
    //*****************************************************************************
    struct
    {
        uint8_t pecEn      : 1;  //! Enables PEC functionality
        uint8_t swackEn    : 1;  //! SW_ACK is enabled (read only)
        uint8_t intEn      : 1;  //! Interupts are enabled (read only)
        uint8_t phyEn      : 1;  //! SMBus PHY is enabled (read only)
        uint8_t master     : 1;  //! Acting in Master mode (read only)
        uint8_t reserved2  : 3;  //! Reserved
    } bits;
    //*****************************************************************************
    //
    //! Allows access to the writeable bits on the structure
    //
    //*****************************************************************************
    struct
    {
        uint8_t writeBits  : 1; //! Control bits that are writable
        uint8_t reserved   : 7; //! Masks the read only control
    } writeableBits;
    //! Whole Control byte access
    uint8_t u8byte;
} SMBus_Ctrl;

//*****************************************************************************
//
//! Physical and Data Link Layer object
//
//*****************************************************************************
typedef struct
{
    uint16_t SMBus_Phy_i2cBase;    //! I2C Base address
    SMBus_Stop SMBus_Phy_stop;     //! Send different types of Stop as master
} SMBus_Phy;

//*****************************************************************************
//
//! SMBus network layer states
//
//*****************************************************************************
typedef enum
{
    SMBus_NwkState_Idle = 0,    //! Network is idle and waiting for new packet
    SMBus_NwkState_RX,          //! Network is receiving a packet
    SMBus_NwkState_TX,          //! Network is transmitting after receive byte
    SMBus_NwkState_TXQuickCMD,  //! Network is sending Quick Command
    SMBus_NwkState_TX_Resp,     //! Network is transmitting a response after restart
    SMBus_NwkState_TX_Block,    //! Network is transmitting a block
    SMBus_NwkState_RX_Block,    //! Network is receiving a block
    SMBus_NwkState_Ending,      //! Network is finishing transfer
    SMBus_NwkState_Error        //! Network error detected
} SMBus_NwkState;

//*****************************************************************************
//
//! Definition of SMBus Network structure
//
//*****************************************************************************
typedef struct
{
    volatile SMBus_NwkState eState;       //! Network state machine
    uint8_t currentAddr;                  //! Current Address+R/W
    uint8_t currentCmd;                   //! Current Command
    uint8_t rxIndex;                      //! RX Byte counter
    uint8_t rxLen;                        //! Bytes to receive
    uint8_t rxSize;                       //! Max size of buffer
    uint8_t *rxBuffPtr;                   //! Reception Buffer pointer
    uint8_t txIndex;                      //! Byte counter
    uint8_t txLen;                        //! Bytes to send
    uint8_t *txBuffPtr;                   //! Transmission pointer
    uint8_t *recByteTxPtr;                //! Receive Byte response
    uint8_t txSize;                       //! Max size of buffer
#if (SMB_PEC_SUPPORTED == 1)
    uint8_t pec;                          //! Current PEC value
#endif
} SMBus_Nwk;

//*****************************************************************************
//
//! List of error codes used by the application to indicate an error to the library
//
//*****************************************************************************
typedef enum
{
    SMBus_ErrorCode_NoError = 0,   //! No error detected
    SMBus_ErrorCode_Packet,        //! Incorrect packet was received
    SMBus_ErrorCode_Cmd            //! Command is not supported
} SMBus_ErrorCode;

//*****************************************************************************
//
//! SMBus state sent to application layer
//
//*****************************************************************************
typedef enum
{
    SMBus_State_OK = 0,                 //! Nothing special to report
    SMBus_State_DataSizeError,          //! Incorrect packet size
    SMBus_State_PECError,               //! PEC Error detected
    SMBus_State_TimeOutError,           //! Timeout Error
    SMBus_State_Slave_FirstByte,        //! 1st byte (cmd) received
    SMBus_State_Slave_ByteReceived,     //! Slave received a byte (2-n)
    SMBus_State_Slave_QCMD,             //! Quick Command detected
    SMBus_State_Slave_CmdComplete,      //! Complete packet received by slave
    SMBus_State_Slave_Error,            //! SMBus Slave Error
    SMBus_State_Slave_NotReady,         //! SMBus Buffers haven't been initialized
    SMBus_State_Slave_NTR,              //! No Interrupt flags detected
    SMBus_State_Master_ArbLost,         //! Arbitration Lost
    SMBus_State_Master_NACK,            //! Unexpected NACKed
    SMBus_State_Master_Error,           //! SMBus Master error
    SMBus_State_Unknown
} SMBus_State;

//*****************************************************************************
//
//! SMBus Status Register
//
//*****************************************************************************
typedef union
{
    //*****************************************************************************
    //
    //! Status flag register
    //
    //*****************************************************************************
    struct
    {
        uint8_t pecErr      : 1;    //! PEC error
        uint8_t toErr       : 1;    //! Timeout error
        uint8_t packErr     : 1;    //! Error in packet format
        uint8_t packOvrErr  : 1;    //! Packet Overrun error
        uint8_t byteOvrErr  : 1;    //! Byte Overrun error
        uint8_t cmdErr      : 1;    //! Incorrect command
        uint8_t reserved    : 2;    //! Reserved bits
    } bits;
    //! Whole status byte access
    uint8_t u8byte;
} SMBus_Status;

//*****************************************************************************
//
//! Main SMBus object
//
//*****************************************************************************
typedef struct
{
    SMBus_Phy phy;                  //! PHY and DataLink object
    SMBus_Nwk nwk;                  //! Network object
    SMBus_Ctrl ctrl;                //! SMBus Control register
    SMBus_Status status;            //! SMBus Status register
    SMBus_State state;              //! SMBus reported state
    uint8_t ownSlaveAddr;           //! Own Slave address
} SMBus;

//*****************************************************************************
// globals
//*****************************************************************************

//*****************************************************************************
// Public functions called by applications
//*****************************************************************************
//*****************************************************************************
//
//! \brief   Clears the current state of SMBus
//
//!  Must be called by application in order to clear the state machine
//!  when a byte/packet was processed
//
//! \param smbus     Pointer to SMBus structure
//
//! \return  None
//
//*****************************************************************************
extern void SMBus_processDone(SMBus *smbus);

//*****************************************************************************
//
//! \brief   Returns the number of received bytes from last transaction
//
//! \param smbus     Pointer to SMBus structure
//
//! \return  Number of bytes in the RX buffer. PEC byte is *not* included.
//
//*****************************************************************************
extern uint8_t SMBus_getRxPayloadAvailable(SMBus *smbus);

//*****************************************************************************
//
//! \brief  Returns the state of the SMBus module
//
//! \param smbus    Pointer to SMBus structure
//
//! \return State of the SMBus module
//
//*****************************************************************************
extern SMBus_State SMBus_getState(SMBus *smbus);

#if (SMB_PEC_SUPPORTED == 1)
//*****************************************************************************
//
//! \brief   Enables PEC support
//
//! \param smbus    Pointer to SMBus structure
//
//! \return  None
//
//*****************************************************************************
extern void SMBus_enablePEC(SMBus *smbus);
//*****************************************************************************
//
//! \brief   Disables PEC support
//
//! \param smbus    Pointer to SMBus structure
//
//! \return  None
//
//*****************************************************************************
extern void SMBus_disablePEC(SMBus *smbus);
#endif

//*****************************************************************************
//
//! \brief   Initialize the SMBus interface as a slave
//
//! Initializes the NWK and PHY layers.
//
//!  \param smbus     Pointer to SMBus structure
//!  \param i2cAddr   Base address of I2C module. For MSP430G2xxx devices,
//!                   this parameter is ignored.
//
//! \return  None
//
//*****************************************************************************
extern void SMBus_slaveInit(SMBus *smbus,
                            uint16_t i2cAddr);

//*****************************************************************************
//
//! \brief   Enables the I2C interrupts for a slave
//
//! This function enables the eUSCI Start,Stop, RX,TX, Timeout interrupts
//! If \b SMB_MANUAL_ACK_ENABLE is enabled, it enables DMA to handle the RX
//! SMBus_slaveInit() must be called before this function
//
//! \param smbus     Pointer to SMBus structure
//
//! \return  None
//
//*****************************************************************************
extern void SMBus_slaveEnableInt(SMBus *smbus);

//*****************************************************************************
//
//! \brief   I2C Interrupt Service routine for a slave
//
//! Handles the interrupts for SMBus passing information to NWK layer
//! Should be called by application when USCI/DMA interrupt is detected
//! Note that this routine also checks the DMA due to the SW ACK workaround if
//! \b SMB_MANUAL_ACK_ENABLE is defined.
//
//! \param smbus    Pointer to SMBus structure
//
//! \return  Processing State (SMBus_State)
//!       - \b SMBus_State_Slave_NotReady - Packet is not ready
//!       - \b SMBus_State_Slave_FirstByte - First byte received (application can use
//!                                    it to validate the command)
//!       - \b SMBus_State_Slave_ByteReceived - Byte 2+ received (application can use
//!                                    it to validate each byte)
//!       - \b SMBus_State_Slave_QCMD - Quick command received
//!       - \b SMBus_State_Slave_CmdComplete - Packet complete and if PEC enabled,
//!                                    validated.
//
//*****************************************************************************
extern SMBus_State SMBus_slaveProcessInt(SMBus *smbus);

//*****************************************************************************
//
//! \brief   Force reset to SMBus master interface
//
//! Resets the network and PHY layers
//
//! \param smbus    Pointer to SMBus structure
//
//! \return  None
//
//*****************************************************************************
extern void SMBus_masterReset(SMBus *smbus);

#ifdef __MSP430_HAS_USCI__
//*****************************************************************************
//
//! \brief   Timer interrupt service routine for slave application
//
//! Handles the interrupts for SMBus tiemout processing for USCI devices.
//! Should be called by application when Timer interrupt is detected
//
//! \param smbus    Pointer to SMBus structure
//
//! \return  Processing State (SMBus_State):
//!           - \b SMBus_State_TimeOutError - Timeout ocurred, SMBUS has
//!                                           been restarted
//
//*****************************************************************************
extern SMBus_State SMBus_slaveProcessTimeoutInt(SMBus *smbus);
#endif

//*****************************************************************************
//
//! \brief  Set the slave's own I2C address
//
//! \param smbus     Pointer to SMBus structure
//! \param slaveAddr  Slave I2C address
//
//! \return  None
//
//*****************************************************************************
extern void SMBus_slaveSetAddress(SMBus *smbus,
                                  uint8_t slaveAddr);

//*****************************************************************************
//
//! \brief   Initialize the reception buffer for slave
//
//! \param smbus     Pointer to SMBus structure
//! \param data      Pointer to Application RX buffer
//! \param size       Maximum size of buffer
//
//! \return  None
//
//*****************************************************************************
extern void SMBus_slaveSetRxBuffer(SMBus *smbus,
                                   uint8_t *data,
                                   uint8_t size);

//*****************************************************************************
//
//! \brief   Initialize the transmission buffer for slave
//
//! \param smbus    Pointer to SMBus structure
//! \param data     Pointer to Application TX buffer
//! \param size      Maximum size of buffer
//
//! \return  None
//
//*****************************************************************************
extern void SMBus_slaveSetTxBuffer(SMBus *smbus,
                                   uint8_t *data,
                                   uint8_t size);

//*****************************************************************************
//
//! \brief   Reports an error to SMBus driver from the slave
//
//! Used to signal an error when incorrect command/data is detected by the slave
//
//! \param smbus       Pointer to SMBus structure
//! \param errorCode    SMBus_ErrorCode
//
//! \return  None
//
//*****************************************************************************
extern void SMBus_slaveReportError(SMBus *smbus,
                                   SMBus_ErrorCode errorCode);

//*****************************************************************************
//
//! \brief   Return the current command (Rxbuffer[0]) received by the slave
//
//! \param smbus       Pointer to SMBus structure
//
//! \return  Current command byte
//
//*****************************************************************************
extern uint8_t SMBus_slaveGetCommand(SMBus *smbus);

//*****************************************************************************
//
//! \brief   Clear the slave's status register
//
//! \param smbus    Pointer to SMBus structure
//! \param val       Bits cleared from status register (1=X, 0=clear)
//
//! \return  Value of Status register after clearing flags
//
//*****************************************************************************
extern uint8_t SMBus_slaveClearStatusReg(SMBus *smbus,
                                         uint8_t val);

//*****************************************************************************
//
//! \brief   Write a value to the slave's control register
//
//! \param smbus    Pointer to SMBus structure
//! \param  val      Value being written to the Control register
//
//! \return  Value of Control register after write
//
//*****************************************************************************
extern uint8_t SMBus_slaveWriteCtrlReg(SMBus *smbus,
                                       uint8_t val);

#if (SMB_MANUAL_ACK_ENABLE == 1)
//*****************************************************************************
//
//! \brief   Enables functionality for the slave to perform manual ACK/NACK
//
//!  DMA will be used to stretch SCL
//
//! \param smbus     Pointer to SMBus structure
//
//! \return  None
//
//*****************************************************************************
extern void SMBus_slaveEnableManualACK(SMBus *smbus);
//*****************************************************************************
//
//! \brief   Disables functionality for the slave to perform manual ACK/NACK
//
//! \param smbus     Pointer to SMBus structure
//
//! \return  None
//
//*****************************************************************************
extern void SMBus_slaveDisableManualACK(SMBus *smbus);

//*****************************************************************************
//
//! \brief Send the ACK/NACK from the slave
//
//! \param smbus    Pointer to SMBus structure
//! \param sendAck   if true, sends ACK. If false, sends NACK
//
//! \return  None
//
//*****************************************************************************
extern void SMBus_slaveSendACK(SMBus *smbus,
                               bool sendAck);
#endif

//*****************************************************************************
//
//! \brief   Initialize the SMBus Interface for a master
//
//! Initializes the NWK and PHY layers
//
//! \param smbus     Pointer to SMBus structure
//! \param i2cAddr   Base address of I2C module. For MSP430G2xxx devices,
//!                  this parameter is ignored.
//! \param busClk    SMCLK Frequency
//
// \return  None
//
//*****************************************************************************
extern void SMBus_masterInit(SMBus *smbus,
                             uint16_t i2cAddr,
                             uint32_t busClk);

//*****************************************************************************
//
//! \brief   Enables the I2C interrupts for a master
//
//! This function enables the eUSCI Start,Stop, RX,TX, Timeout interrupts.
//! SMBus_masterInit() must be called before this function.
//! If \b SMB_MANUAL_ACK_ENABLE is enabled, it enables DMA to handle the RX.
//
//! \param smbus     Pointer to SMBus structure
//
//! \return  none
//
//*****************************************************************************
extern void SMBus_masterEnableInt(SMBus *smbus);

//*****************************************************************************
//
//! \brief   I2C Interrupt Service routine for a master
//
//! Handles the interrupts for SMBus passing information to NWK layer
//! Should be called by application when USCI interrupt is detected
//
//! \param smbus    Pointer to SMBus structure
//
//! \return  Processing State (SMBus_State)
//
//*****************************************************************************
extern SMBus_State SMBus_masterProcessInt(SMBus *smbus);

#ifdef __MSP430_HAS_USCI__
//*****************************************************************************
//
//! \brief   Timer interrupt service routine for a master
//
//! Handles the interrupts for SMBus tiemout processing for USCI devices.
//! Should be called by application when Timer interrupt is detected
//
//! \param smbus    Pointer to SMBus structure
//
//! \return  Processing State (SMBus_State):
//
//*****************************************************************************
extern SMBus_State SMBus_masterProcessTimeoutInt(SMBus *smbus);
#endif

//*****************************************************************************
//
//! \brief   Sends a process call to a slave
//
//! Send process call to the slave. A command byte and 2 bytes of TX data are
//! required. Two bytes of data will be returned by the slave in rxData.
//!
//!~~~~~~~~
//! SMBus Process Call command protocol
//!
//! Process Call:
//!      1         7         1    1         8        1       8           1          8         1  
//!    ------------------------------------------------------------------------------------------
//!    | S | Slave Address | Wr | A | Command Code | A | Data Byte Low | A | Data Byte High | A | ...
//!    ------------------------------------------------------------------------------------------
//!      1         7          1    1         8         1       8           1    1  
//!    ----------------------------------------------------------------------------
//!    | Sr | Slave Address | Rd | A | Data Byte Low | A | Data Byte High | A | P |
//!    ----------------------------------------------------------------------------
//! Process Call with PEC:
//!      1         7         1    1         8        1       8           1          8         1  
//!    ------------------------------------------------------------------------------------------
//!    | S | Slave Address | Wr | A | Command Code | A | Data Byte Low | A | Data Byte High | A | ...
//!    ------------------------------------------------------------------------------------------
//!      1         7          1    1         8         1       8            1    8    1   1  
//!    --------------------------------------------------------------------------------------
//!    | Sr | Slave Address | Rd | A | Data Byte Low | A | Data Byte High | A | PEC | A | P |
//!    --------------------------------------------------------------------------------------
//!
//! where: 
//!     S = Start bit
//!     Sr = Reapeated Start bit
//!     Wr = Write bit (0)
//!     Slave Adddress = SMBus address for slave
//!     Command Code = Command byte sent to slave
//!     Data Byte = data sent to slave
//!     A = Acknowledge from slave
//!     PEC = Optional Packet Error Code
//!     P = Stop bit
//!~~~~~~~~
//
//! \param smbus     Pointer to SMBus structure
//! \param targetAddr Slave address
//! \param command    Command byte for slave
//! \param txData    TX data buffer
//! \param rxData    RX data buffer
//
//! \return  \b SMBUS_RET_ERROR, or \b SMBUS_RET_OK
//
//*****************************************************************************
extern int8_t SMBus_masterProcessCall(SMBus *smbus,
                                      uint8_t targetAddr,
                                      uint8_t command,
                                      uint8_t *txData,
                                      uint8_t *rxData);

//*****************************************************************************
//
//! \brief   Sends a block write-block read process call
//
//! Send block write-block read process call to the slave. A command byte,
//! length and tx data byte  array are required. Ensure that rxData is large
//! enough to hold the data received from the slave.
//
//!
//!~~~~~~~~
//! SMBus Block write-block read process call protocol
//!
//! Block write-block read process call:
//!      1         7         1    1         8        1         8          1       8         1
//!    ----------------------------------------------------------------------------------------
//!    | S | Slave Address | Wr | A | Command Code | A | Byte Count = M | A | Data Byte 1 | A | ...
//!    ----------------------------------------------------------------------------------------
//!           8        1                8        1  
//!    -------------------       -------------------
//!    | Data Byte 2 | A |  ...  | Data Byte M | A | ...
//!    -------------------       -------------------
//!       1        7           1   1         8          1       8         1
//!    ----------------------------------------------------------------------
//!    | Sr | Slave Address | Rd | A | Byte Count = N | A | Data Byte 1 | A | ...
//!    ----------------------------------------------------------------------
//!           8        1                8        1   1 
//!    -------------------       -----------------------
//!    | Data Byte 2 | A |  ...  | Data Byte N | A | P |
//!    -------------------       -----------------------
//! Block write-block read process call with PEC:
//!      1         7         1    1         8        1         8          1       8         1
//!    ----------------------------------------------------------------------------------------
//!    | S | Slave Address | Wr | A | Command Code | A | Byte Count = M | A | Data Byte 1 | A | ...
//!    ----------------------------------------------------------------------------------------
//!           8        1                8        1  
//!    -------------------       -------------------
//!    | Data Byte 2 | A |  ...  | Data Byte M | A | ...
//!    -------------------       -------------------
//!       1        7           1   1         8          1       8         1
//!    ----------------------------------------------------------------------
//!    | Sr | Slave Address | Rd | A | Byte Count = N | A | Data Byte 1 | A | ...
//!    ----------------------------------------------------------------------
//!           8        1                8        1    8    1   1 
//!    -------------------       ---------------------------------
//!    | Data Byte 2 | A |  ...  | Data Byte N | A | PEC | A | P |
//!    -------------------       ---------------------------------
//!
//! where: 
//!     S = Start bit
//!     Sr = Reapeated Start bit
//!     Wr = Write bit (0)
//!     Rd = Read bit (1)
//!     Slave Adddress = SMBus address for slave
//!     Command Code = Command byte sent to slave
//!     Data Byte = data sent to slave
//!     A = Acknowledge from slave
//!     PEC = Optional Packet Error Code
//!     P = Stop bit
//!~~~~~~~~
//
//! \param smbus     Pointer to SMBus structure
//! \param targetAddr Slave address
//! \param command    Command byte for slave
//! \param txData    TX data buffer
//! \param txSize    Size of the txData buffer
//! \param rxData    RX data buffer
//
//! \return  \b SMBUS_RET_ERROR, or \b SMBUS_RET_OK
//
//*****************************************************************************
extern int8_t SMBus_masterProcessCallBlock(SMBus *smbus,
                                           uint8_t targetAddr,
                                           uint8_t command,
                                           uint8_t *txData,
                                           uint8_t txSize,
                                           uint8_t *rxData);

//*****************************************************************************
//
//! \brief   Sends byte to the slave
//
//!
//!~~~~~~~~
//! SMBus Send Byte command protocol
//!
//!      1         7         1    1        8       1   1
//!    ---------------------------------------------------
//!    | S | Slave Address | Wr | A |  Data Byte | A | P |
//!    ---------------------------------------------------
//! Send Byte with PEC:
//!      1         7         1    1       8       1    8    1   1
//!    ------------------------------------------------------------
//!    | S | Slave Address | Wr | A | Data Byte | A | PEC | A | P | 
//!    ------------------------------------------------------------
//!
//! where: 
//!     S = Start bit
//!     Wr = Write bit (0)
//!     Slave Adddress = SMBus address for slave
//!     Data Byte = data sent to slave
//!     A = Acknowledge from slave
//!     PEC = Optional Packet Error Code
//!     P = Stop bit
//!~~~~~~~~
//
//! \param smbus     Pointer to SMBus structure
//! \param targetAddr Slave address
//! \param txData    TX data buffer
//
//! \return  \b SMBUS_RET_ERROR, or \b SMBUS_RET_OK
//
//*****************************************************************************
extern int8_t SMBus_masterSendByte(SMBus *smbus,
                                   uint8_t targetAddr,
                                   uint8_t txData);

//*****************************************************************************
//
//! \brief   Receive a byte from the slave
//
//!~~~~~~~~
//! SMBus Receive Byte command protocol
//!
//!      1         7         1    1       8       1   1
//!    --------------------------------------------------
//!    | S | Slave Address | Rd | A | Data Byte | A | P |
//!    --------------------------------------------------
//! With PEC:
//!      1         7         1    1       8       1    8    1   1
//!    ------------------------------------------------------------
//!    | S | Slave Address | Rd | A | Data Byte | A | PEC | A | P |
//!    ------------------------------------------------------------
//!
//! where: 
//!     S = Start bit
//!     Rd = Read bit (1)
//!     Slave Adddress = SMBus address for slave
//!     Data Byte = data received from slave
//!     A = Acknowledge from slave
//!     PEC = Optional Packet Error Code
//!     P = Stop bit
//!~~~~~~~~
//
//! \param smbus     Pointer to SMBus structure
//! \param targetAddr Slave address
//! \param rxData    RX data buffer
//
//! \return  \b SMBUS_RET_ERROR, or \b SMBUS_RET_OK
//
//*****************************************************************************
extern int8_t SMBus_masterReceiveByte(SMBus *smbus,
                                      uint8_t targetAddr,
                                      uint8_t *rxData);

//*****************************************************************************
//
//! \brief   Receive a block of data from the slave
//
//! Send block data receive call to the slave. A command byte, length and rx
//! data byte array are required. Ensure that rxData is large enough to hold
//! the data received from the slave.
//
//!
//!~~~~~~~~
//! SMBus Block Read command protocol
//!
//! Block Read:
//!      1         7         1    1         8        1   1         8          1    1  
//!    -------------------------------------------------------------------------------
//!    | S | Slave Address | Wr | A | Command Code | A | Sr | Slave Address | Rd | A | ...
//!    -------------------------------------------------------------------------------
//!              8        1        8        1        8        1                8        1   1
//!    ----------------------------------------------------------------------------------------
//!    | Byte Count = N | A | Data Byte 1 | A | Data Byte 2 | A |  ...  | Data Byte N | A | P |
//!    ----------------------------------------------------------------------------------------
//! Block Read with PEC:
//!      1         7         1    1         8        1   1         8          1    1  
//!    -------------------------------------------------------------------------------
//!    | S | Slave Address | Wr | A | Command Code | A | Sr | Slave Address | Rd | A | ...
//!    -------------------------------------------------------------------------------
//!              8        1        8        1        8        1                8        1  
//!    ------------------------------------------------------------------------------------
//!    | Byte Count = N | A | Data Byte 1 | A | Data Byte 2 | A |  ...  | Data Byte N | A |...
//!    ------------------------------------------------------------------------------------
//!       8    1   1
//!    ---------------
//!    | PEC | A | P |
//!    ---------------
//!
//! where: 
//!     S = Start bit
//!     Sr = Reapeated Start bit
//!     Wr = Write bit (0)
//!     Rd = Read bit (1)
//!     Slave Adddress = SMBus address for slave
//!     Command Code = Command byte sent to slave
//!     Data Byte = data sent to slave
//!     A = Acknowledge from slave
//!     PEC = Optional Packet Error Code
//!     P = Stop bit
//!~~~~~~~~
//
//! \param smbus     Pointer to SMBus structure
//! \param targetAddr Slave address
//! \param command    Command byte for slave
//! \param rxData    RX data buffer
//
//! \return  \b SMBUS_RET_ERROR, or \b SMBUS_RET_OK
//
//*****************************************************************************
extern int8_t SMBus_masterReadBlock(SMBus *smbus,
                                    uint8_t targetAddr,
                                    uint8_t command,
                                    uint8_t *rxData);

//*****************************************************************************
//
//! \brief   Transmit a block of data to the slave
//
//! Send block of data to the slave. A command byte, length and tx
//! data byte array are required.
//
//!
//!~~~~~~~~
//! SMBus Block Write command protocol
//!
//! Block Write:
//!      1         7         1    1         8        1         8          1          8      1  
//!    ----------------------------------------------------------------------------------------
//!    | S | Slave Address | Wr | A | Command Code | A | Byte Count = N | A | Data Byte 1 | A | ...
//!    ----------------------------------------------------------------------------------------
//!            8       1                 8       1   1
//!    --------------------      -----------------------
//!    | Data Byte 2 | A |  ...  | Data Byte N | A | P |
//!    --------------------      -----------------------
//! Block Write with PEC:
//!      1         7         1    1         8        1         8          1          8      1  
//!    ----------------------------------------------------------------------------------------
//!    | S | Slave Address | Wr | A | Command Code | A | Byte Count = N | A | Data Byte 1 | A | ...
//!    ----------------------------------------------------------------------------------------
//!            8       1                 8       1    8    1   1
//!    --------------------      ---------------------------------
//!    | Data Byte 2 | A |  ...  | Data Byte N | A | PEC | A | P |
//!    --------------------      ---------------------------------
//!
//! where: 
//!     S = Start bit
//!     Wr = Write bit (0)
//!     Slave Adddress = SMBus address for slave
//!     Command Code = Command byte sent to slave
//!     Data Byte = data sent to slave
//!     A = Acknowledge from slave
//!     PEC = Optional Packet Error Code
//!     P = Stop bit
//!~~~~~~~~
//
//! \param smbus     Pointer to SMBus structure
//! \param targetAddr Slave address
//! \param command    Command byte for slave
//! \param txData    TX data buffer
//! \param txSize    Size of the txData buffer
//
//! \return  \b SMBUS_RET_ERROR, or \b SMBUS_RET_OK
//
//*****************************************************************************
extern int8_t SMBus_masterWriteBlock(SMBus *smbus,
                                     uint8_t targetAddr,
                                     uint8_t command,
                                     uint8_t *txData,
                                     uint8_t txSize);

//*****************************************************************************
//
//! \brief   Send a command requesting a byte or word of data from the slave
//
//
//!~~~~~~~~
//! SMBus Read ByteWord command protocol
//!
//! Read Byte:
//!      1         7         1    1         8        1       8           1    1       8       1   1
//!    ----------------------------------------------------------------------------------------------
//!    | S | Slave Address | Wr | A | Command Code | S | Slave Address | Rd | A | Data Byte | A | P |
//!    ----------------------------------------------------------------------------------------------
//! Read Byte with PEC:
//!      1         7         1    1         8        1       8           1    1       8       1   1
//!    ----------------------------------------------------------------------------------------------
//!    | S | Slave Address | Wr | A | Command Code | S | Slave Address | Rd | A | Data Byte | A | P | ...
//!    ----------------------------------------------------------------------------------------------
//!     8    1   1
//!    ---------------
//!    | PEC | A | P |
//!    ---------------
//!
//! Read Word:
//!      1         7         1    1         8        1       8           1    1       8           1   1
//!    --------------------------------------------------------------------------------------------------
//!    | S | Slave Address | Wr | A | Command Code | S | Slave Address | Rd | A | Data Byte Low | A | P | ...
//!    --------------------------------------------------------------------------------------------------
//!             8         1   1
//!    --------------------------
//!    | Data Byte High | A | P |
//!    --------------------------
//! Read Word with PEC:
//!      1         7         1    1         8        1       8           1    1       8           1   1
//!    --------------------------------------------------------------------------------------------------
//!    | S | Slave Address | Wr | A | Command Code | S | Slave Address | Rd | A | Data Byte Low | A | P | ...
//!    --------------------------------------------------------------------------------------------------
//!             8         1   1    8    1   1
//!    ----------------------------------------
//!    | Data Byte High | A | P | PEC | A | P |
//!    ----------------------------------------
//!
//! where: 
//!     S = Start bit
//!     Wr = Read bit (0)
//!     Rd = Read bit (1)
//!     Slave Adddress = SMBus address for slave
//!     Command Code = Command byte sent to slave
//!     Data Byte = data sent to slave
//!     A = Acknowledge from slave
//!     PEC = Optional Packet Error Code
//!     P = Stop bit
//!~~~~~~~~
//
//! \param smbus     Pointer to SMBus structure
//! \param targetAddr Slave address
//! \param command    Command byte for slave
//! \param rxData    RX data buffer
//! \param rxSize    Must be 1 or 2 bytes
//
//! \return  \b SMBUS_RET_ERROR, or \b SMBUS_RET_OK
//
//*****************************************************************************
extern int8_t SMBus_masterReadByteWord(SMBus *smbus,
                                       uint8_t targetAddr,
                                       uint8_t command,
                                       uint8_t *rxData,
                                       uint8_t rxSize);

//*****************************************************************************
//
//! \brief   Send a command transmitting a byte or word of data from the slave
//!
//!~~~~~~~~
//! SMBus Write ByteWord command protocol
//!
//! Write Byte:
//!      1         7         1    1         8        1       8       1   1
//!    ---------------------------------------------------------------------
//!    | S | Slave Address | Wr | A | Command Code | A | Data Byte | A | P |
//!    ---------------------------------------------------------------------
//! Write Byte with PEC:
//!      1         7         1    1         8        1       8       1    8    1   1
//!    -------------------------------------------------------------------------------
//!    | S | Slave Address | Wr | A | Command Code | A | Data Byte | A | PEC | A | P |
//!    -------------------------------------------------------------------------------
//!
//! Write Word:
//!      1         7         1    1         8        1       8           1          8         1   1
//!    ----------------------------------------------------------------------------------------------
//!    | S | Slave Address | Wr | A | Command Code | A | Data Byte Low | A | Data Byte High | A | P |
//!    ----------------------------------------------------------------------------------------------
//! Write Word with PEC:
//!      1         7         1    1         8        1       8           1          8         1    8    1   1
//!    --------------------------------------------------------------------------------------------------------
//!    | S | Slave Address | Wr | A | Command Code | A | Data Byte Low | A | Data Byte High | A | PEC | A | P |
//!    --------------------------------------------------------------------------------------------------------
//!
//! where: 
//!     S = Start bit
//!     Wr = Write bit (0)
//!     Slave Adddress = SMBus address for slave
//!     Command Code = Command byte sent to slave
//!     Data Byte = data sent to slave
//!     A = Acknowledge from slave
//!     PEC = Optional Packet Error Code
//!     P = Stop bit
//!~~~~~~~~
//
//! \param smbus     Pointer to SMBus structure
//! \param targetAddr Slave address
//! \param command    Command byte for slave
//! \param txData    TX data buffer
//! \param txSize    Must be 1 or 2 bytes
//
//! \return  \b SMBUS_RET_ERROR, or \b SMBUS_RET_OK
//
//*****************************************************************************
extern int8_t SMBus_masterWriteByteWord(SMBus *smbus,
                                        uint8_t targetAddr,
                                        uint8_t command,
                                        uint8_t *txData,
                                        uint8_t txSize);

//*****************************************************************************
//
//! \brief   Send a SMBus "quick command"
//
//! A "quick command" is only a trigger. There is no data sent or received.
//!
//!~~~~~~~~
//! SMBus Quick Commmand Protocol:
//!      1         7           1     1   1
//!    -------------------------------------
//!    | S | Slave Address | Rd/Wr | A | P |
//!    -------------------------------------
//! where: 
//!     S = Start bit
//!     Rd/Wr = Read or Write bit
//!     Slave Adddress = SMBus address for slave
//!     A = Acknowledge from slave
//!     P = Stop bit
//!~~~~~~~~
//
//! \param smbus     Pointer to SMBus structure
//! \param targetAddr Slave address
//! \param write      true if this is a write command, false if this is a read command
//
//! \return  \b SMBUS_RET_ERROR, or \b SMBUS_RET_OK
//
//*****************************************************************************
extern int8_t SMBus_masterQuickCommand(SMBus *smbus,
                                       uint8_t targetAddr,
                                       bool write);

//*****************************************************************************
//
//! \brief   Wait until the previous SMBus command is executed
//
//! \param smbus      Pointer to SMBus structure
//! \param timeout    Software timeout
//
//! \return  \b SMBUS_RET_ERROR, or \b SMBUS_RET_OK
//
//*****************************************************************************
extern int8_t SMBus_masterWaitUntilDone(SMBus *smbus,
                                        int32_t timeout);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

#endif //__SMBUS_H__
