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
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/EightyCharString.hpp>


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
    
    sciInit();

    sciEnterResetState(m_sci);
    sciSetBaudrate(m_sci, baud);

    sciEnableNotification(m_sci, SCI_RX_INT);

    sciExitResetState(m_sci);

    return true;
  }

  FreeRtosSerialDriverComponentImpl ::
    ~FreeRtosSerialDriverComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------
 //! This method will be called by the new thread to wait for input on the serial port.
  void FreeRtosSerialDriverComponentImpl :: serialReadTaskEntry(void * ptr){}
    Drv::SerialReadStatus serReadStat;
    FreeRtosSerialDriverComponentImpl* comp = static_cast<FreeRtosSerialDriverComponentImpl*>(ptr);
    Fw::Buffer buff;
    uint32_t tries = 10;

    while (1) {
        // wait for data
        int sizeRead = 0;

        // find open buffer
        comp->m_readBuffMutex.lock();

        // search for open entry
        NATIVE_INT_TYPE entryFound = false;
        for(NATIVE_INT_TYPE entry = 0; entry < DR_MAX_NUM_BUFFERS; entry++){
            if (comp->m_buffSet[entry].available) {
                comp->m_buffSet[entry].available = false;
                buff = comp->m_buffSet[entry].readBuffer;
                entryFound = true;
                break;
            }
        }

        comp->m_readBuffMutex.unLock();

        if (not entryFound) {
            Fw::LogStringArg _arg = comp->m_device;
            comp->log_WARNING_HI_DR_NoBuffers(_arg);
            serReadStat = Drv::SER_NO_BUFFERS;
            comp->serialRecv_out(0,buff,serReadStat);
            // to avoid spinning, wait 50 ms
            Os::Task::delay(50);
            continue;
        }

        bool waiting = true;
        int stat = 0;

        while (waiting){
          if(comp->m_quitReadThread) {
            return;
          }

          // Block here if a rx is already underway
          while(--tries && !sciIsRxReady(m_sci));

          //Blocking until timeout or data available
          stat = sciReceive(m_sci, 
                           buff.getsize(),
                           reinterpret_cast<unsigned char*>(buff.getdata()));

          // Good read:
          if (stat > 0) {
            sizeRead = stat;
          }

          // check for timeout
          if (stat == 0) {
            if (comp->m_quitReadThread) {
              return;
            }
          } else { // quit if other error or data
              waiting = false;
            }
        } // end of while waiting

        if (comp->m_quitReadThread) {
            return;
        }

        // check stat, maybe output event
        if (stat == -1) {
            Fw::LogStringArg _arg = comp->m_device;
            comp->log_WARNING_HI_DR_ReadError(_arg,stat);
            serReadStat = Drv::SER_OTHER_ERR;
        } else {
            buff.setsize(sizeRead);
            serReadStat = Drv::SER_OK;
        }
        comp->serialRecv_out(0,buff,serReadStat);
      }
  }

  void FreeRtosSerialDriverComponentImpl :: serialSend_handler(const NATIVE_INT_TYPE portNum,
                                                                Fw::Buffer &serBuffer){
    uint32_t tries = 10;
    unsigned char* data = reinterpret_cast<unsigned char*>(serBuffer.getdata());
    NATIVE_INT_TYPE xferSize = serBuffer.getsize();

    // Block here if a tx is already underway
    while(--tries && !sciIsTxReady(m_sci));

    if(!tries){
      Fw::LogStringArg _arg = "FreeRtosSerialDriver";
      this->log_WARNING_HI_DR_WriteError(_arg, -1);
      return;
    } 

    //Send data using interrupt, do not call sciSend again until data is fully sent
    sciSend(m_sci, data, xferSize);

  }

} // end namespace Drv
