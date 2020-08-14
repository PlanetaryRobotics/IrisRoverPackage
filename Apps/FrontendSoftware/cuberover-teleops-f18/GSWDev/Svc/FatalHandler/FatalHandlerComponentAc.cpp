// ====================================================================== 
// \title  FatalHandlerComponentAc.hpp
// \author Auto-generated
// \brief  cpp file for FatalHandler component base class
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
#include <Svc/FatalHandler/FatalHandlerComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#if FW_ENABLE_TEXT_LOGGING
#include <Fw/Types/EightyCharString.hpp>
#endif


namespace Svc {

  // ----------------------------------------------------------------------
  // Getters for numbers of input ports
  // ----------------------------------------------------------------------

  Svc::InputFatalEventPort *FatalHandlerComponentBase ::
    get_FatalReceive_InputPort(NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_FatalReceive_InputPorts(),static_cast<AssertArg>(portNum));
    return &this->m_FatalReceive_InputPort[portNum];
  }

  // ----------------------------------------------------------------------
  // Component construction, initialization, and destruction
  // ----------------------------------------------------------------------

  FatalHandlerComponentBase ::
#if FW_OBJECT_NAMES == 1
    FatalHandlerComponentBase(const char* compName) :
      Fw::PassiveComponentBase(compName)
#else
    FatalHandlerComponentBase() :
      Fw::PassiveComponentBase()
#endif
  {


  
  
  }

  void FatalHandlerComponentBase ::
    init(NATIVE_INT_TYPE instance)
  {

    // Initialize base class
    Fw::PassiveComponentBase::init(instance);

    // Connect input port FatalReceive
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_FatalReceive_InputPorts();
        port++
    ) {

      this->m_FatalReceive_InputPort[port].init();
      this->m_FatalReceive_InputPort[port].addCallComp(
          this,
          m_p_FatalReceive_in
      );
      this->m_FatalReceive_InputPort[port].setPortNum(port);

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_FatalReceive_InputPort[%d]",
          this->m_objName,
          port
      );
      this->m_FatalReceive_InputPort[port].setObjName(portName);
#endif

    }


  }

  FatalHandlerComponentBase::
    ~FatalHandlerComponentBase() {

  }

  // ----------------------------------------------------------------------
  // Invocation functions for output ports
  // ----------------------------------------------------------------------

  // ----------------------------------------------------------------------
  // Getters for numbers of ports
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE FatalHandlerComponentBase ::
    getNum_FatalReceive_InputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_FatalReceive_InputPort
    );
  }

  // ----------------------------------------------------------------------
  // Calls for invocations received on typed input ports
  // ----------------------------------------------------------------------

  void FatalHandlerComponentBase ::
    m_p_FatalReceive_in(
        Fw::PassiveComponentBase* callComp,
        NATIVE_INT_TYPE portNum,
        FwEventIdType Id
    )
  {
    FW_ASSERT(callComp);
    FatalHandlerComponentBase* compPtr =
      (FatalHandlerComponentBase*) callComp;
    compPtr->FatalReceive_handlerBase(portNum, Id);
  }

  // ----------------------------------------------------------------------
  // Port handler base-class functions for typed input ports
  // ----------------------------------------------------------------------

  void FatalHandlerComponentBase ::
    FatalReceive_handlerBase(
        NATIVE_INT_TYPE portNum,
        FwEventIdType Id
    )
  {

    // Make sure port number is valid
    FW_ASSERT(portNum < this->getNum_FatalReceive_InputPorts(),static_cast<AssertArg>(portNum));

    // Down call to pure virtual handler method implemented in Impl class
    this->FatalReceive_handler(portNum, Id);

  }

} // end namespace Svc
