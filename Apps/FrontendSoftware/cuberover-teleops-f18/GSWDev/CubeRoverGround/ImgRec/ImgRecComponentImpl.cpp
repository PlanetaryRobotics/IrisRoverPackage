// ====================================================================== 
// \title  ImgRecImpl.cpp
// \author sofiahurtado
// \brief  cpp file for ImgRec component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 


#include <CubeRoverGround/ImgRec/ImgRecComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace CubeRoverGround {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  ImgRecComponentImpl ::
#if FW_OBJECT_NAMES == 1
    ImgRecComponentImpl(
        const char *const compName
    ) :
      ImgRecComponentBase(compName)
#else
    ImgRecImpl(void)
#endif
  {

  }

  void ImgRecComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    ) 
  {
    ImgRecComponentBase::init(queueDepth, instance);
  }

  ImgRecComponentImpl ::
    ~ImgRecComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void ImgRecComponentImpl ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Command handler implementations 
  // ----------------------------------------------------------------------

  void ImgRecComponentImpl ::
    SignalGen_Settings_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U32 Frequency,
        U32 Amplitude,
        U32 Phase
    )
  {
    // TODO
  }

  void ImgRecComponentImpl ::
    SignalGen_Toggle_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
  }

  void ImgRecComponentImpl ::
    SignalGen_Skip_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
  }

} // end namespace CubeRoverGround
