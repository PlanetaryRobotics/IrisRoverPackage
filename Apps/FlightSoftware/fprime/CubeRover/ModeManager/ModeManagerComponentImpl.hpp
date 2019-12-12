// ======================================================================
// \title  ModeManagerComponentImpl.hpp
// \author raewynduvall
// \brief  hpp file for ModeManager component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef ModeManager_HPP
#define ModeManager_HPP

#include "CubeRover/ModeManager/ModeManagerComponentAc.hpp"

namespace CubeRover {

  class ModeManagerComponentImpl :
    public ModeManagerComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object ModeManager
      //!
      ModeManagerComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object ModeManager
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object ModeManager
      //!
      ~ModeManagerComponentImpl(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for FatalReceive
      //!
      void FatalReceive_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwEventIdType Id /*!< The ID of the FATAL event*/
      );

      //! Handler implementation for pingIn
      //!
      void pingIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );

      //! Handler implementation for controlMode
      //!
      void controlMode_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          OpMode mode /*!< mode argument*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for GOTO_MODE command handler
      //! Goto a specifified mode
      void GOTO_MODE_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          OpModeCmd mode_cmd 
      );


    };

} // end namespace CubeRover

#endif
