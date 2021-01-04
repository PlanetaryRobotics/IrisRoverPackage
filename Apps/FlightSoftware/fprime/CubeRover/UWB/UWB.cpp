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


#include <CubeRover/UWB/UWBComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

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
    UWB_cycles_rate = 10;
    UWB_cycles = 10;
  }

  UWBComponentImpl ::
    ~UWBComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void UWBComponentImpl ::
    Run_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    // Check if UWB_cycles is zero
    if(UWB_cycles > 0)
    {
      UWB_cycles--;
      return;
    }
    else
    {
      // Reset UWB_cycles to set cycles rate
      UWB_cycles = UWB_cycles_rate;
      // TODO
    }
  }

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
    Change_Rate_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U8 cycles
    )
  {
    UWB_cycles_rate = cycles;
    UWB_cycles = cycles;
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

} // end namespace CubeRover
