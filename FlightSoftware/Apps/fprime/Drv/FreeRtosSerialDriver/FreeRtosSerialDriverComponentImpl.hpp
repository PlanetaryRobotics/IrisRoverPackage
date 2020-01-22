// ======================================================================
// \title  FreeRtosSerialDriverComponentImpl.hpp
// \author cedric
// \brief  hpp file for FreeRtosSerialDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FreeRtosSerialDriver_HPP
#define FreeRtosSerialDriver_HPP

#include <sci.h>
#include <Os/Mutex.hpp>

#include "Drv/FreeRtosSerialDriver/FreeRtosSerialDriverComponentAc.hpp"

namespace Drv {

  enum {
    DR_MAX_NUM_BUFFERS = 20,
  };

  class FreeRtosSerialDriverComponentImpl :
    public FreeRtosSerialDriverComponentBase
  {

    public:

      //! Configure UART parameters
      typedef enum BAUD_RATE {
          BAUD_9600,
          BAUD_19200,
          BAUD_38400,
          BAUD_57600,
          BAUD_115K,
          BAUD_230K,
          BAUD_460K,
          BAUD_921K
      } UartBaudRate ;

      typedef enum FLOW_CONTROL {
          NO_FLOW,
          HW_FLOW
      } UartFlowControl;

      typedef enum PARITY  {
          PARITY_NONE,
          PARITY_ODD,
          PARITY_EVEN
      } UartParity;

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object FreeRtosSerialDriver
      //!
      FreeRtosSerialDriverComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object FreeRtosSerialDriver
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

// Open device with specified baud and flow control.
      bool open(sciBASE_t *sci, 
                UartBaudRate baud, 
                UartFlowControl fc, 
                UartParity parity, 
                bool block);

      //! start the serial poll thread.
      //! buffSize is the max receive buffer size
      //!
      void startReadThread(NATIVE_INT_TYPE priority,
                           NATIVE_INT_TYPE stackSize,
                           NATIVE_INT_TYPE cpuAffinity = -1);

      //! Quit thread
      void quitReadThread(void);

      //! Destroy object FreeRtosSerialDriver
      //!
      ~FreeRtosSerialDriverComponentImpl(void);

    PRIVATE:
      
      sciBASE_t * m_sci;

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for readBufferSend
      //!
      void readBufferSend_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );

      //! Handler implementation for serialSend
      //!
      void serialSend_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &serBuffer 
      );

      //! This method will be called by the new thread to wait for input on the serial port.
      static void serialReadTaskEntry(void * ptr);

      Os::Task m_readTask; //!< task instance for thread to read serial port

      struct BufferSet {
          Fw::Buffer readBuffer; //!< buffers for port reads
          bool available; //!< is buffer available?
      } m_buffSet[DR_MAX_NUM_BUFFERS];

      Os::Mutex m_readBuffMutex;

      bool m_quitReadThread; //!< flag to quit thread

    };

} // end namespace Drv

#endif
