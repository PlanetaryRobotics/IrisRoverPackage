// ======================================================================
// \title  UWBComponentImpl.cpp
// \author alec
// \brief  cpp file for UWB component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/UWB/UWB.hpp>
#include "Fw/Types/BasicTypes.hpp"

#include <spi.h>
#include "App/SPI.h"

/* SPI configuration */
static spiBASE_t *spi = spiREG5;
static spiDAT1_t dataConfig = {
    .CS_HOLD = 0,
    .WDEL = 0,
    .DFSEL = SPI_FMT_0,
    .CSNR = 0b11111110   // Each index corresponds to CS[i]. Value represents the CS level when a transaction is occurring (1: hign, 0: low). SPIDEF sets the non-transaction CS level.
};

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  UWBComponentImpl ::
#if FW_OBJECT_NAMES == 1
    UWBComponentImpl(
        const char *const compName
    ) :
      UWBComponentBase(compName)
#else
    UWBComponentImpl(void)
#endif
  {

  }

  void UWBComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    UWBComponentBase::init(queueDepth, instance);
    m_callbackId = 0;
    m_bytesSent = 0;
  }

  UWBComponentImpl ::
    ~UWBComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void UWBComponentImpl ::
    PingIn_handler(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    this->PingOut_out(0, key);
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void UWBComponentImpl ::
    Get_Time_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    char times[4*4]; /*4, 4 byte sized U32's for time stamps*/
    Fw::Buffer fwBuffer(0, 0, reinterpret_cast<U64>(&times), sizeof(times));

    // Transmit function
    // One Byte: uint32 spiTransmitOneByte(spi, dataConfig, uint16* srcbuff)
    // Multibyte: uint32 spiTransmitData(spi, dataConfig, uint32 blocksize, uint16 * srcbuff);

    // Receive function
    // One Byte: uint32 spiReceiveOneByte(spi, dataConfig, uint16 *destbuff)
    // Multibyte: uint32 spiReceiveData(spi, dataConfig, 16, static_cast<uint16_t*>(&times));


    uint32_t createTime = static_cast<uint32_t>(getTime().get_time_ms());
    UWBSend_out(0, m_callbackId, createTime, fwBuffer);
    m_bytesSent += static_cast<U32>(sizeof(times));
    tlmWrite_UWBBytesSent(m_bytesSent);
    m_callbackId++;
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void UWBComponentImpl ::
    Get_Data_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    char times[4000+4*4]; /*4, 4 byte sized U32's for time stamps and 4k bytes of data*/
    Fw::Buffer fwBuffer(0, 0, reinterpret_cast<U64>(&times), sizeof(times));
    uint32_t createTime = static_cast<uint32_t>(getTime().get_time_ms());
    UWBSend_out(0, m_callbackId, createTime, fwBuffer);
    m_bytesSent += static_cast<U32>(sizeof(times));
    tlmWrite_UWBBytesSent(m_bytesSent);
    m_callbackId++;
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

} // end namespace CubeRover
