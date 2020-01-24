// ====================================================================== 
// \title  LinuxTimerComponentAc.hpp
// \author Auto-generated
// \brief  hpp file for LinuxTimer component base class
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

#ifndef LINUXTIMER_COMP_HPP_
#define LINUXTIMER_COMP_HPP_

#include <Fw/Cfg/Config.hpp>
#include <Fw/Port/InputSerializePort.hpp>
#include <Fw/Port/OutputSerializePort.hpp>
#include <Fw/Comp/ActiveComponentBase.hpp>
#include <Svc/Cycle/TimerVal.hpp>
#include <Svc/Cycle/CyclePortAc.hpp>

namespace Svc {

  //! \class LinuxTimerComponentBase
  //! \brief Auto-generated base for LinuxTimer component
  //!
  class LinuxTimerComponentBase :
    public Fw::PassiveComponentBase
  {

    // ----------------------------------------------------------------------
    // Friend classes
    // ----------------------------------------------------------------------

    //! Friend class for white-box testing
    //!
    friend class LinuxTimerComponentBaseFriend;

  public:

    // ----------------------------------------------------------------------
    // Connect typed input ports to typed output ports
    // ----------------------------------------------------------------------

    //! Connect port to CycleOut[portNum]
    //!
    void set_CycleOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Svc::InputCyclePort *port /*!< The port*/
    );

#if FW_PORT_SERIALIZATION

  public:

    // ----------------------------------------------------------------------
    // Connect serialization input ports to typed output ports
    // ----------------------------------------------------------------------

    //! Connect port to CycleOut[portNum]
    //!
    void set_CycleOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

#endif

  PROTECTED:

    // ----------------------------------------------------------------------
    // Component construction, initialization, and destruction
    // ----------------------------------------------------------------------

#if FW_OBJECT_NAMES == 1
    //! Construct a LinuxTimerComponentBase object
    //!
    LinuxTimerComponentBase(
        const char* compName /*!< Component name*/
    );
#else
    //! Construct a LinuxTimerComponentBase object
    //!
    LinuxTimerComponentBase(void);
#endif

    //! Initialize a LinuxTimerComponentBase object
    //!
    void init(
        NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    //! Destroy a LinuxTimerComponentBase object
    //!
    virtual ~LinuxTimerComponentBase(void);

  PROTECTED:

    // ----------------------------------------------------------------------
    // Invocation functions for typed output ports
    // ----------------------------------------------------------------------

    //! Invoke output port CycleOut
    //!
    void CycleOut_out(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Svc::TimerVal &cycleStart /*!< Cycle start timer value*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Getters for numbers of output ports
    // ----------------------------------------------------------------------

    //! Get the number of CycleOut output ports
    //!
    //! \return The number of CycleOut output ports
    //!
    NATIVE_INT_TYPE getNum_CycleOut_OutputPorts(void);

  
    enum {
       NUM_CYCLEOUT_OUTPUT_PORTS = 1,
    };
  
  PROTECTED:

    // ----------------------------------------------------------------------
    // Connection status queries for output ports
    // ----------------------------------------------------------------------

    //! Check whether port CycleOut is connected
    //!
    //! \return Whether port CycleOut is connected
    //!
    bool isConnected_CycleOut_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Event logging functions
    // ----------------------------------------------------------------------

  PRIVATE:

    // ----------------------------------------------------------------------
    // Typed output ports
    // ----------------------------------------------------------------------

    //! Output port CycleOut
    //!
    Svc::OutputCyclePort m_CycleOut_OutputPort[NUM_CYCLEOUT_OUTPUT_PORTS];



  };  

} // end namespace Svc
#endif
