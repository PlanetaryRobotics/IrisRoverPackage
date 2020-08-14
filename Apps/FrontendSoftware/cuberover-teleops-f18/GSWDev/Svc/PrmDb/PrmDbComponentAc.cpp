// ====================================================================== 
// \title  PrmDbComponentAc.hpp
// \author Auto-generated
// \brief  cpp file for PrmDb component base class
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
#include <Svc/PrmDb/PrmDbComponentAc.hpp>
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
      PRMDB_COMPONENT_EXIT = Fw::ActiveComponentBase::ACTIVE_COMPONENT_EXIT,
      SETPRM_PRMSET,
      PINGIN_PING,
      CMD_PRM_SAVE_FILE
    } MsgTypeEnum;

    // Get the max size by doing a union of the input port serialization sizes.

    typedef union {
      BYTE port1[Fw::InputPrmSetPort::SERIALIZED_SIZE];
      BYTE port2[Svc::InputPingPort::SERIALIZED_SIZE];
      BYTE cmdPort[Fw::InputCmdPort::SERIALIZED_SIZE];
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

  Fw::InputPrmGetPort *PrmDbComponentBase ::
    get_getPrm_InputPort(NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_getPrm_InputPorts(),static_cast<AssertArg>(portNum));
    return &this->m_getPrm_InputPort[portNum];
  }

  Fw::InputPrmSetPort *PrmDbComponentBase ::
    get_setPrm_InputPort(NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_setPrm_InputPorts(),static_cast<AssertArg>(portNum));
    return &this->m_setPrm_InputPort[portNum];
  }

  Svc::InputPingPort *PrmDbComponentBase ::
    get_pingIn_InputPort(NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_pingIn_InputPorts(),static_cast<AssertArg>(portNum));
    return &this->m_pingIn_InputPort[portNum];
  }

  Fw::InputCmdPort *PrmDbComponentBase ::
    get_CmdDisp_InputPort(NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_CmdDisp_InputPorts(),static_cast<AssertArg>(portNum));
    return &this->m_CmdDisp_InputPort[portNum];
  }

  // ----------------------------------------------------------------------
  // Typed connectors for output ports
  // ----------------------------------------------------------------------

  void PrmDbComponentBase ::
    set_pingOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Svc::InputPingPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_pingOut_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_pingOut_OutputPort[portNum].addCallPort(port);
  }

  void PrmDbComponentBase ::
    set_CmdStatus_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputCmdResponsePort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_CmdStatus_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_CmdStatus_OutputPort[portNum].addCallPort(port);
  }

  void PrmDbComponentBase ::
    set_CmdReg_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputCmdRegPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_CmdReg_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_CmdReg_OutputPort[portNum].addCallPort(port);
  }

  void PrmDbComponentBase ::
    set_Time_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputTimePort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_Time_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_Time_OutputPort[portNum].addCallPort(port);
  }

  void PrmDbComponentBase ::
    set_Log_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputLogPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_Log_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_Log_OutputPort[portNum].addCallPort(port);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void PrmDbComponentBase ::
    set_LogText_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputLogTextPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_LogText_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_LogText_OutputPort[portNum].addCallPort(port);
  }
#endif

  // ----------------------------------------------------------------------
  // Serialization connectors for output ports
  // ----------------------------------------------------------------------

#if FW_PORT_SERIALIZATION

  void PrmDbComponentBase ::
    set_pingOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_pingOut_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_pingOut_OutputPort[portNum].registerSerialPort(port);
  }

  void PrmDbComponentBase ::
    set_CmdStatus_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_CmdStatus_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_CmdStatus_OutputPort[portNum].registerSerialPort(port);
  }

  void PrmDbComponentBase ::
    set_CmdReg_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_CmdReg_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_CmdReg_OutputPort[portNum].registerSerialPort(port);
  }

  void PrmDbComponentBase ::
    set_Time_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_Time_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_Time_OutputPort[portNum].registerSerialPort(port);
  }

  void PrmDbComponentBase ::
    set_Log_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_Log_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_Log_OutputPort[portNum].registerSerialPort(port);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void PrmDbComponentBase ::
    set_LogText_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_LogText_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_LogText_OutputPort[portNum].registerSerialPort(port);
  }
#endif

#endif

  // ----------------------------------------------------------------------
  // Command registration
  // ----------------------------------------------------------------------

  void PrmDbComponentBase ::
    regCommands(void)
  {
    FW_ASSERT(this->m_CmdReg_OutputPort[0].isConnected());
    this->m_CmdReg_OutputPort[0].invoke(
        this->getIdBase() + OPCODE_PRM_SAVE_FILE
    );
  }

  // ----------------------------------------------------------------------
  // Component construction, initialization, and destruction
  // ----------------------------------------------------------------------

  PrmDbComponentBase ::
#if FW_OBJECT_NAMES == 1
    PrmDbComponentBase(const char* compName) :
      Fw::ActiveComponentBase(compName)
#else
    PrmDbComponentBase() :
      Fw::ActiveComponentBase()
#endif
  {

  
    this->m_PrmIdNotFoundThrottle = 0;
  
  }

  void PrmDbComponentBase ::
    init(
        NATIVE_INT_TYPE queueDepth,
        NATIVE_INT_TYPE instance
    )
  {

    // Initialize base class
    Fw::ActiveComponentBase::init(instance);

    // Connect input port getPrm
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_getPrm_InputPorts();
        port++
    ) {

      this->m_getPrm_InputPort[port].init();
      this->m_getPrm_InputPort[port].addCallComp(
          this,
          m_p_getPrm_in
      );
      this->m_getPrm_InputPort[port].setPortNum(port);

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_getPrm_InputPort[%d]",
          this->m_objName,
          port
      );
      this->m_getPrm_InputPort[port].setObjName(portName);
#endif

    }

    // Connect input port setPrm
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_setPrm_InputPorts();
        port++
    ) {

      this->m_setPrm_InputPort[port].init();
      this->m_setPrm_InputPort[port].addCallComp(
          this,
          m_p_setPrm_in
      );
      this->m_setPrm_InputPort[port].setPortNum(port);

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_setPrm_InputPort[%d]",
          this->m_objName,
          port
      );
      this->m_setPrm_InputPort[port].setObjName(portName);
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

    // Connect input port CmdDisp
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_CmdDisp_InputPorts();
        port++
    ) {

      this->m_CmdDisp_InputPort[port].init();
      this->m_CmdDisp_InputPort[port].addCallComp(
          this,
          m_p_CmdDisp_in
      );
      this->m_CmdDisp_InputPort[port].setPortNum(port);

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_CmdDisp_InputPort[%d]",
          this->m_objName,
          port
      );
      this->m_CmdDisp_InputPort[port].setObjName(portName);
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

    // Initialize output port CmdStatus
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_CmdStatus_OutputPorts();
        port++
    ) {
      this->m_CmdStatus_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_CmdStatus_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_CmdStatus_OutputPort[port].setObjName(portName);
#endif

    }

    // Initialize output port CmdReg
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_CmdReg_OutputPorts();
        port++
    ) {
      this->m_CmdReg_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_CmdReg_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_CmdReg_OutputPort[port].setObjName(portName);
#endif

    }

    // Initialize output port Time
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_Time_OutputPorts();
        port++
    ) {
      this->m_Time_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_Time_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_Time_OutputPort[port].setObjName(portName);
#endif

    }

    // Initialize output port Log
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_Log_OutputPorts();
        port++
    ) {
      this->m_Log_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_Log_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_Log_OutputPort[port].setObjName(portName);
#endif

    }

    // Initialize output port LogText
#if FW_ENABLE_TEXT_LOGGING == 1
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_LogText_OutputPorts();
        port++
    ) {
      this->m_LogText_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_LogText_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_LogText_OutputPort[port].setObjName(portName);
#endif

    }
#endif

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

  PrmDbComponentBase::
    ~PrmDbComponentBase() {

  }

  // ----------------------------------------------------------------------
  // Invocation functions for output ports
  // ----------------------------------------------------------------------

  void PrmDbComponentBase ::
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

  NATIVE_INT_TYPE PrmDbComponentBase ::
    getNum_getPrm_InputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_getPrm_InputPort
    );
  }

  NATIVE_INT_TYPE PrmDbComponentBase ::
    getNum_setPrm_InputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_setPrm_InputPort
    );
  }

  NATIVE_INT_TYPE PrmDbComponentBase ::
    getNum_pingIn_InputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_pingIn_InputPort
    );
  }

  NATIVE_INT_TYPE PrmDbComponentBase ::
    getNum_pingOut_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_pingOut_OutputPort
    );
  }

  NATIVE_INT_TYPE PrmDbComponentBase ::
    getNum_CmdDisp_InputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_CmdDisp_InputPort
    );
  }

  NATIVE_INT_TYPE PrmDbComponentBase ::
    getNum_CmdStatus_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_CmdStatus_OutputPort
    );
  }

  NATIVE_INT_TYPE PrmDbComponentBase ::
    getNum_CmdReg_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_CmdReg_OutputPort
    );
  }

  NATIVE_INT_TYPE PrmDbComponentBase ::
    getNum_Time_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_Time_OutputPort
    );
  }

  NATIVE_INT_TYPE PrmDbComponentBase ::
    getNum_Log_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_Log_OutputPort
    );
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE PrmDbComponentBase ::
    getNum_LogText_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_LogText_OutputPort
    );
  }
#endif

  // ----------------------------------------------------------------------
  // Port connection status queries
  // ----------------------------------------------------------------------

  bool PrmDbComponentBase ::
    isConnected_pingOut_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_pingOut_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_pingOut_OutputPort[portNum].isConnected();
  }

  bool PrmDbComponentBase ::
    isConnected_CmdStatus_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_CmdStatus_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_CmdStatus_OutputPort[portNum].isConnected();
  }

  bool PrmDbComponentBase ::
    isConnected_CmdReg_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_CmdReg_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_CmdReg_OutputPort[portNum].isConnected();
  }

  bool PrmDbComponentBase ::
    isConnected_Time_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_Time_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_Time_OutputPort[portNum].isConnected();
  }

  bool PrmDbComponentBase ::
    isConnected_Log_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_Log_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_Log_OutputPort[portNum].isConnected();
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  bool PrmDbComponentBase ::
    isConnected_LogText_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_LogText_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_LogText_OutputPort[portNum].isConnected();
  }
#endif

  // ----------------------------------------------------------------------
  // Calls for messages received on command input ports
  // ----------------------------------------------------------------------

  void PrmDbComponentBase ::
    m_p_CmdDisp_in(
        Fw::PassiveComponentBase* callComp,
        NATIVE_INT_TYPE portNum,
        FwOpcodeType opCode,
        U32 cmdSeq,
        Fw::CmdArgBuffer &args
    )
  {

    // Select base class function based on opcode
    FW_ASSERT(callComp);
    PrmDbComponentBase* compPtr =
      (PrmDbComponentBase*) callComp;

    const U32 idBase = callComp->getIdBase();
    FW_ASSERT(opCode >= idBase, opCode, idBase);
    switch (opCode - idBase) {

      case OPCODE_PRM_SAVE_FILE: /*!< Command to save parameter image to file. Uses file name passed to constructor */
        compPtr->PRM_SAVE_FILE_cmdHandlerBase(
            opCode,
            cmdSeq,
            args
        );
        break;
        
      default: {
        compPtr->cmdResponse_out(
            opCode,cmdSeq,
            Fw::COMMAND_INVALID_OPCODE
        );
        break;
      }

    }

  }

  void PrmDbComponentBase ::
    cmdResponse_out(
        FwOpcodeType opCode,
        U32 cmdSeq,
        Fw::CommandResponse response
    )
  {
    FW_ASSERT(this->m_CmdStatus_OutputPort[0].isConnected());
    this->m_CmdStatus_OutputPort[0].invoke(opCode,cmdSeq,response);
  }

  // ----------------------------------------------------------------------
  // Base class command functions
  // ----------------------------------------------------------------------

  void PrmDbComponentBase ::
    PRM_SAVE_FILE_cmdHandlerBase(
        FwOpcodeType opCode,
        U32 cmdSeq,
        Fw::CmdArgBuffer &args
    )
  {

    // Call pre-message hook
    this->PRM_SAVE_FILE_preMsgHook(opCode,cmdSeq);

    // Defer deserializing arguments to the message dispatcher
    // to avoid deserializing and reserializing just for IPC
    ComponentIpcSerializableBuffer msg;
    Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;

    // Serialize for IPC
    _status = msg.serialize((NATIVE_INT_TYPE)CMD_PRM_SAVE_FILE);
    FW_ASSERT (
        _status == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(_status)
        );

    // Fake port number to make message dequeue work
    NATIVE_INT_TYPE port = 0;
    _status = msg.serialize(port);
    FW_ASSERT (
        _status == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(_status)
    );

    _status = msg.serialize(opCode);
    FW_ASSERT (
        _status == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(_status)
    );

    _status = msg.serialize(cmdSeq);
    FW_ASSERT (
        _status == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(_status)
    );

    _status = msg.serialize(args);
    FW_ASSERT (
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

  void PrmDbComponentBase::
    PRM_SAVE_FILE_preMsgHook(
        FwOpcodeType opCode,
        U32 cmdSeq
    )
  {
    // Defaults to no-op; can be overridden
  }

  // ----------------------------------------------------------------------
  // Time
  // ----------------------------------------------------------------------

  Fw::Time PrmDbComponentBase ::
    getTime(void) 
  {
    if (this->m_Time_OutputPort[0].isConnected()) {
      Fw::Time _time;
      this->m_Time_OutputPort[0].invoke(_time);
      return _time;
    } else {
      return Fw::Time(TB_NONE,0,0);
    }
  }

  // ----------------------------------------------------------------------
  // Event handling functions
  // ----------------------------------------------------------------------

  void PrmDbComponentBase ::
    log_WARNING_LO_PrmIdNotFound(
        U32 Id
    )
  {
  
    // check throttle value
    if (this->m_PrmIdNotFoundThrottle >= EVENTID_PRMIDNOTFOUND_THROTTLE) {
        return;
    } else {
        this->m_PrmIdNotFoundThrottle++;
    }

    // Get the time
    Fw::Time _logTime;
    if (this->m_Time_OutputPort[0].isConnected()) {
       this->m_Time_OutputPort[0].invoke( _logTime);
    }
    
    FwEventIdType _id = static_cast<FwEventIdType>(0);
    
    _id = this->getIdBase() + EVENTID_PRMIDNOTFOUND;

    // Emit the event on the log port
    if (this->m_Log_OutputPort[0].isConnected()) {

      Fw::LogBuffer _logBuff;
      Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;

#if FW_AMPCS_COMPATIBLE
      // Serialize the number of arguments
      _status = _logBuff.serialize(static_cast<U8>(1));
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

#if FW_AMPCS_COMPATIBLE
      // Serialize the argument size
      _status = _logBuff.serialize(
          static_cast<U8>(sizeof(Id))
      );
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

      _status = _logBuff.serialize(Id);
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );


      this->m_Log_OutputPort[0].invoke(
          _id,
          _logTime,Fw::LOG_WARNING_LO,
          _logBuff
      );

    }

    // Emit the event on the text log port
#if FW_ENABLE_TEXT_LOGGING
    if (this->m_LogText_OutputPort[0].isConnected()) {

#if FW_OBJECT_NAMES == 1
      const char* _formatString =
        "(%s) %s: Parameter ID 0x%08X not found";
#else
      const char* _formatString =
        "%s: Parameter ID 0x%08X not found";
#endif

      char _textBuffer[FW_LOG_TEXT_BUFFER_SIZE];

      (void) snprintf(
          _textBuffer,
          FW_LOG_TEXT_BUFFER_SIZE,
          _formatString,
#if FW_OBJECT_NAMES == 1
          this->m_objName,
#endif
          "PrmIdNotFound "
        , Id
      );

      // Null terminate
      _textBuffer[FW_LOG_TEXT_BUFFER_SIZE-1] = 0;
      Fw::TextLogString _logString = _textBuffer;
      this->m_LogText_OutputPort[0].invoke(
          _id,
          _logTime,Fw::TEXT_LOG_WARNING_LO,
          _logString
      );

    }
#endif

  }
  
    void PrmDbComponentBase::log_WARNING_LO_PrmIdNotFound_ThrottleClear(void) {
        // reset throttle counter
        this->m_PrmIdNotFoundThrottle = 0;
    }

  void PrmDbComponentBase ::
    log_ACTIVITY_HI_PrmIdUpdated(
        U32 Id
    )
  {
  

    // Get the time
    Fw::Time _logTime;
    if (this->m_Time_OutputPort[0].isConnected()) {
       this->m_Time_OutputPort[0].invoke( _logTime);
    }
    
    FwEventIdType _id = static_cast<FwEventIdType>(0);
    
    _id = this->getIdBase() + EVENTID_PRMIDUPDATED;

    // Emit the event on the log port
    if (this->m_Log_OutputPort[0].isConnected()) {

      Fw::LogBuffer _logBuff;
      Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;

#if FW_AMPCS_COMPATIBLE
      // Serialize the number of arguments
      _status = _logBuff.serialize(static_cast<U8>(1));
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

#if FW_AMPCS_COMPATIBLE
      // Serialize the argument size
      _status = _logBuff.serialize(
          static_cast<U8>(sizeof(Id))
      );
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

      _status = _logBuff.serialize(Id);
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );


      this->m_Log_OutputPort[0].invoke(
          _id,
          _logTime,Fw::LOG_ACTIVITY_HI,
          _logBuff
      );

    }

    // Emit the event on the text log port
#if FW_ENABLE_TEXT_LOGGING
    if (this->m_LogText_OutputPort[0].isConnected()) {

#if FW_OBJECT_NAMES == 1
      const char* _formatString =
        "(%s) %s: Parameter ID 0x%08X updated";
#else
      const char* _formatString =
        "%s: Parameter ID 0x%08X updated";
#endif

      char _textBuffer[FW_LOG_TEXT_BUFFER_SIZE];

      (void) snprintf(
          _textBuffer,
          FW_LOG_TEXT_BUFFER_SIZE,
          _formatString,
#if FW_OBJECT_NAMES == 1
          this->m_objName,
#endif
          "PrmIdUpdated "
        , Id
      );

      // Null terminate
      _textBuffer[FW_LOG_TEXT_BUFFER_SIZE-1] = 0;
      Fw::TextLogString _logString = _textBuffer;
      this->m_LogText_OutputPort[0].invoke(
          _id,
          _logTime,Fw::TEXT_LOG_ACTIVITY_HI,
          _logString
      );

    }
#endif

  }
  

  void PrmDbComponentBase ::
    log_FATAL_PrmDbFull(
        U32 Id
    )
  {
  

    // Get the time
    Fw::Time _logTime;
    if (this->m_Time_OutputPort[0].isConnected()) {
       this->m_Time_OutputPort[0].invoke( _logTime);
    }
    
    FwEventIdType _id = static_cast<FwEventIdType>(0);
    
    _id = this->getIdBase() + EVENTID_PRMDBFULL;

    // Emit the event on the log port
    if (this->m_Log_OutputPort[0].isConnected()) {

      Fw::LogBuffer _logBuff;
      Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;

#if FW_AMPCS_COMPATIBLE
      // Serialize the number of arguments
      _status = _logBuff.serialize(static_cast<U8>(1+1));
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
      // For FATAL, add stack size of 4 and a dummy entry. No support for stacks yet.
      _status = _logBuff.serialize(static_cast<U8>(4));
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
      _status = _logBuff.serialize(static_cast<U32>(0));
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

#if FW_AMPCS_COMPATIBLE
      // Serialize the argument size
      _status = _logBuff.serialize(
          static_cast<U8>(sizeof(Id))
      );
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

      _status = _logBuff.serialize(Id);
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );


      this->m_Log_OutputPort[0].invoke(
          _id,
          _logTime,Fw::LOG_FATAL,
          _logBuff
      );

    }

    // Emit the event on the text log port
#if FW_ENABLE_TEXT_LOGGING
    if (this->m_LogText_OutputPort[0].isConnected()) {

#if FW_OBJECT_NAMES == 1
      const char* _formatString =
        "(%s) %s: Parameter DB full when adding ID 0x%08X";
#else
      const char* _formatString =
        "%s: Parameter DB full when adding ID 0x%08X";
#endif

      char _textBuffer[FW_LOG_TEXT_BUFFER_SIZE];

      (void) snprintf(
          _textBuffer,
          FW_LOG_TEXT_BUFFER_SIZE,
          _formatString,
#if FW_OBJECT_NAMES == 1
          this->m_objName,
#endif
          "PrmDbFull "
        , Id
      );

      // Null terminate
      _textBuffer[FW_LOG_TEXT_BUFFER_SIZE-1] = 0;
      Fw::TextLogString _logString = _textBuffer;
      this->m_LogText_OutputPort[0].invoke(
          _id,
          _logTime,Fw::TEXT_LOG_FATAL,
          _logString
      );

    }
#endif

  }
  

  void PrmDbComponentBase ::
    log_ACTIVITY_HI_PrmIdAdded(
        U32 Id
    )
  {
  

    // Get the time
    Fw::Time _logTime;
    if (this->m_Time_OutputPort[0].isConnected()) {
       this->m_Time_OutputPort[0].invoke( _logTime);
    }
    
    FwEventIdType _id = static_cast<FwEventIdType>(0);
    
    _id = this->getIdBase() + EVENTID_PRMIDADDED;

    // Emit the event on the log port
    if (this->m_Log_OutputPort[0].isConnected()) {

      Fw::LogBuffer _logBuff;
      Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;

#if FW_AMPCS_COMPATIBLE
      // Serialize the number of arguments
      _status = _logBuff.serialize(static_cast<U8>(1));
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

#if FW_AMPCS_COMPATIBLE
      // Serialize the argument size
      _status = _logBuff.serialize(
          static_cast<U8>(sizeof(Id))
      );
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

      _status = _logBuff.serialize(Id);
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );


      this->m_Log_OutputPort[0].invoke(
          _id,
          _logTime,Fw::LOG_ACTIVITY_HI,
          _logBuff
      );

    }

    // Emit the event on the text log port
#if FW_ENABLE_TEXT_LOGGING
    if (this->m_LogText_OutputPort[0].isConnected()) {

#if FW_OBJECT_NAMES == 1
      const char* _formatString =
        "(%s) %s: Parameter ID 0x%08X added";
#else
      const char* _formatString =
        "%s: Parameter ID 0x%08X added";
#endif

      char _textBuffer[FW_LOG_TEXT_BUFFER_SIZE];

      (void) snprintf(
          _textBuffer,
          FW_LOG_TEXT_BUFFER_SIZE,
          _formatString,
#if FW_OBJECT_NAMES == 1
          this->m_objName,
#endif
          "PrmIdAdded "
        , Id
      );

      // Null terminate
      _textBuffer[FW_LOG_TEXT_BUFFER_SIZE-1] = 0;
      Fw::TextLogString _logString = _textBuffer;
      this->m_LogText_OutputPort[0].invoke(
          _id,
          _logTime,Fw::TEXT_LOG_ACTIVITY_HI,
          _logString
      );

    }
#endif

  }
  

  void PrmDbComponentBase ::
    log_WARNING_HI_PrmFileWriteError(
        PrmWriteError stage, I32 record, I32 error
    )
  {
  

    // Get the time
    Fw::Time _logTime;
    if (this->m_Time_OutputPort[0].isConnected()) {
       this->m_Time_OutputPort[0].invoke( _logTime);
    }
    
    FwEventIdType _id = static_cast<FwEventIdType>(0);
    
    _id = this->getIdBase() + EVENTID_PRMFILEWRITEERROR;

    // Emit the event on the log port
    if (this->m_Log_OutputPort[0].isConnected()) {

      Fw::LogBuffer _logBuff;
      Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;

#if FW_AMPCS_COMPATIBLE
      // Serialize the number of arguments
      _status = _logBuff.serialize(static_cast<U8>(3));
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

#if FW_AMPCS_COMPATIBLE
      // Serialize the argument size
      _status = _logBuff.serialize(static_cast<U8>(sizeof(FwEnumStoreType)));
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

      _status = _logBuff.serialize(
          static_cast<FwEnumStoreType>(stage)
      );
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );

#if FW_AMPCS_COMPATIBLE
      // Serialize the argument size
      _status = _logBuff.serialize(
          static_cast<U8>(sizeof(record))
      );
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

      _status = _logBuff.serialize(record);
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );

#if FW_AMPCS_COMPATIBLE
      // Serialize the argument size
      _status = _logBuff.serialize(
          static_cast<U8>(sizeof(error))
      );
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

      _status = _logBuff.serialize(error);
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );


      this->m_Log_OutputPort[0].invoke(
          _id,
          _logTime,Fw::LOG_WARNING_HI,
          _logBuff
      );

    }

    // Emit the event on the text log port
#if FW_ENABLE_TEXT_LOGGING
    if (this->m_LogText_OutputPort[0].isConnected()) {

#if FW_OBJECT_NAMES == 1
      const char* _formatString =
        "(%s) %s: Parameter write failed in stage %d with record %d and error %d";
#else
      const char* _formatString =
        "%s: Parameter write failed in stage %d with record %d and error %d";
#endif

      char _textBuffer[FW_LOG_TEXT_BUFFER_SIZE];

      (void) snprintf(
          _textBuffer,
          FW_LOG_TEXT_BUFFER_SIZE,
          _formatString,
#if FW_OBJECT_NAMES == 1
          this->m_objName,
#endif
          "PrmFileWriteError "
        , stage
        , record
        , error
      );

      // Null terminate
      _textBuffer[FW_LOG_TEXT_BUFFER_SIZE-1] = 0;
      Fw::TextLogString _logString = _textBuffer;
      this->m_LogText_OutputPort[0].invoke(
          _id,
          _logTime,Fw::TEXT_LOG_WARNING_HI,
          _logString
      );

    }
#endif

  }
  

  void PrmDbComponentBase ::
    log_ACTIVITY_HI_PrmFileSaveComplete(
        U32 records
    )
  {
  

    // Get the time
    Fw::Time _logTime;
    if (this->m_Time_OutputPort[0].isConnected()) {
       this->m_Time_OutputPort[0].invoke( _logTime);
    }
    
    FwEventIdType _id = static_cast<FwEventIdType>(0);
    
    _id = this->getIdBase() + EVENTID_PRMFILESAVECOMPLETE;

    // Emit the event on the log port
    if (this->m_Log_OutputPort[0].isConnected()) {

      Fw::LogBuffer _logBuff;
      Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;

#if FW_AMPCS_COMPATIBLE
      // Serialize the number of arguments
      _status = _logBuff.serialize(static_cast<U8>(1));
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

#if FW_AMPCS_COMPATIBLE
      // Serialize the argument size
      _status = _logBuff.serialize(
          static_cast<U8>(sizeof(records))
      );
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

      _status = _logBuff.serialize(records);
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );


      this->m_Log_OutputPort[0].invoke(
          _id,
          _logTime,Fw::LOG_ACTIVITY_HI,
          _logBuff
      );

    }

    // Emit the event on the text log port
#if FW_ENABLE_TEXT_LOGGING
    if (this->m_LogText_OutputPort[0].isConnected()) {

#if FW_OBJECT_NAMES == 1
      const char* _formatString =
        "(%s) %s: Parameter file save completed. Wrote %d records.";
#else
      const char* _formatString =
        "%s: Parameter file save completed. Wrote %d records.";
#endif

      char _textBuffer[FW_LOG_TEXT_BUFFER_SIZE];

      (void) snprintf(
          _textBuffer,
          FW_LOG_TEXT_BUFFER_SIZE,
          _formatString,
#if FW_OBJECT_NAMES == 1
          this->m_objName,
#endif
          "PrmFileSaveComplete "
        , records
      );

      // Null terminate
      _textBuffer[FW_LOG_TEXT_BUFFER_SIZE-1] = 0;
      Fw::TextLogString _logString = _textBuffer;
      this->m_LogText_OutputPort[0].invoke(
          _id,
          _logTime,Fw::TEXT_LOG_ACTIVITY_HI,
          _logString
      );

    }
#endif

  }
  

  void PrmDbComponentBase ::
    log_WARNING_HI_PrmFileReadError(
        PrmReadError stage, I32 record, I32 error
    )
  {
  

    // Get the time
    Fw::Time _logTime;
    if (this->m_Time_OutputPort[0].isConnected()) {
       this->m_Time_OutputPort[0].invoke( _logTime);
    }
    
    FwEventIdType _id = static_cast<FwEventIdType>(0);
    
    _id = this->getIdBase() + EVENTID_PRMFILEREADERROR;

    // Emit the event on the log port
    if (this->m_Log_OutputPort[0].isConnected()) {

      Fw::LogBuffer _logBuff;
      Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;

#if FW_AMPCS_COMPATIBLE
      // Serialize the number of arguments
      _status = _logBuff.serialize(static_cast<U8>(3));
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

#if FW_AMPCS_COMPATIBLE
      // Serialize the argument size
      _status = _logBuff.serialize(static_cast<U8>(sizeof(FwEnumStoreType)));
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

      _status = _logBuff.serialize(
          static_cast<FwEnumStoreType>(stage)
      );
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );

#if FW_AMPCS_COMPATIBLE
      // Serialize the argument size
      _status = _logBuff.serialize(
          static_cast<U8>(sizeof(record))
      );
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

      _status = _logBuff.serialize(record);
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );

#if FW_AMPCS_COMPATIBLE
      // Serialize the argument size
      _status = _logBuff.serialize(
          static_cast<U8>(sizeof(error))
      );
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

      _status = _logBuff.serialize(error);
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );


      this->m_Log_OutputPort[0].invoke(
          _id,
          _logTime,Fw::LOG_WARNING_HI,
          _logBuff
      );

    }

    // Emit the event on the text log port
#if FW_ENABLE_TEXT_LOGGING
    if (this->m_LogText_OutputPort[0].isConnected()) {

#if FW_OBJECT_NAMES == 1
      const char* _formatString =
        "(%s) %s: Parameter file read failed in stage %d with record %d and error %d";
#else
      const char* _formatString =
        "%s: Parameter file read failed in stage %d with record %d and error %d";
#endif

      char _textBuffer[FW_LOG_TEXT_BUFFER_SIZE];

      (void) snprintf(
          _textBuffer,
          FW_LOG_TEXT_BUFFER_SIZE,
          _formatString,
#if FW_OBJECT_NAMES == 1
          this->m_objName,
#endif
          "PrmFileReadError "
        , stage
        , record
        , error
      );

      // Null terminate
      _textBuffer[FW_LOG_TEXT_BUFFER_SIZE-1] = 0;
      Fw::TextLogString _logString = _textBuffer;
      this->m_LogText_OutputPort[0].invoke(
          _id,
          _logTime,Fw::TEXT_LOG_WARNING_HI,
          _logString
      );

    }
#endif

  }
  

  void PrmDbComponentBase ::
    log_ACTIVITY_HI_PrmFileLoadComplete(
        U32 records
    )
  {
  

    // Get the time
    Fw::Time _logTime;
    if (this->m_Time_OutputPort[0].isConnected()) {
       this->m_Time_OutputPort[0].invoke( _logTime);
    }
    
    FwEventIdType _id = static_cast<FwEventIdType>(0);
    
    _id = this->getIdBase() + EVENTID_PRMFILELOADCOMPLETE;

    // Emit the event on the log port
    if (this->m_Log_OutputPort[0].isConnected()) {

      Fw::LogBuffer _logBuff;
      Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;

#if FW_AMPCS_COMPATIBLE
      // Serialize the number of arguments
      _status = _logBuff.serialize(static_cast<U8>(1));
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

#if FW_AMPCS_COMPATIBLE
      // Serialize the argument size
      _status = _logBuff.serialize(
          static_cast<U8>(sizeof(records))
      );
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

      _status = _logBuff.serialize(records);
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );


      this->m_Log_OutputPort[0].invoke(
          _id,
          _logTime,Fw::LOG_ACTIVITY_HI,
          _logBuff
      );

    }

    // Emit the event on the text log port
#if FW_ENABLE_TEXT_LOGGING
    if (this->m_LogText_OutputPort[0].isConnected()) {

#if FW_OBJECT_NAMES == 1
      const char* _formatString =
        "(%s) %s: Parameter file load completed. Read %d records.";
#else
      const char* _formatString =
        "%s: Parameter file load completed. Read %d records.";
#endif

      char _textBuffer[FW_LOG_TEXT_BUFFER_SIZE];

      (void) snprintf(
          _textBuffer,
          FW_LOG_TEXT_BUFFER_SIZE,
          _formatString,
#if FW_OBJECT_NAMES == 1
          this->m_objName,
#endif
          "PrmFileLoadComplete "
        , records
      );

      // Null terminate
      _textBuffer[FW_LOG_TEXT_BUFFER_SIZE-1] = 0;
      Fw::TextLogString _logString = _textBuffer;
      this->m_LogText_OutputPort[0].invoke(
          _id,
          _logTime,Fw::TEXT_LOG_ACTIVITY_HI,
          _logString
      );

    }
#endif

  }
  

  // ----------------------------------------------------------------------
  // Mutex functions for guarded ports
  // ----------------------------------------------------------------------

  void PrmDbComponentBase ::
    lock(void)
  {
    this->m_guardedPortMutex.lock();
  }

  void PrmDbComponentBase ::
    unLock(void)
  {
    this->m_guardedPortMutex.unLock();
  }

  // ----------------------------------------------------------------------
  // Calls for invocations received on typed input ports
  // ----------------------------------------------------------------------

  Fw::ParamValid PrmDbComponentBase ::
    m_p_getPrm_in(
        Fw::PassiveComponentBase* callComp,
        NATIVE_INT_TYPE portNum,
        FwPrmIdType id, Fw::ParamBuffer &val
    )
  {
    FW_ASSERT(callComp);
    PrmDbComponentBase* compPtr =
      (PrmDbComponentBase*) callComp;
    return compPtr->getPrm_handlerBase(portNum, id, val);
  }

  void PrmDbComponentBase ::
    m_p_setPrm_in(
        Fw::PassiveComponentBase* callComp,
        NATIVE_INT_TYPE portNum,
        FwPrmIdType id, Fw::ParamBuffer &val
    )
  {
    FW_ASSERT(callComp);
    PrmDbComponentBase* compPtr =
      (PrmDbComponentBase*) callComp;
    compPtr->setPrm_handlerBase(portNum, id, val);
  }

  void PrmDbComponentBase ::
    m_p_pingIn_in(
        Fw::PassiveComponentBase* callComp,
        NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    FW_ASSERT(callComp);
    PrmDbComponentBase* compPtr =
      (PrmDbComponentBase*) callComp;
    compPtr->pingIn_handlerBase(portNum, key);
  }

  // ----------------------------------------------------------------------
  // Port handler base-class functions for typed input ports
  // ----------------------------------------------------------------------

  Fw::ParamValid PrmDbComponentBase ::
    getPrm_handlerBase(
        NATIVE_INT_TYPE portNum,
        FwPrmIdType id, Fw::ParamBuffer &val
    )
  {

    // Make sure port number is valid
    FW_ASSERT(portNum < this->getNum_getPrm_InputPorts(),static_cast<AssertArg>(portNum));
    Fw::ParamValid  retVal;

    // Lock guard mutex before calling
    this->lock();

    // Down call to pure virtual handler method implemented in Impl class
    retVal = this->getPrm_handler(portNum, id, val);

    // Unlock guard mutex
    this->unLock();

    return retVal;

  }

  void PrmDbComponentBase ::
    setPrm_handlerBase(
        NATIVE_INT_TYPE portNum,
        FwPrmIdType id, Fw::ParamBuffer &val
    )
  {

    // Make sure port number is valid
    FW_ASSERT(portNum < this->getNum_setPrm_InputPorts(),static_cast<AssertArg>(portNum));

    // Call pre-message hook
    setPrm_preMsgHook(
        portNum,
        id, val
    );

    ComponentIpcSerializableBuffer msg;
    Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;

    _status = msg.serialize(
        static_cast<NATIVE_INT_TYPE>(SETPRM_PRMSET)
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

    // Serialize argument id
    _status = msg.serialize(id);
    FW_ASSERT(
        _status == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(_status)
    );

    // Serialize argument val
    _status = msg.serialize(val);
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

  void PrmDbComponentBase ::
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

  void PrmDbComponentBase ::
    setPrm_preMsgHook(
        NATIVE_INT_TYPE portNum,
        FwPrmIdType id, Fw::ParamBuffer &val
    )
  {
    // Default: no-op
  }

  void PrmDbComponentBase ::
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

  Fw::QueuedComponentBase::MsgDispatchStatus PrmDbComponentBase ::
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

    if (msgType == PRMDB_COMPONENT_EXIT) {
      return MSG_DISPATCH_EXIT;
    }

    NATIVE_INT_TYPE portNum;
    deserStatus = msg.deserialize(portNum);
    FW_ASSERT(
        deserStatus == Fw::FW_SERIALIZE_OK,
        static_cast<AssertArg>(deserStatus)
    );

    switch (msgType) {

      // Handle async input port setPrm
      case SETPRM_PRMSET: {

        // Deserialize argument id
        FwPrmIdType id;
        deserStatus = msg.deserialize(id);
        FW_ASSERT(
            deserStatus == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(deserStatus)
        );

        // Deserialize argument val
        Fw::ParamBuffer val;
        deserStatus = msg.deserialize(val);
        FW_ASSERT(
            deserStatus == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(deserStatus)
        );

        // Call handler function
        this->setPrm_handler(
            portNum,
            id, val
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

      // Handle command PRM_SAVE_FILE
      case CMD_PRM_SAVE_FILE: {
        // Deserialize opcode
        FwOpcodeType opCode;
        deserStatus = msg.deserialize(opCode);
        FW_ASSERT (
            deserStatus == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(deserStatus)
        );

        // Deserialize command sequence
        U32 cmdSeq;
        deserStatus = msg.deserialize(cmdSeq);
        FW_ASSERT (
            deserStatus == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(deserStatus)
        );

        // Deserialize command argument buffer
        Fw::CmdArgBuffer args;
        deserStatus = msg.deserialize(args);
        FW_ASSERT (
            deserStatus == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(deserStatus)
        );

        // Reset buffer
        args.resetDeser();

        // Make sure there was no data left over.
        // That means the argument buffer size was incorrect.
#if FW_CMD_CHECK_RESIDUAL        
        if (args.getBuffLeft() != 0) {
          if (this->m_CmdStatus_OutputPort[0].isConnected()) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_FORMAT_ERROR);
          }
          // Don't crash the task if bad arguments were passed from the ground
          break;
        }
#endif
        // Call handler function
        this->PRM_SAVE_FILE_cmdHandler(opCode, cmdSeq);

        break;

      }

      default:
        return MSG_DISPATCH_ERROR;

    }

    return MSG_DISPATCH_OK;

  }

} // end namespace Svc
