// ======================================================================
// \title  CameraInterfaceComponentImpl.cpp
// \author cedric
// \brief  cpp file for CameraInterface component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/CameraInterface/CameraInterfaceComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  CameraInterfaceComponentImpl ::
#if FW_OBJECT_NAMES == 1
    CameraInterfaceComponentImpl(
        const char *const compName
    ) :
      CameraInterfaceComponentBase(compName)
#else
    CameraInterfaceComponentImpl(void)
#endif
  {

  }

  void CameraInterfaceComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    CameraInterfaceComponentBase::init(queueDepth, instance);
  }

  CameraInterfaceComponentImpl ::
    ~CameraInterfaceComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void CameraInterfaceComponentImpl ::
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

  void CameraInterfaceComponentImpl ::
    CAM_CaptureImage_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void CameraInterfaceComponentImpl ::
    CAM_FetchImage_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

} // end namespace CubeRover
