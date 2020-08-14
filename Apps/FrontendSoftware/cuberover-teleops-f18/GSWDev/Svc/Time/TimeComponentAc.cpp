// ====================================================================== 
// \title  TimeComponentAc.hpp
// \author Auto-generated
// \brief  cpp file for Time component base class
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

#include <stdio.h>
#include <Fw/Cfg/Config.hpp>
#include <Svc/Time/TimeComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#if FW_ENABLE_TEXT_LOGGING
#include <Fw/Types/EightyCharString.hpp>
#endif


namespace Svc {

  // ----------------------------------------------------------------------
  // Getters for numbers of input ports
  // ----------------------------------------------------------------------

  Fw::InputTimePort *TimeComponentBase ::
    get_timeGetPort_InputPort(NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_timeGetPort_InputPorts(),static_cast<AssertArg>(portNum));
    return &this->m_timeGetPort_InputPort[portNum];
  }

  // ----------------------------------------------------------------------
  // Component construction, initialization, and destruction
  // ----------------------------------------------------------------------

  TimeComponentBase ::
#if FW_OBJECT_NAMES == 1
    TimeComponentBase(const char* compName) :
      Fw::PassiveComponentBase(compName)
#else
    TimeComponentBase() :
      Fw::PassiveComponentBase()
#endif
  {


  
  
  }

  void TimeComponentBase ::
    init(NATIVE_INT_TYPE instance)
  {

    // Initialize base class
    Fw::PassiveComponentBase::init(instance);

    // Connect input port timeGetPort
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_timeGetPort_InputPorts();
        port++
    ) {

      this->m_timeGetPort_InputPort[port].init();
      this->m_timeGetPort_InputPort[port].addCallComp(
          this,
          m_p_timeGetPort_in
      );
      this->m_timeGetPort_InputPort[port].setPortNum(port);

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_timeGetPort_InputPort[%d]",
          this->m_objName,
          port
      );
      this->m_timeGetPort_InputPort[port].setObjName(portName);
#endif

    }


  }

  TimeComponentBase::
    ~TimeComponentBase() {

  }

  // ----------------------------------------------------------------------
  // Invocation functions for output ports
  // ----------------------------------------------------------------------

  // ----------------------------------------------------------------------
  // Getters for numbers of ports
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE TimeComponentBase ::
    getNum_timeGetPort_InputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_timeGetPort_InputPort
    );
  }

  // ----------------------------------------------------------------------
  // Calls for invocations received on typed input ports
  // ----------------------------------------------------------------------

  void TimeComponentBase ::
    m_p_timeGetPort_in(
        Fw::PassiveComponentBase* callComp,
        NATIVE_INT_TYPE portNum,
        Fw::Time &time
    )
  {
    FW_ASSERT(callComp);
    TimeComponentBase* compPtr =
      (TimeComponentBase*) callComp;
    compPtr->timeGetPort_handlerBase(portNum, time);
  }

  // ----------------------------------------------------------------------
  // Port handler base-class functions for typed input ports
  // ----------------------------------------------------------------------

  void TimeComponentBase ::
    timeGetPort_handlerBase(
        NATIVE_INT_TYPE portNum,
        Fw::Time &time
    )
  {

    // Make sure port number is valid
    FW_ASSERT(portNum < this->getNum_timeGetPort_InputPorts(),static_cast<AssertArg>(portNum));

    // Down call to pure virtual handler method implemented in Impl class
    this->timeGetPort_handler(portNum, time);

  }

} // end namespace Svc
