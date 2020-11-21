// ======================================================================
// \title  IMUComponentImpl.cpp
// \author michael
// \brief  cpp file for IMU component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/IMU/IMUComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  IMUComponentImpl ::
#if FW_OBJECT_NAMES == 1
    IMUComponentImpl(
        const char *const compName
    ) :
      IMUComponentBase(compName)
#else
    IMUComponentImpl(void)
#endif
  {

  }

  void IMUComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    IMUComponentBase::init(instance);
  }

  IMUComponentImpl ::
    ~IMUComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void IMUComponentImpl ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    // TODO
  }

  void IMUComponentImpl ::
    PingIn_handler(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void IMUComponentImpl ::
    Imu_ReportData_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

} // end namespace CubeRover
