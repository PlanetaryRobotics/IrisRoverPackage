// ====================================================================== 
// \title  RateCounterImpl.hpp
// \author sofiahurtado
// \brief  hpp file for RateCounter component implementation class
//
// ====================================================================== 

#ifndef RateCounter_HPP
#define RateCounter_HPP

#include "CmuApp/RateCounter/RateCounterComponentAc.hpp"

namespace CmuApp {

  class RateCounterComponentImpl :
    public RateCounterComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object RateCounter
      //!
      RateCounterComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object RateCounter
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object RateCounter
      //!
      ~RateCounterComponentImpl(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for schedIn
      //!
      void schedIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations 
      // ----------------------------------------------------------------------

      //! Implementation for RateCounter_report command handler
      //! Reports current count from the EVR
      void RateCounter_report_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      U32 m_count;


    };

} // end namespace CmuApp

#endif
