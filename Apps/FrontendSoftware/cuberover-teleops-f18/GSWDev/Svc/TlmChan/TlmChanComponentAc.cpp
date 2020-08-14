// ====================================================================== 
// \title  TlmChanComponentAc.hpp
// \author Auto-generated
// \brief  cpp file for TlmChan component base class
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
#include <Svc/TlmChan/TlmChanComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#if FW_ENABLE_TEXT_LOGGING
#include <Fw/Types/EightyCharString.hpp>
#endif


namespace Svc {

  // ----------------------------------------------------------------------
  // Anonymous namespace to prevent name collisions
  // ----------------------------------------------------------------------

  namespace {

    typedef enum {
      TLMCHAN_COMPONENT_EXIT = Fw::ActiveComponentBase::ACTIVE_COMPONENT_EXIT,
      RUN_SCHED,
      PINGIN_PING
    } MsgTypeEnum;

    // Get the max size by doing a union of the input port serialization sizes.

    typedef union {
      BYTE port1[Svc::InputSchedPort::SERIALIZED_SIZE];
      BYTE port2[Svc::InputPingPort::SERIALIZED_SIZE];
    } BuffUnion;

    // Define a message buffer class large enough to handle all the
    // asynchronous inputs to the component

    class ComponentIpcSerializableBuffer : 
      public Fw::SerializeBufferBase
    {

      public:

        enum {
          // Max. message size = size of data + message id + port
          SERIALIZATION_SIZE =
            sizeof(BuffUnion) +
            sizeof(NATIVE_INT_TYPE) +
            sizeof(NATIVE_INT_TYPE)
        };

        NATIVE_UINT_TYPE getBuffCapacity(void) const {
          return sizeof(m_buff);
        }

        U8* getBuffAddr(void) {
          return m_buff;
        }

        const U8* getBuffAddr(void) const {
          return m_buff;
        }

      private:
        // Should be the max of all the input ports serialized sizes...
        U8 m_buff[SERIALIZATION_SIZE];

    };

  }

  // ----------------------------------------------------------------------
  // Getters for numbers of input ports
  // ----------------------------------------------------------------------

  Fw::InputTlmPort *TlmChanComponentBase ::
    get_TlmRecv_InputPort(NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_TlmRecv_InputPorts(),static_cast<AssertArg>(portNum));
    return &this->m_TlmRecv_InputPort[portNum];
  }

  Fw::InputTlmPort *TlmChanComponentBase ::
    get_TlmGet_InputPort(NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_TlmGet_InputPorts(),static_cast<AssertArg>(portNum));
    return &this->m_TlmGet_InputPort[portNum];
  }

  Svc::InputSchedPort *TlmChanComponentBase ::
    get_Run_InputPort(NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_Run_InputPorts(),static_cast<AssertArg>(portNum));
    return &this->m_Run_InputPort[portNum];
  }

  Svc::InputPingPort *TlmChanComponentBase ::
    get_pingIn_InputPort(NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_pingIn_InputPorts(),static_cast<AssertArg>(portNum));
    return &this->m_pingIn_InputPort[portNum];
  }

  // ----------------------------------------------------------------------
  // Typed connectors for output ports
  // ----------------------------------------------------------------------

  void TlmChanComponentBase ::
    set_PktSend_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputComPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_PktSend_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_PktSend_OutputPort[portNum].addCallPort(port);
  }

  void TlmChanComponentBase ::
    set_pingOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Svc::InputPingPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_pingOut_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_pingOut_OutputPort[portNum].addCallPort(port);
  }

  // ----------------------------------------------------------------------
  // Serialization connectors for output ports
  // ----------------------------------------------------------------------

#if FW_PORT_SERIALIZATION

  void TlmChanComponentBase ::
    set_PktSend_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_PktSend_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_PktSend_OutputPort[portNum].registerSerialPort(port);
  }

  void TlmChanComponentBase ::
    set_pingOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_pingOut_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_pingOut_OutputPort[portNum].registerSerialPort(port);
  }

#endif

  // ----------------------------------------------------------------------
  // Component construction, initialization, and destruction
  // ----------------------------------------------------------------------

  TlmChanComponentBase ::
#if FW_OBJECT_NAMES == 1
    TlmChanComponentBase(const char* compName) :
      Fw::ActiveComponentBase(compName)
#else
    TlmChanComponentBase() :
      Fw::ActiveComponentBase()
#endif
  {

  
  
  }

  void TlmChanComponentBase ::
    init(
        NATIVE_INT_TYPE queueDepth,
        NATIVE_INT_TYPE instance
    )
  {

    // Initialize base class
    Fw::ActiveComponentBase::init(instance);

    // Connect input port TlmRecv
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_TlmRecv_InputPorts();
        port++
    ) {

      this->m_TlmRecv_InputPort[port].init();
      this->m_TlmRecv_InputPort[port].addCallComp(
          this,
          m_p_TlmRecv_in
      );
      this->m_TlmRecv_InputPort[port].setPortNum(port);

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_TlmRecv_InputPort[%d]",
          this->m_objName,
          port
      );
      this->m_TlmRecv_InputPort[port].setObjName(portName);
#endif

    }

    // Connect input port TlmGet
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_TlmGet_InputPorts();
        port++
    ) {

      this->m_TlmGet_InputPort[port].init();
      this->m_TlmGet_InputPort[port].addCallComp(
          this,
          m_p_TlmGet_in
      );
      this->m_TlmGet_InputPort[port].setPortNum(port);

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_TlmGet_InputPort[%d]",
          this->m_objName,
          port
      );
      this->m_TlmGet_InputPort[port].setObjName(portName);
#endif

    }

    // Connect input port Run
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_Run_InputPorts();
        port++
    ) {

      this->m_Run_InputPort[port].init();
      this->m_Run_InputPort[port].addCallComp(
          this,
          m_p_Run_in
      );
      this->m_Run_InputPort[port].setPortNum(port);

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_Run_InputPort[%d]",
          this->m_objName,
          port
      );
      this->m_Run_InputPort[port].setObjName(portName);
#endif

    }

    // Connect input port pingIn
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_pingIn_InputPorts();
        port++
    ) {

      this->m_pingIn_InputPort[port].init();
      this->m_pingIn_InputPort[port].addCallComp(
          this,
          m_p_pingIn_in
      );
      this->m_pingIn_InputPort[port].setPortNum(port);

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_pingIn_InputPort[%d]",
          this->m_objName,
          port
      );
      this->m_pingIn_InputPort[port].setObjName(portName);
#endif

    }

    // Initialize output port PktSend
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_PktSend_OutputPorts();
        port++
    ) {
      this->m_PktSend_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_PktSend_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_PktSend_OutputPort[port].setObjName(portName);
#endif

    }

    // Initialize output port pingOut
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_pingOut_OutputPorts();
        port++
    ) {
      this->m_pingOut_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_pingOut_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_pingOut_OutputPort[port].setObjName(portName);
#endif

    }

    Os::Queue::QueueStatus qStat =
    this->createQueue(
        queueDepth,
        ComponentIpcSerializableBuffer::SERIALIZATION_SIZE
    );
    FW_ASSERT(
        Os::Queue::QUEUE_OK == qStat,
        static_cast<AssertArg>(qStat)
    );

  }

  TlmChanComponentBase::
    ~TlmChanComponentBase() {

  }

  // ----------------------------------------------------------------------
  // Invocation functions for output ports
  // ----------------------------------------------------------------------

  void TlmChanComponentBase ::
    PktSend_out(
        NATIVE_INT_TYPE portNum,
        Fw::ComBuffer &data, U32 context
    )
  {
    FW_ASSERT(portNum < this->getNum_PktSend_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_PktSend_OutputPort[portNum].invoke(data, context);
  }

  void TlmChanComponentBase ::
    pingOut_out(
        NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    FW_ASSERT(portNum < this->getNum_pingOut_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_pingOut_OutputPort[portNum].invoke(key);
  }

  // ----------------------------------------------------------------------
  // Getters for numbers of ports
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE TlmChanComponentBase ::
    getNum_TlmRecv_InputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_TlmRecv_InputPort
    );
  }

  NATIVE_INT_TYPE TlmChanComponentBase ::
    getNum_TlmGet_InputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_TlmGet_InputPort
    );
  }

  NATIVE_INT_TYPE TlmChanComponentBase ::
    getNum_Run_InputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_Run_InputPort
    );
  }

  NATIVE_INT_TYPE TlmChanComponentBase ::
    getNum_PktSend_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_PktSend_OutputPort
    );
  }

  NATIVE_INT_TYPE TlmChanComponentBase ::
    getNum_pingIn_InputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_pingIn_InputPort
    );
  }

  NATIVE_INT_TYPE TlmChanComponentBase ::
    getNum_pingOut_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_pingOut_OutputPort
    );
  }

  // ----------------------------------------------------------------------
  // Port connection status queries
  // ----------------------------------------------------------------------

  bool TlmChanComponentBase ::
    isConnected_PktSend_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_PktSend_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_PktSend_OutputPort[portNum].isConnected();
  }

  bool TlmChanComponentBase ::
    isConnected_pingOut_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_pingOut_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_pingOut_OutputPort[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Mutex functions for guarded ports
  // ----------------------------------------------------------------------

  void TlmChanComponentBase ::
    lock(void)
  {
    this->m_guardedPortMutex.lock();
  }

  void TlmChanComponentBase ::
    unLock(void)
  {
    this->m_guardedPortMutex.unLock();
  }

  // ----------------------------------------------------------------------
  // Calls for invocations received on typed input ports
  // ----------------------------------------------------------------------

  void TlmChanComponentBase ::
    m_p_TlmRecv_in(
        Fw::PassiveComponentBase* callComp,
        NATIVE_INT_TYPE portNum,
        FwChanIdType id, Fw::Time &timeTag, Fw::TlmBuffer &val
    )
  {
    FW_ASSERT(callComp);
    TlmChanComponentBase* compPtr =
      (TlmChanComponentBase*) callComp;
    compPtr->TlmRecv_handlerBase(portNum, id, timeTag, val);
  }

  void TlmChanComponentBase ::
    m_p_TlmGet_in(
        Fw::PassiveComponentBase* callComp,
        NATIVE_INT_TYPE portNum,
        FwChanIdType id, Fw::Time &timeTag, Fw::TlmBuffer &val
    )
  {
    FW_ASSERT(callComp);
    TlmChanComponentBase* compPtr =
      (TlmChanComponentBase*) callComp;
    compPtr->TlmGet_handlerBase(portNum, id, timeTag, val);
  }

  void TlmChanComponentBase ::
    m_p_Run_in(
        Fw::PassiveComponentBase* callComp,
        NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    FW_ASSERT(callComp);
    TlmChanComponentBase* compPtr =
      (TlmChanComponentBase*) callComp;
    compPtr->Run_handlerBase(portNum, context);
  }

  void TlmChanComponentBase ::
    m_p_pingIn_in(
        Fw::PassiveComponentBase* callComp,
        NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    FW_ASSERT(callComp);
    TlmChanComponentBase* compPtr =
      (TlmChanComponentBase*) callComp;
    compPtr->pingIn_handlerBase(portNum, key);
  }

  // ----------------------------------------------------------------------
  // Port handler base-class functions for typed input ports
  // ----------------------------------------------------------------------

  void TlmChanComponentBase ::
    TlmRecv_handlerBase(
        NATIVE_INT_TYPE portNum,
        FwChanIdType id, Fw::Time &timeTag, Fw::TlmBuffer &val
    )
  {

    // Make sure port number is valid
    FW_ASSERT(portNum < this->getNum_TlmRecv_InputPorts(),static_cast<AssertArg>(portNum));

    // Lock guard mutex before calling
    this->lock();

    // Down call to pure virtual handler method implemented in Impl class
    this->TlmRecv_handler(portNum, id, timeTag, val);

    // Unlock guard mutex
    this->unLock();

  }

  void TlmChanComponentBase ::
    TlmGet_handlerBase(
        NATIVE_INT_TYPE portNum,
        FwChanIdType id, Fw::Time &timeTag, Fw::TlmBuffer &val
    )
  {

    // Make sure port number is valid
    FW_ASSERT(portNum < this->getNum_TlmGet_InputPorts(),static_cast<AssertArg>(portNum));

    // Lock guard mutex before calling
    this->lock();

    // Down call to pure virtual handler method implemented in Impl class
    this->TlmGet_handler(portNum, id, timeTag, val);

    // Unlock guard mutex
    this->unLock();

  }

  void TlmChanComponentBase ::
    Run_handlerBase(
        NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {

    // Make sure port number is valid
    FW_ASSERT(portNum < this->getNum_Run_InputPorts(),static_cast<AssertArg>(portNum));

    // Call pre-message hook
    Run_preMsgHook(
        portNum,
        context
    );

    ComponentIpcSerializableBuffer msg;
    Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;

    _status = msg.serialize(
        static_cast<NATIVE_INT_TYPE>(RUN_SCHED)
    );
    FW_ASSERT (
        _status == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(_status)
    );

    _status = msg.serialize(portNum);
    FW_ASSERT (
        _status == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(_status)
    );

    // Serialize argument context
    _status = msg.serialize(context);
    FW_ASSERT(
        _status == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(_status)
    );

      
    // send message
    Os::Queue::QueueBlocking _block = 
      Os::Queue::QUEUE_NONBLOCKING;
    Os::Queue::QueueStatus qStatus =
      this->m_queue.send(msg, 0,_block);
    FW_ASSERT(
        qStatus == Os::Queue::QUEUE_OK,
        static_cast<AssertArg>(qStatus)
    );      
      
  }

  void TlmChanComponentBase ::
    pingIn_handlerBase(
        NATIVE_INT_TYPE portNum,
        U32 key
    )
  {

    // Make sure port number is valid
    FW_ASSERT(portNum < this->getNum_pingIn_InputPorts(),static_cast<AssertArg>(portNum));

    // Call pre-message hook
    pingIn_preMsgHook(
        portNum,
        key
    );

    ComponentIpcSerializableBuffer msg;
    Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;

    _status = msg.serialize(
        static_cast<NATIVE_INT_TYPE>(PINGIN_PING)
    );
    FW_ASSERT (
        _status == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(_status)
    );

    _status = msg.serialize(portNum);
    FW_ASSERT (
        _status == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(_status)
    );

    // Serialize argument key
    _status = msg.serialize(key);
    FW_ASSERT(
        _status == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(_status)
    );

      
    // send message
    Os::Queue::QueueBlocking _block = 
      Os::Queue::QUEUE_NONBLOCKING;
    Os::Queue::QueueStatus qStatus =
      this->m_queue.send(msg, 0,_block);
    FW_ASSERT(
        qStatus == Os::Queue::QUEUE_OK,
        static_cast<AssertArg>(qStatus)
    );      
      
  }

  // ----------------------------------------------------------------------
  // Pre-message hooks for async input ports
  // ----------------------------------------------------------------------

  void TlmChanComponentBase ::
    Run_preMsgHook(
        NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    // Default: no-op
  }

  void TlmChanComponentBase ::
    pingIn_preMsgHook(
        NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    // Default: no-op
  }

  // ----------------------------------------------------------------------
  // Message dispatch method for active and queued components. Called
  // by active component thread or implementation code for queued components
  // ----------------------------------------------------------------------

  Fw::QueuedComponentBase::MsgDispatchStatus TlmChanComponentBase ::
    doDispatch(void)
  {
    ComponentIpcSerializableBuffer msg;
    NATIVE_INT_TYPE priority;

    Os::Queue::QueueStatus msgStatus = this->m_queue.receive(msg,priority,Os::Queue::QUEUE_BLOCKING);
    FW_ASSERT(
        msgStatus == Os::Queue::QUEUE_OK,
        static_cast<AssertArg>(msgStatus)
    );

    // Reset to beginning of buffer
    msg.resetDeser();

    NATIVE_INT_TYPE desMsg;
    Fw::SerializeStatus deserStatus = msg.deserialize(desMsg);
    FW_ASSERT(
        deserStatus == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(deserStatus)
    );

    MsgTypeEnum msgType = static_cast<MsgTypeEnum>(desMsg);

    if (msgType == TLMCHAN_COMPONENT_EXIT) {
      return MSG_DISPATCH_EXIT;
    }

    NATIVE_INT_TYPE portNum;
    deserStatus = msg.deserialize(portNum);
    FW_ASSERT(
        deserStatus == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(deserStatus)
    );

    switch (msgType) {

      // Handle async input port Run
      case RUN_SCHED: {

        // Deserialize argument context
        NATIVE_UINT_TYPE context;
        deserStatus = msg.deserialize(context);
        FW_ASSERT(
            deserStatus == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(deserStatus)
        );

        // Call handler function
        this->Run_handler(
            portNum,
            context
        );

        break;

      }

      // Handle async input port pingIn
      case PINGIN_PING: {

        // Deserialize argument key
        U32 key;
        deserStatus = msg.deserialize(key);
        FW_ASSERT(
            deserStatus == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(deserStatus)
        );

        // Call handler function
        this->pingIn_handler(
            portNum,
            key
        );

        break;

      }

      default:
        return MSG_DISPATCH_ERROR;

    }

    return MSG_DISPATCH_OK;

  }

} // end namespace Svc
