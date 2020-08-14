// ====================================================================== 
// \title  RateGroupDriverComponentAc.hpp
// \author Auto-generated
// \brief  cpp file for RateGroupDriver component base class
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
#include <Svc/RateGroupDriver/RateGroupDriverComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#if FW_ENABLE_TEXT_LOGGING
#include <Fw/Types/EightyCharString.hpp>
#endif


namespace Svc {

  // ----------------------------------------------------------------------
  // Getters for numbers of input ports
  // ----------------------------------------------------------------------

  Svc::InputCyclePort *RateGroupDriverComponentBase ::
    get_CycleIn_InputPort(NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_CycleIn_InputPorts(),static_cast<AssertArg>(portNum));
    return &this->m_CycleIn_InputPort[portNum];
  }

  // ----------------------------------------------------------------------
  // Typed connectors for output ports
  // ----------------------------------------------------------------------

  void RateGroupDriverComponentBase ::
    set_CycleOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Svc::InputCyclePort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_CycleOut_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_CycleOut_OutputPort[portNum].addCallPort(port);
  }

  // ----------------------------------------------------------------------
  // Serialization connectors for output ports
  // ----------------------------------------------------------------------

#if FW_PORT_SERIALIZATION

  void RateGroupDriverComponentBase ::
    set_CycleOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_CycleOut_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_CycleOut_OutputPort[portNum].registerSerialPort(port);
  }

#endif

  // ----------------------------------------------------------------------
  // Component construction, initialization, and destruction
  // ----------------------------------------------------------------------

  RateGroupDriverComponentBase ::
#if FW_OBJECT_NAMES == 1
    RateGroupDriverComponentBase(const char* compName) :
      Fw::PassiveComponentBase(compName)
#else
    RateGroupDriverComponentBase() :
      Fw::PassiveComponentBase()
#endif
  {


  
  
  }

  void RateGroupDriverComponentBase ::
    init(NATIVE_INT_TYPE instance)
  {

    // Initialize base class
    Fw::PassiveComponentBase::init(instance);

    // Connect input port CycleIn
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_CycleIn_InputPorts();
        port++
    ) {

      this->m_CycleIn_InputPort[port].init();
      this->m_CycleIn_InputPort[port].addCallComp(
          this,
          m_p_CycleIn_in
      );
      this->m_CycleIn_InputPort[port].setPortNum(port);

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_CycleIn_InputPort[%d]",
          this->m_objName,
          port
      );
      this->m_CycleIn_InputPort[port].setObjName(portName);
#endif

    }

    // Initialize output port CycleOut
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_CycleOut_OutputPorts();
        port++
    ) {
      this->m_CycleOut_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_CycleOut_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_CycleOut_OutputPort[port].setObjName(portName);
#endif

    }


  }

  RateGroupDriverComponentBase::
    ~RateGroupDriverComponentBase() {

  }

  // ----------------------------------------------------------------------
  // Invocation functions for output ports
  // ----------------------------------------------------------------------

  void RateGroupDriverComponentBase ::
    CycleOut_out(
        NATIVE_INT_TYPE portNum,
        Svc::TimerVal &cycleStart
    )
  {
    FW_ASSERT(portNum < this->getNum_CycleOut_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_CycleOut_OutputPort[portNum].invoke(cycleStart);
  }

  // ----------------------------------------------------------------------
  // Getters for numbers of ports
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE RateGroupDriverComponentBase ::
    getNum_CycleIn_InputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_CycleIn_InputPort
    );
  }

  NATIVE_INT_TYPE RateGroupDriverComponentBase ::
    getNum_CycleOut_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_CycleOut_OutputPort
    );
  }

  // ----------------------------------------------------------------------
  // Port connection status queries
  // ----------------------------------------------------------------------

  bool RateGroupDriverComponentBase ::
    isConnected_CycleOut_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_CycleOut_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_CycleOut_OutputPort[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Calls for invocations received on typed input ports
  // ----------------------------------------------------------------------

  void RateGroupDriverComponentBase ::
    m_p_CycleIn_in(
        Fw::PassiveComponentBase* callComp,
        NATIVE_INT_TYPE portNum,
        Svc::TimerVal &cycleStart
    )
  {
    FW_ASSERT(callComp);
    RateGroupDriverComponentBase* compPtr =
      (RateGroupDriverComponentBase*) callComp;
    compPtr->CycleIn_handlerBase(portNum, cycleStart);
  }

  // ----------------------------------------------------------------------
  // Port handler base-class functions for typed input ports
  // ----------------------------------------------------------------------

  void RateGroupDriverComponentBase ::
    CycleIn_handlerBase(
        NATIVE_INT_TYPE portNum,
        Svc::TimerVal &cycleStart
    )
  {

    // Make sure port number is valid
    FW_ASSERT(portNum < this->getNum_CycleIn_InputPorts(),static_cast<AssertArg>(portNum));

    // Down call to pure virtual handler method implemented in Impl class
    this->CycleIn_handler(portNum, cycleStart);

  }

} // end namespace Svc
