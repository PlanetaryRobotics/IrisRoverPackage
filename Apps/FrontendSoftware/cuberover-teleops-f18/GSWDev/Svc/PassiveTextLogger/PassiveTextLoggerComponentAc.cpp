// ====================================================================== 
// \title  PassiveTextLoggerComponentAc.hpp
// \author Auto-generated
// \brief  cpp file for PassiveTextLogger component base class
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
#include <Svc/PassiveTextLogger/PassiveTextLoggerComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#if FW_ENABLE_TEXT_LOGGING
#include <Fw/Types/EightyCharString.hpp>
#endif


namespace Svc {

  // ----------------------------------------------------------------------
  // Getters for numbers of input ports
  // ----------------------------------------------------------------------

  Fw::InputLogTextPort *PassiveTextLoggerComponentBase ::
    get_TextLogger_InputPort(NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_TextLogger_InputPorts(),static_cast<AssertArg>(portNum));
    return &this->m_TextLogger_InputPort[portNum];
  }

  // ----------------------------------------------------------------------
  // Component construction, initialization, and destruction
  // ----------------------------------------------------------------------

  PassiveTextLoggerComponentBase ::
#if FW_OBJECT_NAMES == 1
    PassiveTextLoggerComponentBase(const char* compName) :
      Fw::PassiveComponentBase(compName)
#else
    PassiveTextLoggerComponentBase() :
      Fw::PassiveComponentBase()
#endif
  {


  
  
  }

  void PassiveTextLoggerComponentBase ::
    init(NATIVE_INT_TYPE instance)
  {

    // Initialize base class
    Fw::PassiveComponentBase::init(instance);

    // Connect input port TextLogger
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_TextLogger_InputPorts();
        port++
    ) {

      this->m_TextLogger_InputPort[port].init();
      this->m_TextLogger_InputPort[port].addCallComp(
          this,
          m_p_TextLogger_in
      );
      this->m_TextLogger_InputPort[port].setPortNum(port);

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_TextLogger_InputPort[%d]",
          this->m_objName,
          port
      );
      this->m_TextLogger_InputPort[port].setObjName(portName);
#endif

    }


  }

  PassiveTextLoggerComponentBase::
    ~PassiveTextLoggerComponentBase() {

  }

  // ----------------------------------------------------------------------
  // Invocation functions for output ports
  // ----------------------------------------------------------------------

  // ----------------------------------------------------------------------
  // Getters for numbers of ports
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE PassiveTextLoggerComponentBase ::
    getNum_TextLogger_InputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_TextLogger_InputPort
    );
  }

  // ----------------------------------------------------------------------
  // Calls for invocations received on typed input ports
  // ----------------------------------------------------------------------

  void PassiveTextLoggerComponentBase ::
    m_p_TextLogger_in(
        Fw::PassiveComponentBase* callComp,
        NATIVE_INT_TYPE portNum,
        FwEventIdType id, Fw::Time &timeTag, Fw::TextLogSeverity severity, Fw::TextLogString &text
    )
  {
    FW_ASSERT(callComp);
    PassiveTextLoggerComponentBase* compPtr =
      (PassiveTextLoggerComponentBase*) callComp;
    compPtr->TextLogger_handlerBase(portNum, id, timeTag, severity, text);
  }

  // ----------------------------------------------------------------------
  // Port handler base-class functions for typed input ports
  // ----------------------------------------------------------------------

  void PassiveTextLoggerComponentBase ::
    TextLogger_handlerBase(
        NATIVE_INT_TYPE portNum,
        FwEventIdType id, Fw::Time &timeTag, Fw::TextLogSeverity severity, Fw::TextLogString &text
    )
  {

    // Make sure port number is valid
    FW_ASSERT(portNum < this->getNum_TextLogger_InputPorts(),static_cast<AssertArg>(portNum));

    // Down call to pure virtual handler method implemented in Impl class
    this->TextLogger_handler(portNum, id, timeTag, severity, text);

  }

} // end namespace Svc
