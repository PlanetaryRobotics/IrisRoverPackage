// ======================================================================
// \title  FreeRtosSerialDriverComponentImpl.cpp
// \author cedric
// \brief  cpp file for FreeRtosSerialDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Drv/FreeRtosSerialDriver/FreeRtosSerialDriverComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Drv {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  FreeRtosSerialDriverComponentImpl ::
#if FW_OBJECT_NAMES == 1
    FreeRtosSerialDriverComponentImpl(
        const char *const compName
    ) :
      FreeRtosSerialDriverComponentBase(compName)
#else
    FreeRtosSerialDriverComponentImpl(void)
#endif
  {

  }

  void FreeRtosSerialDriverComponentImpl :: init(
        const NATIVE_INT_TYPE instance
    )
  {
    FreeRtosSerialDriverComponentBase::init(instance);
  }

  bool FreeRtosSerialDriverComponentImpl :: open( sciBASE_t *sci, 
                                                  UartBaudRate baud, 
                                                  UartFlowControl fc, 
                                                  UartParity parity, 
                                                  bool block){
    if(sci == NULL) return false;

    m_sci = sci;
    
    muxInit();
    sciInit();

    sciEnterResetState(m_sci);
    sciSetBaudrate(m_sci, baud);

    sciEnableNotification(m_sci, SCI_RX_INT);

    sciExitResetState(m_sci);
  }

  FreeRtosSerialDriverComponentImpl ::
    ~FreeRtosSerialDriverComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void FreeRtosSerialDriverComponentImpl ::
    readBufferSend_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    // TODO
  }

  void FreeRtosSerialDriverComponentImpl ::
    serialSend_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &serBuffer
    )
  {
    uint32_t tries = 10;
    unsigned char* data = reinterpret_cast<unsigned char*>(serBuffer.getdata());
    NATIVE_INT_TYPE xferSize = serBuffer.getsize();

    // Block here if a tx is already underway
    while(--tries && !sciIsTxReady(sciREG));

    if(!tries){
      Fw::LogStringArg _arg = "FreeRtosSerialDriver";
      this->log_WARNING_HI_DR_WriteError(_arg, -1);
      return;
    } 

    //Send data using interrupt, do not call sciSend again until data is fully sent
    sciSend(m_sci, data, xferSize);

  }

} // end namespace Drv
