// ====================================================================== 
// \title  TlmChanComponentAc.hpp
// \author Auto-generated
// \brief  hpp file for TlmChan component base class
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

#ifndef TLMCHAN_COMP_HPP_
#define TLMCHAN_COMP_HPP_

#include <Fw/Cfg/Config.hpp>
#include <Fw/Port/InputSerializePort.hpp>
#include <Fw/Port/OutputSerializePort.hpp>
#include <Fw/Comp/ActiveComponentBase.hpp>
#include <Os/Mutex.hpp>
#include <Fw/Tlm/TlmBuffer.hpp>
#include <Fw/Time/Time.hpp>
#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Tlm/TlmPortAc.hpp>
#include <Fw/Com/ComPortAc.hpp>
#include <Svc/Sched/SchedPortAc.hpp>
#include <Svc/Ping/PingPortAc.hpp>

namespace Svc {

  //! \class TlmChanComponentBase
  //! \brief Auto-generated base for TlmChan component
  //!
  class TlmChanComponentBase :
    public Fw::ActiveComponentBase
  {

    // ----------------------------------------------------------------------
    // Friend classes
    // ----------------------------------------------------------------------

    //! Friend class for white-box testing
    //!
    friend class TlmChanComponentBaseFriend;

  public:

    // ----------------------------------------------------------------------
    // Getters for typed input ports
    // ----------------------------------------------------------------------

    //! Get input port at index
    //!
    //! \return TlmRecv[portNum]
    //!
    Fw::InputTlmPort* get_TlmRecv_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Get input port at index
    //!
    //! \return TlmGet[portNum]
    //!
    Fw::InputTlmPort* get_TlmGet_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Get input port at index
    //!
    //! \return Run[portNum]
    //!
    Svc::InputSchedPort* get_Run_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Get input port at index
    //!
    //! \return pingIn[portNum]
    //!
    Svc::InputPingPort* get_pingIn_InputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

  public:

    // ----------------------------------------------------------------------
    // Connect typed input ports to typed output ports
    // ----------------------------------------------------------------------

    //! Connect port to PktSend[portNum]
    //!
    void set_PktSend_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputComPort *port /*!< The port*/
    );

    //! Connect port to pingOut[portNum]
    //!
    void set_pingOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Svc::InputPingPort *port /*!< The port*/
    );

#if FW_PORT_SERIALIZATION

  public:

    // ----------------------------------------------------------------------
    // Connect serialization input ports to typed output ports
    // ----------------------------------------------------------------------

    //! Connect port to PktSend[portNum]
    //!
    void set_PktSend_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

    //! Connect port to pingOut[portNum]
    //!
    void set_pingOut_OutputPort(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::InputSerializePort *port /*!< The port*/
    );

#endif

  PROTECTED:

    // ----------------------------------------------------------------------
    // Component construction, initialization, and destruction
    // ----------------------------------------------------------------------

#if FW_OBJECT_NAMES == 1
    //! Construct a TlmChanComponentBase object
    //!
    TlmChanComponentBase(
        const char* compName /*!< Component name*/
    );
#else
    //! Construct a TlmChanComponentBase object
    //!
    TlmChanComponentBase(void);
#endif

    //! Initialize a TlmChanComponentBase object
    //!
    void init(
        NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
        NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    //! Destroy a TlmChanComponentBase object
    //!
    virtual ~TlmChanComponentBase(void);

  PROTECTED:

    // ----------------------------------------------------------------------
    //! Mutex operations for guarded ports.
    // ----------------------------------------------------------------------
    //! You can override these operations to provide more sophisticated
    //! synchronization.
    // ----------------------------------------------------------------------

    //! Lock the guarded mutex
    //!
    virtual void lock(void);

    //! Unlock the guarded mutex
    //!
    virtual void unLock(void);

  PROTECTED:

    // ----------------------------------------------------------------------
    // Handlers to implement for typed input ports
    // ----------------------------------------------------------------------

    //! Handler for input port TlmRecv
    //
    virtual void TlmRecv_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwChanIdType id, /*!< Telemetry Channel ID*/
        Fw::Time &timeTag, /*!< Time Tag*/
        Fw::TlmBuffer &val /*!< Buffer containing serialized telemetry value*/
    ) = 0;

    //! Handler for input port TlmGet
    //
    virtual void TlmGet_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwChanIdType id, /*!< Telemetry Channel ID*/
        Fw::Time &timeTag, /*!< Time Tag*/
        Fw::TlmBuffer &val /*!< Buffer containing serialized telemetry value*/
    ) = 0;

    //! Handler for input port Run
    //
    virtual void Run_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        NATIVE_UINT_TYPE context /*!< The call order*/
    ) = 0;

    //! Handler for input port pingIn
    //
    virtual void pingIn_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 key /*!< Value to return to pinger*/
    ) = 0;

  PROTECTED:

    // ----------------------------------------------------------------------
    // Port handler base-class functions for typed input ports.
    // ----------------------------------------------------------------------
    // Call these functions directly to bypass the corresponding ports.
    // ----------------------------------------------------------------------

    //! Handler base-class function for input port TlmRecv
    //!
    void TlmRecv_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwChanIdType id, /*!< Telemetry Channel ID*/
        Fw::Time &timeTag, /*!< Time Tag*/
        Fw::TlmBuffer &val /*!< Buffer containing serialized telemetry value*/
    );

    //! Handler base-class function for input port TlmGet
    //!
    void TlmGet_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwChanIdType id, /*!< Telemetry Channel ID*/
        Fw::Time &timeTag, /*!< Time Tag*/
        Fw::TlmBuffer &val /*!< Buffer containing serialized telemetry value*/
    );

    //! Handler base-class function for input port Run
    //!
    void Run_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        NATIVE_UINT_TYPE context /*!< The call order*/
    );

    //! Handler base-class function for input port pingIn
    //!
    void pingIn_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 key /*!< Value to return to pinger*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Pre-message hooks for typed async input ports.
    // ----------------------------------------------------------------------
    // Each of these functions is invoked just before processing a message
    // on the corresponding port. By default they do nothing. You can
    // override them to provide specific pre-message behavior.
    // ----------------------------------------------------------------------

    //! Pre-message hook for async input port Run
    //!
    virtual void Run_preMsgHook(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        NATIVE_UINT_TYPE context /*!< The call order*/
    );

    //! Pre-message hook for async input port pingIn
    //!
    virtual void pingIn_preMsgHook(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 key /*!< Value to return to pinger*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Invocation functions for typed output ports
    // ----------------------------------------------------------------------

    //! Invoke output port PktSend
    //!
    void PktSend_out(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::ComBuffer &data, /*!< Buffer containing packet data*/
        U32 context /*!< Call context value; meaning chosen by user*/
    );

    //! Invoke output port pingOut
    //!
    void pingOut_out(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 key /*!< Value to return to pinger*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Getters for numbers of input ports
    // ----------------------------------------------------------------------

    //! Get the number of TlmRecv input ports
    //!
    //! \return The number of TlmRecv input ports
    //!
    NATIVE_INT_TYPE getNum_TlmRecv_InputPorts(void);

    //! Get the number of TlmGet input ports
    //!
    //! \return The number of TlmGet input ports
    //!
    NATIVE_INT_TYPE getNum_TlmGet_InputPorts(void);

    //! Get the number of Run input ports
    //!
    //! \return The number of Run input ports
    //!
    NATIVE_INT_TYPE getNum_Run_InputPorts(void);

    //! Get the number of pingIn input ports
    //!
    //! \return The number of pingIn input ports
    //!
    NATIVE_INT_TYPE getNum_pingIn_InputPorts(void);

  
    // ----------------------------------------------------------------------
    // Enumerations for number of ports
    // ----------------------------------------------------------------------
    
    enum {
       NUM_TLMRECV_INPUT_PORTS = 1,
       NUM_TLMGET_INPUT_PORTS = 1,
       NUM_RUN_INPUT_PORTS = 1,
       NUM_PINGIN_INPUT_PORTS = 1,
    };
  
  PROTECTED:

    // ----------------------------------------------------------------------
    // Getters for numbers of output ports
    // ----------------------------------------------------------------------

    //! Get the number of PktSend output ports
    //!
    //! \return The number of PktSend output ports
    //!
    NATIVE_INT_TYPE getNum_PktSend_OutputPorts(void);

    //! Get the number of pingOut output ports
    //!
    //! \return The number of pingOut output ports
    //!
    NATIVE_INT_TYPE getNum_pingOut_OutputPorts(void);

  
    enum {
       NUM_PKTSEND_OUTPUT_PORTS = 1,
       NUM_PINGOUT_OUTPUT_PORTS = 1,
    };
  
  PROTECTED:

    // ----------------------------------------------------------------------
    // Connection status queries for output ports
    // ----------------------------------------------------------------------

    //! Check whether port PktSend is connected
    //!
    //! \return Whether port PktSend is connected
    //!
    bool isConnected_PktSend_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Check whether port pingOut is connected
    //!
    //! \return Whether port pingOut is connected
    //!
    bool isConnected_pingOut_OutputPort(
        NATIVE_INT_TYPE portNum /*!< The port number*/
    );

  PROTECTED:

    // ----------------------------------------------------------------------
    // Event logging functions
    // ----------------------------------------------------------------------


  PRIVATE:

    // ----------------------------------------------------------------------
    // Typed input ports
    // ----------------------------------------------------------------------

    //! Input port TlmRecv
    //!
    Fw::InputTlmPort m_TlmRecv_InputPort[NUM_TLMRECV_INPUT_PORTS];

    //! Input port TlmGet
    //!
    Fw::InputTlmPort m_TlmGet_InputPort[NUM_TLMGET_INPUT_PORTS];

    //! Input port Run
    //!
    Svc::InputSchedPort m_Run_InputPort[NUM_RUN_INPUT_PORTS];

    //! Input port pingIn
    //!
    Svc::InputPingPort m_pingIn_InputPort[NUM_PINGIN_INPUT_PORTS];

  PRIVATE:

    // ----------------------------------------------------------------------
    // Typed output ports
    // ----------------------------------------------------------------------

    //! Output port PktSend
    //!
    Fw::OutputComPort m_PktSend_OutputPort[NUM_PKTSEND_OUTPUT_PORTS];

    //! Output port pingOut
    //!
    Svc::OutputPingPort m_pingOut_OutputPort[NUM_PINGOUT_OUTPUT_PORTS];

  PRIVATE:

    // ----------------------------------------------------------------------
    // Calls for messages received on typed input ports
    // ----------------------------------------------------------------------

    //! Callback for port TlmRecv
    //!
    static void m_p_TlmRecv_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwChanIdType id, /*!< Telemetry Channel ID*/
        Fw::Time &timeTag, /*!< Time Tag*/
        Fw::TlmBuffer &val /*!< Buffer containing serialized telemetry value*/
    );

    //! Callback for port TlmGet
    //!
    static void m_p_TlmGet_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        FwChanIdType id, /*!< Telemetry Channel ID*/
        Fw::Time &timeTag, /*!< Time Tag*/
        Fw::TlmBuffer &val /*!< Buffer containing serialized telemetry value*/
    );

    //! Callback for port Run
    //!
    static void m_p_Run_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        NATIVE_UINT_TYPE context /*!< The call order*/
    );

    //! Callback for port pingIn
    //!
    static void m_p_pingIn_in(
        Fw::PassiveComponentBase* callComp, /*!< The component instance*/
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 key /*!< Value to return to pinger*/
    );

  PRIVATE:

    // ----------------------------------------------------------------------
    // Message dispatch functions
    // ----------------------------------------------------------------------

    //! Called in the message loop to dispatch a message from the queue
    //!
    virtual MsgDispatchStatus doDispatch(void);

  PRIVATE:

    // ----------------------------------------------------------------------
    // Mutexes
    // ----------------------------------------------------------------------

    //! Mutex for guarded ports
    //!
    Os::Mutex m_guardedPortMutex;



  };  

} // end namespace Svc
#endif
