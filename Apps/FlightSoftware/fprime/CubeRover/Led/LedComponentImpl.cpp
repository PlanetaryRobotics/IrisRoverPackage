// ======================================================================
// \title  LedComponentImpl.cpp
// \author justin
// \brief  cpp file for Led component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/Led/LedComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include <gio.h>

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  LedComponentImpl ::
#if FW_OBJECT_NAMES == 1
    LedComponentImpl(
        const char *const compName
    ) :
      LedComponentBase(compName)
#else
    LedComponentImpl(void)
#endif
  {

  }

  void LedComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    LedComponentBase::init(instance);
    gio_config_reg_t gio_state;
    gioGetConfigValue(&gio_state, CurrentValue);
    uint32 new_dir = gio_state.CONFIG_PORTBDIR | (uint32)((uint32)1U << 2U);    // gioB[2] OUTPUT -> LedComponent
    gioSetDirection(gioPORTB, new_dir);
  }

  LedComponentImpl ::
    ~LedComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void LedComponentImpl ::
    Run_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
      gioSetBit(gioPORTB, 1, 0);    // Disable idle task Led
      gioSetBit(gioPORTB, 2, 1);
      for(uint32_t i=0; i<220000; i++) asm("  NOP");     // Poll just long enough to see the Led turn on
      gioSetBit(gioPORTB, 2, 0);
  }

} // end namespace CubeRover
