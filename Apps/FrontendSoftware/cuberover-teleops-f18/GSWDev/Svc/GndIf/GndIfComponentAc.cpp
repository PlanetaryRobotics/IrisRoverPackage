// ====================================================================== 
// \title  GndIfComponentAc.hpp
// \author Auto-generated
// \brief  cpp file for GndIf component base class
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
#include <Svc/GndIf/GndIfComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#if FW_ENABLE_TEXT_LOGGING
#include <Fw/Types/EightyCharString.hpp>
#endif


namespace Svc {

  // ----------------------------------------------------------------------
  // Getters for numbers of input ports
  // ----------------------------------------------------------------------

  Fw::InputComPort *GndIfComponentBase ::
    get_downlinkPort_InputPort(NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_downlinkPort_InputPorts(),static_cast<AssertArg>(portNum));
    return &this->m_downlinkPort_InputPort[portNum];
  }

  Fw::InputBufferSendPort *GndIfComponentBase ::
    get_fileDownlinkBufferSendIn_InputPort(NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_fileDownlinkBufferSendIn_InputPorts(),static_cast<AssertArg>(portNum));
    return &this->m_fileDownlinkBufferSendIn_InputPort[portNum];
  }

  // ----------------------------------------------------------------------
  // Typed connectors for output ports
  // ----------------------------------------------------------------------

  void GndIfComponentBase ::
    set_fileUplinkBufferSendOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputBufferSendPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_fileUplinkBufferSendOut_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_fileUplinkBufferSendOut_OutputPort[portNum].addCallPort(port);
  }

  void GndIfComponentBase ::
    set_Log_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputLogPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_Log_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_Log_OutputPort[portNum].addCallPort(port);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void GndIfComponentBase ::
    set_LogText_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputLogTextPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_LogText_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_LogText_OutputPort[portNum].addCallPort(port);
  }
#endif

  void GndIfComponentBase ::
    set_Time_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputTimePort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_Time_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_Time_OutputPort[portNum].addCallPort(port);
  }

  void GndIfComponentBase ::
    set_uplinkPort_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputComPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_uplinkPort_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_uplinkPort_OutputPort[portNum].addCallPort(port);
  }

  void GndIfComponentBase ::
    set_fileDownlinkBufferSendOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputBufferSendPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_fileDownlinkBufferSendOut_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_fileDownlinkBufferSendOut_OutputPort[portNum].addCallPort(port);
  }

  void GndIfComponentBase ::
    set_fileUplinkBufferGet_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputBufferGetPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_fileUplinkBufferGet_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_fileUplinkBufferGet_OutputPort[portNum].addCallPort(port);
  }

  // ----------------------------------------------------------------------
  // Serialization connectors for output ports
  // ----------------------------------------------------------------------

#if FW_PORT_SERIALIZATION

  void GndIfComponentBase ::
    set_fileUplinkBufferSendOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_fileUplinkBufferSendOut_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_fileUplinkBufferSendOut_OutputPort[portNum].registerSerialPort(port);
  }

  void GndIfComponentBase ::
    set_Log_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_Log_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_Log_OutputPort[portNum].registerSerialPort(port);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void GndIfComponentBase ::
    set_LogText_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_LogText_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_LogText_OutputPort[portNum].registerSerialPort(port);
  }
#endif

  void GndIfComponentBase ::
    set_Time_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_Time_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_Time_OutputPort[portNum].registerSerialPort(port);
  }

  void GndIfComponentBase ::
    set_uplinkPort_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_uplinkPort_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_uplinkPort_OutputPort[portNum].registerSerialPort(port);
  }

  void GndIfComponentBase ::
    set_fileDownlinkBufferSendOut_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_fileDownlinkBufferSendOut_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_fileDownlinkBufferSendOut_OutputPort[portNum].registerSerialPort(port);
  }

  void GndIfComponentBase ::
    set_fileUplinkBufferGet_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_fileUplinkBufferGet_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_fileUplinkBufferGet_OutputPort[portNum].registerSerialPort(port);
  }

#endif

  // ----------------------------------------------------------------------
  // Component construction, initialization, and destruction
  // ----------------------------------------------------------------------

  GndIfComponentBase ::
#if FW_OBJECT_NAMES == 1
    GndIfComponentBase(const char* compName) :
      Fw::PassiveComponentBase(compName)
#else
    GndIfComponentBase() :
      Fw::PassiveComponentBase()
#endif
  {


  
  
  }

  void GndIfComponentBase ::
    init(NATIVE_INT_TYPE instance)
  {

    // Initialize base class
    Fw::PassiveComponentBase::init(instance);

    // Connect input port downlinkPort
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_downlinkPort_InputPorts();
        port++
    ) {

      this->m_downlinkPort_InputPort[port].init();
      this->m_downlinkPort_InputPort[port].addCallComp(
          this,
          m_p_downlinkPort_in
      );
      this->m_downlinkPort_InputPort[port].setPortNum(port);

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_downlinkPort_InputPort[%d]",
          this->m_objName,
          port
      );
      this->m_downlinkPort_InputPort[port].setObjName(portName);
#endif

    }

    // Connect input port fileDownlinkBufferSendIn
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_fileDownlinkBufferSendIn_InputPorts();
        port++
    ) {

      this->m_fileDownlinkBufferSendIn_InputPort[port].init();
      this->m_fileDownlinkBufferSendIn_InputPort[port].addCallComp(
          this,
          m_p_fileDownlinkBufferSendIn_in
      );
      this->m_fileDownlinkBufferSendIn_InputPort[port].setPortNum(port);

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_fileDownlinkBufferSendIn_InputPort[%d]",
          this->m_objName,
          port
      );
      this->m_fileDownlinkBufferSendIn_InputPort[port].setObjName(portName);
#endif

    }

    // Initialize output port fileUplinkBufferSendOut
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_fileUplinkBufferSendOut_OutputPorts();
        port++
    ) {
      this->m_fileUplinkBufferSendOut_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_fileUplinkBufferSendOut_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_fileUplinkBufferSendOut_OutputPort[port].setObjName(portName);
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

    // Initialize output port uplinkPort
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_uplinkPort_OutputPorts();
        port++
    ) {
      this->m_uplinkPort_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_uplinkPort_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_uplinkPort_OutputPort[port].setObjName(portName);
#endif

    }

    // Initialize output port fileDownlinkBufferSendOut
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_fileDownlinkBufferSendOut_OutputPorts();
        port++
    ) {
      this->m_fileDownlinkBufferSendOut_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_fileDownlinkBufferSendOut_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_fileDownlinkBufferSendOut_OutputPort[port].setObjName(portName);
#endif

    }

    // Initialize output port fileUplinkBufferGet
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_fileUplinkBufferGet_OutputPorts();
        port++
    ) {
      this->m_fileUplinkBufferGet_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_fileUplinkBufferGet_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_fileUplinkBufferGet_OutputPort[port].setObjName(portName);
#endif

    }


  }

  GndIfComponentBase::
    ~GndIfComponentBase() {

  }

  // ----------------------------------------------------------------------
  // Invocation functions for output ports
  // ----------------------------------------------------------------------

  void GndIfComponentBase ::
    fileUplinkBufferSendOut_out(
        NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    FW_ASSERT(portNum < this->getNum_fileUplinkBufferSendOut_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_fileUplinkBufferSendOut_OutputPort[portNum].invoke(fwBuffer);
  }

  void GndIfComponentBase ::
    uplinkPort_out(
        NATIVE_INT_TYPE portNum,
        Fw::ComBuffer &data, U32 context
    )
  {
    FW_ASSERT(portNum < this->getNum_uplinkPort_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_uplinkPort_OutputPort[portNum].invoke(data, context);
  }

  void GndIfComponentBase ::
    fileDownlinkBufferSendOut_out(
        NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    FW_ASSERT(portNum < this->getNum_fileDownlinkBufferSendOut_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_fileDownlinkBufferSendOut_OutputPort[portNum].invoke(fwBuffer);
  }

  Fw::Buffer GndIfComponentBase ::
    fileUplinkBufferGet_out(
        NATIVE_INT_TYPE portNum,
        U32 size
    )
  {
    FW_ASSERT(portNum < this->getNum_fileUplinkBufferGet_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_fileUplinkBufferGet_OutputPort[portNum].invoke(size);
  }

  // ----------------------------------------------------------------------
  // Getters for numbers of ports
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE GndIfComponentBase ::
    getNum_fileUplinkBufferSendOut_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_fileUplinkBufferSendOut_OutputPort
    );
  }

  NATIVE_INT_TYPE GndIfComponentBase ::
    getNum_Log_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_Log_OutputPort
    );
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE GndIfComponentBase ::
    getNum_LogText_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_LogText_OutputPort
    );
  }
#endif

  NATIVE_INT_TYPE GndIfComponentBase ::
    getNum_downlinkPort_InputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_downlinkPort_InputPort
    );
  }

  NATIVE_INT_TYPE GndIfComponentBase ::
    getNum_Time_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_Time_OutputPort
    );
  }

  NATIVE_INT_TYPE GndIfComponentBase ::
    getNum_uplinkPort_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_uplinkPort_OutputPort
    );
  }

  NATIVE_INT_TYPE GndIfComponentBase ::
    getNum_fileDownlinkBufferSendOut_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_fileDownlinkBufferSendOut_OutputPort
    );
  }

  NATIVE_INT_TYPE GndIfComponentBase ::
    getNum_fileDownlinkBufferSendIn_InputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_fileDownlinkBufferSendIn_InputPort
    );
  }

  NATIVE_INT_TYPE GndIfComponentBase ::
    getNum_fileUplinkBufferGet_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_fileUplinkBufferGet_OutputPort
    );
  }

  // ----------------------------------------------------------------------
  // Port connection status queries
  // ----------------------------------------------------------------------

  bool GndIfComponentBase ::
    isConnected_fileUplinkBufferSendOut_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_fileUplinkBufferSendOut_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_fileUplinkBufferSendOut_OutputPort[portNum].isConnected();
  }

  bool GndIfComponentBase ::
    isConnected_Log_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_Log_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_Log_OutputPort[portNum].isConnected();
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  bool GndIfComponentBase ::
    isConnected_LogText_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_LogText_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_LogText_OutputPort[portNum].isConnected();
  }
#endif

  bool GndIfComponentBase ::
    isConnected_Time_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_Time_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_Time_OutputPort[portNum].isConnected();
  }

  bool GndIfComponentBase ::
    isConnected_uplinkPort_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_uplinkPort_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_uplinkPort_OutputPort[portNum].isConnected();
  }

  bool GndIfComponentBase ::
    isConnected_fileDownlinkBufferSendOut_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_fileDownlinkBufferSendOut_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_fileDownlinkBufferSendOut_OutputPort[portNum].isConnected();
  }

  bool GndIfComponentBase ::
    isConnected_fileUplinkBufferGet_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_fileUplinkBufferGet_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_fileUplinkBufferGet_OutputPort[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Time
  // ----------------------------------------------------------------------

  Fw::Time GndIfComponentBase ::
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

  void GndIfComponentBase ::
    log_WARNING_LO_NoConnectionToServer(
        U32 PortNumber
    )
  {
  

    // Get the time
    Fw::Time _logTime;
    if (this->m_Time_OutputPort[0].isConnected()) {
       this->m_Time_OutputPort[0].invoke( _logTime);
    }
    
    FwEventIdType _id = static_cast<FwEventIdType>(0);
    
    _id = this->getIdBase() + EVENTID_NOCONNECTIONTOSERVER;

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
          static_cast<U8>(sizeof(PortNumber))
      );
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

      _status = _logBuff.serialize(PortNumber);
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
        "(%s) %s: Unable to connect to the socket server on port %d";
#else
      const char* _formatString =
        "%s: Unable to connect to the socket server on port %d";
#endif

      char _textBuffer[FW_LOG_TEXT_BUFFER_SIZE];

      (void) snprintf(
          _textBuffer,
          FW_LOG_TEXT_BUFFER_SIZE,
          _formatString,
#if FW_OBJECT_NAMES == 1
          this->m_objName,
#endif
          "NoConnectionToServer "
        , PortNumber
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
  

  void GndIfComponentBase ::
    log_ACTIVITY_HI_ConnectedToServer(
        U32 PortNumber
    )
  {
  

    // Get the time
    Fw::Time _logTime;
    if (this->m_Time_OutputPort[0].isConnected()) {
       this->m_Time_OutputPort[0].invoke( _logTime);
    }
    
    FwEventIdType _id = static_cast<FwEventIdType>(0);
    
    _id = this->getIdBase() + EVENTID_CONNECTEDTOSERVER;

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
          static_cast<U8>(sizeof(PortNumber))
      );
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

      _status = _logBuff.serialize(PortNumber);
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
        "(%s) %s: Connection made to the socket server on port %d";
#else
      const char* _formatString =
        "%s: Connection made to the socket server on port %d";
#endif

      char _textBuffer[FW_LOG_TEXT_BUFFER_SIZE];

      (void) snprintf(
          _textBuffer,
          FW_LOG_TEXT_BUFFER_SIZE,
          _formatString,
#if FW_OBJECT_NAMES == 1
          this->m_objName,
#endif
          "ConnectedToServer "
        , PortNumber
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
  

  void GndIfComponentBase ::
    log_WARNING_LO_LostConnectionToServer(
        U32 PortNumber
    )
  {
  

    // Get the time
    Fw::Time _logTime;
    if (this->m_Time_OutputPort[0].isConnected()) {
       this->m_Time_OutputPort[0].invoke( _logTime);
    }
    
    FwEventIdType _id = static_cast<FwEventIdType>(0);
    
    _id = this->getIdBase() + EVENTID_LOSTCONNECTIONTOSERVER;

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
          static_cast<U8>(sizeof(PortNumber))
      );
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

      _status = _logBuff.serialize(PortNumber);
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
        "(%s) %s: Connection lost with socket server on port %d";
#else
      const char* _formatString =
        "%s: Connection lost with socket server on port %d";
#endif

      char _textBuffer[FW_LOG_TEXT_BUFFER_SIZE];

      (void) snprintf(
          _textBuffer,
          FW_LOG_TEXT_BUFFER_SIZE,
          _formatString,
#if FW_OBJECT_NAMES == 1
          this->m_objName,
#endif
          "LostConnectionToServer "
        , PortNumber
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
  

  // ----------------------------------------------------------------------
  // Calls for invocations received on typed input ports
  // ----------------------------------------------------------------------

  void GndIfComponentBase ::
    m_p_downlinkPort_in(
        Fw::PassiveComponentBase* callComp,
        NATIVE_INT_TYPE portNum,
        Fw::ComBuffer &data, U32 context
    )
  {
    FW_ASSERT(callComp);
    GndIfComponentBase* compPtr =
      (GndIfComponentBase*) callComp;
    compPtr->downlinkPort_handlerBase(portNum, data, context);
  }

  void GndIfComponentBase ::
    m_p_fileDownlinkBufferSendIn_in(
        Fw::PassiveComponentBase* callComp,
        NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    FW_ASSERT(callComp);
    GndIfComponentBase* compPtr =
      (GndIfComponentBase*) callComp;
    compPtr->fileDownlinkBufferSendIn_handlerBase(portNum, fwBuffer);
  }

  // ----------------------------------------------------------------------
  // Port handler base-class functions for typed input ports
  // ----------------------------------------------------------------------

  void GndIfComponentBase ::
    downlinkPort_handlerBase(
        NATIVE_INT_TYPE portNum,
        Fw::ComBuffer &data, U32 context
    )
  {

    // Make sure port number is valid
    FW_ASSERT(portNum < this->getNum_downlinkPort_InputPorts(),static_cast<AssertArg>(portNum));

    // Down call to pure virtual handler method implemented in Impl class
    this->downlinkPort_handler(portNum, data, context);

  }

  void GndIfComponentBase ::
    fileDownlinkBufferSendIn_handlerBase(
        NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {

    // Make sure port number is valid
    FW_ASSERT(portNum < this->getNum_fileDownlinkBufferSendIn_InputPorts(),static_cast<AssertArg>(portNum));

    // Down call to pure virtual handler method implemented in Impl class
    this->fileDownlinkBufferSendIn_handler(portNum, fwBuffer);

  }

} // end namespace Svc
