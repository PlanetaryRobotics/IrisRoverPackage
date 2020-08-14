// ====================================================================== 
// \title  RecvBuffComponentAc.hpp
// \author Auto-generated
// \brief  cpp file for RecvBuff component base class
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
#include <Ref/RecvBuffApp/RecvBuffComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#if FW_ENABLE_TEXT_LOGGING
#include <Fw/Types/EightyCharString.hpp>
#endif


namespace Ref {

  // ----------------------------------------------------------------------
  // Getters for numbers of input ports
  // ----------------------------------------------------------------------

  Drv::InputDataBufferPort *RecvBuffComponentBase ::
    get_Data_InputPort(NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_Data_InputPorts(),static_cast<AssertArg>(portNum));
    return &this->m_Data_InputPort[portNum];
  }

  Fw::InputCmdPort *RecvBuffComponentBase ::
    get_CmdDisp_InputPort(NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_CmdDisp_InputPorts(),static_cast<AssertArg>(portNum));
    return &this->m_CmdDisp_InputPort[portNum];
  }

  // ----------------------------------------------------------------------
  // Typed connectors for output ports
  // ----------------------------------------------------------------------

  void RecvBuffComponentBase ::
    set_CmdStatus_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputCmdResponsePort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_CmdStatus_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_CmdStatus_OutputPort[portNum].addCallPort(port);
  }

  void RecvBuffComponentBase ::
    set_CmdReg_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputCmdRegPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_CmdReg_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_CmdReg_OutputPort[portNum].addCallPort(port);
  }

  void RecvBuffComponentBase ::
    set_ParamGet_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputPrmGetPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_ParamGet_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_ParamGet_OutputPort[portNum].addCallPort(port);
  }

  void RecvBuffComponentBase ::
    set_ParamSet_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputPrmSetPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_ParamSet_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_ParamSet_OutputPort[portNum].addCallPort(port);
  }

  void RecvBuffComponentBase ::
    set_Tlm_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputTlmPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_Tlm_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_Tlm_OutputPort[portNum].addCallPort(port);
  }

  void RecvBuffComponentBase ::
    set_Time_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputTimePort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_Time_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_Time_OutputPort[portNum].addCallPort(port);
  }

  void RecvBuffComponentBase ::
    set_Log_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputLogPort* port
    )
  {
    FW_ASSERT(portNum < this->getNum_Log_OutputPorts(),static_cast<AssertArg>(portNum));
    this->m_Log_OutputPort[portNum].addCallPort(port);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void RecvBuffComponentBase ::
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

  void RecvBuffComponentBase ::
    set_CmdStatus_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_CmdStatus_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_CmdStatus_OutputPort[portNum].registerSerialPort(port);
  }

  void RecvBuffComponentBase ::
    set_CmdReg_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_CmdReg_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_CmdReg_OutputPort[portNum].registerSerialPort(port);
  }

  void RecvBuffComponentBase ::
    set_ParamGet_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_ParamGet_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_ParamGet_OutputPort[portNum].registerSerialPort(port);
  }

  void RecvBuffComponentBase ::
    set_ParamSet_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_ParamSet_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_ParamSet_OutputPort[portNum].registerSerialPort(port);
  }

  void RecvBuffComponentBase ::
    set_Tlm_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_Tlm_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_Tlm_OutputPort[portNum].registerSerialPort(port);
  }

  void RecvBuffComponentBase ::
    set_Time_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_Time_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_Time_OutputPort[portNum].registerSerialPort(port);
  }

  void RecvBuffComponentBase ::
    set_Log_OutputPort(
        NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *port
    )
  {
    FW_ASSERT(portNum < this->getNum_Log_OutputPorts(),static_cast<AssertArg>(portNum));
    return this->m_Log_OutputPort[portNum].registerSerialPort(port);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void RecvBuffComponentBase ::
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

  void RecvBuffComponentBase ::
    regCommands(void)
  {
    FW_ASSERT(this->m_CmdReg_OutputPort[0].isConnected());
    this->m_CmdReg_OutputPort[0].invoke(
        this->getIdBase() + OPCODE_PARAMETER1_SET
    );
    this->m_CmdReg_OutputPort[0].invoke(
        this->getIdBase() + OPCODE_PARAMETER1_SAVE
    );
    this->m_CmdReg_OutputPort[0].invoke(
        this->getIdBase() + OPCODE_PARAMETER2_SET
    );
    this->m_CmdReg_OutputPort[0].invoke(
        this->getIdBase() + OPCODE_PARAMETER2_SAVE
    );
  }

  // ----------------------------------------------------------------------
  // Parameter loading
  // ----------------------------------------------------------------------

  void RecvBuffComponentBase::loadParameters(void) {

    Fw::ParamBuffer buff;
    Fw::SerializeStatus stat = Fw::FW_SERIALIZE_OK;
    FW_ASSERT(this->m_ParamGet_OutputPort[0].isConnected());

    FwPrmIdType _id; 
    // choose parameter ID to get
    _id = this->getIdBase() + PARAMID_PARAMETER1;
    // Get parameter parameter1
    this->m_param_parameter1_valid =
      this->m_ParamGet_OutputPort[0].invoke(
          _id,
          buff
      );

    // Deserialize value
    this->m_paramLock.lock();

    if (this->m_param_parameter1_valid == Fw::PARAM_VALID) {
      stat = buff.deserialize(this->m_parameter1);
      // If there was a deserialization issue, mark it invalid.
      if (stat != Fw::FW_SERIALIZE_OK) {
        this->m_param_parameter1_valid = Fw::PARAM_DEFAULT;
        // set default value
        this->m_parameter1 = 10;
      }
    }
    else {
      // set default value
      this->m_param_parameter1_valid = Fw::PARAM_DEFAULT;
      this->m_parameter1 = 10;
    }

    this->m_paramLock.unLock();

    // choose parameter ID to get
    _id = this->getIdBase() + PARAMID_PARAMETER2;
    // Get parameter parameter2
    this->m_param_parameter2_valid =
      this->m_ParamGet_OutputPort[0].invoke(
          _id,
          buff
      );

    // Deserialize value
    this->m_paramLock.lock();

    if (this->m_param_parameter2_valid == Fw::PARAM_VALID) {
      stat = buff.deserialize(this->m_parameter2);
      // If there was a deserialization issue, mark it invalid.
      if (stat != Fw::FW_SERIALIZE_OK) {
        this->m_param_parameter2_valid = Fw::PARAM_DEFAULT;
        // set default value
        this->m_parameter2 = 11;
      }
    }
    else {
      // set default value
      this->m_param_parameter2_valid = Fw::PARAM_DEFAULT;
      this->m_parameter2 = 11;
    }

    this->m_paramLock.unLock();

    // Call notifier
    this->parametersLoaded();
  }

  // ----------------------------------------------------------------------
  // Component construction, initialization, and destruction
  // ----------------------------------------------------------------------

  RecvBuffComponentBase ::
#if FW_OBJECT_NAMES == 1
    RecvBuffComponentBase(const char* compName) :
      Fw::PassiveComponentBase(compName)
#else
    RecvBuffComponentBase() :
      Fw::PassiveComponentBase()
#endif
  {


    // Initialize telemetry channel NumErrPkts
    this->m_first_update_NumErrPkts = true;
    this->m_last_NumErrPkts = 0;
    // Initialize telemetry channel Parameter1
    this->m_first_update_Parameter1 = true;
    this->m_last_Parameter1 = 0;
    // Initialize telemetry channel Parameter2
    this->m_first_update_Parameter2 = true;
    this->m_last_Parameter2 = 0;
  
  
    this->m_param_parameter1_valid = Fw::PARAM_UNINIT;
    this->m_param_parameter2_valid = Fw::PARAM_UNINIT;
  }

  void RecvBuffComponentBase ::
    init(NATIVE_INT_TYPE instance)
  {

    // Initialize base class
    Fw::PassiveComponentBase::init(instance);

    // Connect input port Data
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_Data_InputPorts();
        port++
    ) {

      this->m_Data_InputPort[port].init();
      this->m_Data_InputPort[port].addCallComp(
          this,
          m_p_Data_in
      );
      this->m_Data_InputPort[port].setPortNum(port);

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_Data_InputPort[%d]",
          this->m_objName,
          port
      );
      this->m_Data_InputPort[port].setObjName(portName);
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

    // Initialize output port ParamGet
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_ParamGet_OutputPorts();
        port++
    ) {
      this->m_ParamGet_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_ParamGet_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_ParamGet_OutputPort[port].setObjName(portName);
#endif

    }

    // Initialize output port ParamSet
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_ParamSet_OutputPorts();
        port++
    ) {
      this->m_ParamSet_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_ParamSet_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_ParamSet_OutputPort[port].setObjName(portName);
#endif

    }

    // Initialize output port Tlm
    for (
        NATIVE_INT_TYPE port = 0;
        port < this->getNum_Tlm_OutputPorts();
        port++
    ) {
      this->m_Tlm_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      char portName[80];
      (void) snprintf(
          portName,
          sizeof(portName),
          "%s_Tlm_OutputPort[%d]",
          this->m_objName,
          port
      );
      this->m_Tlm_OutputPort[port].setObjName(portName);
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


  }

  RecvBuffComponentBase::
    ~RecvBuffComponentBase() {

  }

  // ----------------------------------------------------------------------
  // Invocation functions for output ports
  // ----------------------------------------------------------------------

  // ----------------------------------------------------------------------
  // Getters for numbers of ports
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE RecvBuffComponentBase ::
    getNum_Data_InputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_Data_InputPort
    );
  }

  NATIVE_INT_TYPE RecvBuffComponentBase ::
    getNum_CmdDisp_InputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_CmdDisp_InputPort
    );
  }

  NATIVE_INT_TYPE RecvBuffComponentBase ::
    getNum_CmdStatus_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_CmdStatus_OutputPort
    );
  }

  NATIVE_INT_TYPE RecvBuffComponentBase ::
    getNum_CmdReg_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_CmdReg_OutputPort
    );
  }

  NATIVE_INT_TYPE RecvBuffComponentBase ::
    getNum_ParamGet_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_ParamGet_OutputPort
    );
  }

  NATIVE_INT_TYPE RecvBuffComponentBase ::
    getNum_ParamSet_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_ParamSet_OutputPort
    );
  }

  NATIVE_INT_TYPE RecvBuffComponentBase ::
    getNum_Tlm_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_Tlm_OutputPort
    );
  }

  NATIVE_INT_TYPE RecvBuffComponentBase ::
    getNum_Time_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_Time_OutputPort
    );
  }

  NATIVE_INT_TYPE RecvBuffComponentBase ::
    getNum_Log_OutputPorts(void)
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(
        this->m_Log_OutputPort
    );
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE RecvBuffComponentBase ::
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

  bool RecvBuffComponentBase ::
    isConnected_CmdStatus_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_CmdStatus_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_CmdStatus_OutputPort[portNum].isConnected();
  }

  bool RecvBuffComponentBase ::
    isConnected_CmdReg_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_CmdReg_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_CmdReg_OutputPort[portNum].isConnected();
  }

  bool RecvBuffComponentBase ::
    isConnected_ParamGet_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_ParamGet_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_ParamGet_OutputPort[portNum].isConnected();
  }

  bool RecvBuffComponentBase ::
    isConnected_ParamSet_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_ParamSet_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_ParamSet_OutputPort[portNum].isConnected();
  }

  bool RecvBuffComponentBase ::
    isConnected_Tlm_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_Tlm_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_Tlm_OutputPort[portNum].isConnected();
  }

  bool RecvBuffComponentBase ::
    isConnected_Time_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_Time_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_Time_OutputPort[portNum].isConnected();
  }

  bool RecvBuffComponentBase ::
    isConnected_Log_OutputPort(NATIVE_INT_TYPE portNum)
  {
     FW_ASSERT(
        portNum < this->getNum_Log_OutputPorts(),
        static_cast<AssertArg>(portNum)
     );
     return this->m_Log_OutputPort[portNum].isConnected();
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  bool RecvBuffComponentBase ::
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

  void RecvBuffComponentBase ::
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
    RecvBuffComponentBase* compPtr =
      (RecvBuffComponentBase*) callComp;

    const U32 idBase = callComp->getIdBase();
    FW_ASSERT(opCode >= idBase, opCode, idBase);
    switch (opCode - idBase) {

      case OPCODE_PARAMETER1_SET: 
      {
        Fw::CommandResponse _cstat = compPtr->paramSet_parameter1(args);
        compPtr->cmdResponse_out(
            opCode,
            cmdSeq,
            _cstat
        );
        break;
      }
      case OPCODE_PARAMETER1_SAVE: 
      {
        Fw::CommandResponse _cstat = compPtr->paramSave_parameter1();
        compPtr->cmdResponse_out(
            opCode,
            cmdSeq,
            _cstat
        );
        break;
      }
      case OPCODE_PARAMETER2_SET: 
      {
        Fw::CommandResponse _cstat = compPtr->paramSet_parameter2(args);
        compPtr->cmdResponse_out(
            opCode,
            cmdSeq,
            _cstat
        );
        break;
      }
      case OPCODE_PARAMETER2_SAVE: 
      {
        Fw::CommandResponse _cstat = compPtr->paramSave_parameter2();
        compPtr->cmdResponse_out(
            opCode,
            cmdSeq,
            _cstat
        );
        break;
      }
      default: {
        compPtr->cmdResponse_out(
            opCode,cmdSeq,
            Fw::COMMAND_INVALID_OPCODE
        );
        break;
      }

    }

  }

  void RecvBuffComponentBase ::
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
  // Telemetry write functions
  // ----------------------------------------------------------------------

  void RecvBuffComponentBase ::
    tlmWrite_NumPkts(U32 arg)
  {

    if (this->m_Tlm_OutputPort[0].isConnected()) {
      Fw::Time _tlmTime;
      if (this->m_Time_OutputPort[0].isConnected()) {
         this->m_Time_OutputPort[0].invoke( _tlmTime);
      }
      Fw::TlmBuffer _tlmBuff;
      Fw::SerializeStatus _stat = _tlmBuff.serialize(arg);
      FW_ASSERT(
          _stat == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_stat)
      );
      
      FwChanIdType _id;
      
      _id = this->getIdBase() + CHANNELID_NUMPKTS;
      
      this->m_Tlm_OutputPort[0].invoke(
          _id,
          _tlmTime,
          _tlmBuff
      );
    }

  }

  void RecvBuffComponentBase ::
    tlmWrite_NumErrPkts(U32 arg)
  {

    // Check to see if it is the first time
    if (not this->m_first_update_NumErrPkts) {
      // Check to see if value has changed. If not, don't write it.
      if (arg == this->m_last_NumErrPkts) {
        return;
      } else {
        this->m_last_NumErrPkts = arg;
      }
    }
    else {
      this->m_first_update_NumErrPkts = false;
      this->m_last_NumErrPkts = arg;
    }

    if (this->m_Tlm_OutputPort[0].isConnected()) {
      Fw::Time _tlmTime;
      if (this->m_Time_OutputPort[0].isConnected()) {
         this->m_Time_OutputPort[0].invoke( _tlmTime);
      }
      Fw::TlmBuffer _tlmBuff;
      Fw::SerializeStatus _stat = _tlmBuff.serialize(arg);
      FW_ASSERT(
          _stat == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_stat)
      );
      
      FwChanIdType _id;
      
      _id = this->getIdBase() + CHANNELID_NUMERRPKTS;
      
      this->m_Tlm_OutputPort[0].invoke(
          _id,
          _tlmTime,
          _tlmBuff
      );
    }

  }

  void RecvBuffComponentBase ::
    tlmWrite_Sensor1(F32 arg)
  {

    if (this->m_Tlm_OutputPort[0].isConnected()) {
      Fw::Time _tlmTime;
      if (this->m_Time_OutputPort[0].isConnected()) {
         this->m_Time_OutputPort[0].invoke( _tlmTime);
      }
      Fw::TlmBuffer _tlmBuff;
      Fw::SerializeStatus _stat = _tlmBuff.serialize(arg);
      FW_ASSERT(
          _stat == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_stat)
      );
      
      FwChanIdType _id;
      
      _id = this->getIdBase() + CHANNELID_SENSOR1;
      
      this->m_Tlm_OutputPort[0].invoke(
          _id,
          _tlmTime,
          _tlmBuff
      );
    }

  }

  void RecvBuffComponentBase ::
    tlmWrite_Sensor2(F32 arg)
  {

    if (this->m_Tlm_OutputPort[0].isConnected()) {
      Fw::Time _tlmTime;
      if (this->m_Time_OutputPort[0].isConnected()) {
         this->m_Time_OutputPort[0].invoke( _tlmTime);
      }
      Fw::TlmBuffer _tlmBuff;
      Fw::SerializeStatus _stat = _tlmBuff.serialize(arg);
      FW_ASSERT(
          _stat == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_stat)
      );
      
      FwChanIdType _id;
      
      _id = this->getIdBase() + CHANNELID_SENSOR2;
      
      this->m_Tlm_OutputPort[0].invoke(
          _id,
          _tlmTime,
          _tlmBuff
      );
    }

  }

  void RecvBuffComponentBase ::
    tlmWrite_Parameter1(U32 arg)
  {

    // Check to see if it is the first time
    if (not this->m_first_update_Parameter1) {
      // Check to see if value has changed. If not, don't write it.
      if (arg == this->m_last_Parameter1) {
        return;
      } else {
        this->m_last_Parameter1 = arg;
      }
    }
    else {
      this->m_first_update_Parameter1 = false;
      this->m_last_Parameter1 = arg;
    }

    if (this->m_Tlm_OutputPort[0].isConnected()) {
      Fw::Time _tlmTime;
      if (this->m_Time_OutputPort[0].isConnected()) {
         this->m_Time_OutputPort[0].invoke( _tlmTime);
      }
      Fw::TlmBuffer _tlmBuff;
      Fw::SerializeStatus _stat = _tlmBuff.serialize(arg);
      FW_ASSERT(
          _stat == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_stat)
      );
      
      FwChanIdType _id;
      
      _id = this->getIdBase() + CHANNELID_PARAMETER1;
      
      this->m_Tlm_OutputPort[0].invoke(
          _id,
          _tlmTime,
          _tlmBuff
      );
    }

  }

  void RecvBuffComponentBase ::
    tlmWrite_Parameter2(I16 arg)
  {

    // Check to see if it is the first time
    if (not this->m_first_update_Parameter2) {
      // Check to see if value has changed. If not, don't write it.
      if (arg == this->m_last_Parameter2) {
        return;
      } else {
        this->m_last_Parameter2 = arg;
      }
    }
    else {
      this->m_first_update_Parameter2 = false;
      this->m_last_Parameter2 = arg;
    }

    if (this->m_Tlm_OutputPort[0].isConnected()) {
      Fw::Time _tlmTime;
      if (this->m_Time_OutputPort[0].isConnected()) {
         this->m_Time_OutputPort[0].invoke( _tlmTime);
      }
      Fw::TlmBuffer _tlmBuff;
      Fw::SerializeStatus _stat = _tlmBuff.serialize(arg);
      FW_ASSERT(
          _stat == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_stat)
      );
      
      FwChanIdType _id;
      
      _id = this->getIdBase() + CHANNELID_PARAMETER2;
      
      this->m_Tlm_OutputPort[0].invoke(
          _id,
          _tlmTime,
          _tlmBuff
      );
    }

  }

  // ----------------------------------------------------------------------
  // Time
  // ----------------------------------------------------------------------

  Fw::Time RecvBuffComponentBase ::
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
  // Parameter update hook
  // ----------------------------------------------------------------------

  void RecvBuffComponentBase ::
    parameterUpdated(FwPrmIdType id)
  {
    // Do nothing by default
  }

  // ----------------------------------------------------------------------
  // Parameter load hook
  // ----------------------------------------------------------------------

  void RecvBuffComponentBase ::
    parametersLoaded()
  {
    // Do nothing by default
  }

  // ----------------------------------------------------------------------
  // Protected parameter get functions
  // ----------------------------------------------------------------------

  U32 RecvBuffComponentBase ::
    paramGet_parameter1(Fw::ParamValid& valid)
  {
    U32 _local;
    this->m_paramLock.lock();
    valid = this->m_param_parameter1_valid;
    _local = this->m_parameter1;
    this->m_paramLock.unLock();
    return _local;
  }

  I16 RecvBuffComponentBase ::
    paramGet_parameter2(Fw::ParamValid& valid)
  {
    I16 _local;
    this->m_paramLock.lock();
    valid = this->m_param_parameter2_valid;
    _local = this->m_parameter2;
    this->m_paramLock.unLock();
    return _local;
  }

  // ----------------------------------------------------------------------
  // Private parameter get function
  // ----------------------------------------------------------------------

  Fw::ParamValid RecvBuffComponentBase ::
    getParam(
      FwPrmIdType id, 
      Fw::ParamBuffer& buff
    ) 
  {
    if (this->m_ParamGet_OutputPort[0].isConnected()) {
      return this->m_ParamGet_OutputPort[0].invoke(id,buff);
    } else {
      return Fw::PARAM_INVALID;
    }

  }

  // ----------------------------------------------------------------------
  // Parameter set functions
  // ----------------------------------------------------------------------

  Fw::CommandResponse RecvBuffComponentBase ::
    paramSet_parameter1(Fw::SerializeBufferBase &val)
  {

    U32 _local_val;
    Fw::SerializeStatus _stat = val.deserialize(_local_val);
    if (_stat != Fw::FW_SERIALIZE_OK) {
      return Fw::COMMAND_VALIDATION_ERROR;
    }

    // Assign value only if successfully deserialized
    this->m_paramLock.lock();
    this->m_parameter1 = _local_val;
    this->m_paramLock.unLock();

    // Call notifier
    this->parameterUpdated(PARAMID_PARAMETER1);
    return Fw::COMMAND_OK;

  }

  // ----------------------------------------------------------------------
  // Parameter save functions
  // ----------------------------------------------------------------------

  Fw::CommandResponse RecvBuffComponentBase ::
    paramSave_parameter1(void)
  {

    if (this->m_ParamSet_OutputPort[0].isConnected()) {
      Fw::ParamBuffer saveBuff;
      this->m_paramLock.lock();

      Fw::SerializeStatus stat =
        saveBuff.serialize(m_parameter1);
      this->m_paramLock.unLock();
      if (stat != Fw::FW_SERIALIZE_OK) {
        return Fw::COMMAND_VALIDATION_ERROR;
      }

      FwPrmIdType id = 0;
      id = this->getIdBase() + PARAMID_PARAMETER1;
      // Save the parameter
      this->m_ParamSet_OutputPort[0].invoke(
          id,
          saveBuff
      );
      return Fw::COMMAND_OK;

    }

    return Fw::COMMAND_EXECUTION_ERROR;

  }

  Fw::CommandResponse RecvBuffComponentBase ::
    paramSet_parameter2(Fw::SerializeBufferBase &val)
  {

    I16 _local_val;
    Fw::SerializeStatus _stat = val.deserialize(_local_val);
    if (_stat != Fw::FW_SERIALIZE_OK) {
      return Fw::COMMAND_VALIDATION_ERROR;
    }

    // Assign value only if successfully deserialized
    this->m_paramLock.lock();
    this->m_parameter2 = _local_val;
    this->m_paramLock.unLock();

    // Call notifier
    this->parameterUpdated(PARAMID_PARAMETER2);
    return Fw::COMMAND_OK;

  }

  // ----------------------------------------------------------------------
  // Parameter save functions
  // ----------------------------------------------------------------------

  Fw::CommandResponse RecvBuffComponentBase ::
    paramSave_parameter2(void)
  {

    if (this->m_ParamSet_OutputPort[0].isConnected()) {
      Fw::ParamBuffer saveBuff;
      this->m_paramLock.lock();

      Fw::SerializeStatus stat =
        saveBuff.serialize(m_parameter2);
      this->m_paramLock.unLock();
      if (stat != Fw::FW_SERIALIZE_OK) {
        return Fw::COMMAND_VALIDATION_ERROR;
      }

      FwPrmIdType id = 0;
      id = this->getIdBase() + PARAMID_PARAMETER2;
      // Save the parameter
      this->m_ParamSet_OutputPort[0].invoke(
          id,
          saveBuff
      );
      return Fw::COMMAND_OK;

    }

    return Fw::COMMAND_EXECUTION_ERROR;

  }

  // ----------------------------------------------------------------------
  // Event handling functions
  // ----------------------------------------------------------------------

  void RecvBuffComponentBase ::
    log_ACTIVITY_LO_FirstPacketReceived(
        U32 id
    )
  {
  

    // Get the time
    Fw::Time _logTime;
    if (this->m_Time_OutputPort[0].isConnected()) {
       this->m_Time_OutputPort[0].invoke( _logTime);
    }
    
    FwEventIdType _id = static_cast<FwEventIdType>(0);
    
    _id = this->getIdBase() + EVENTID_FIRSTPACKETRECEIVED;

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
          static_cast<U8>(sizeof(id))
      );
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

      _status = _logBuff.serialize(id);
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );


      this->m_Log_OutputPort[0].invoke(
          _id,
          _logTime,Fw::LOG_ACTIVITY_LO,
          _logBuff
      );

    }

    // Emit the event on the text log port
#if FW_ENABLE_TEXT_LOGGING
    if (this->m_LogText_OutputPort[0].isConnected()) {

#if FW_OBJECT_NAMES == 1
      const char* _formatString =
        "(%s) %s: First packet ID %d received";
#else
      const char* _formatString =
        "%s: First packet ID %d received";
#endif

      char _textBuffer[FW_LOG_TEXT_BUFFER_SIZE];

      (void) snprintf(
          _textBuffer,
          FW_LOG_TEXT_BUFFER_SIZE,
          _formatString,
#if FW_OBJECT_NAMES == 1
          this->m_objName,
#endif
          "FirstPacketReceived "
        , id
      );

      // Null terminate
      _textBuffer[FW_LOG_TEXT_BUFFER_SIZE-1] = 0;
      Fw::TextLogString _logString = _textBuffer;
      this->m_LogText_OutputPort[0].invoke(
          _id,
          _logTime,Fw::TEXT_LOG_ACTIVITY_LO,
          _logString
      );

    }
#endif

  }
  

  void RecvBuffComponentBase ::
    log_WARNING_HI_PacketChecksumError(
        U32 id
    )
  {
  

    // Get the time
    Fw::Time _logTime;
    if (this->m_Time_OutputPort[0].isConnected()) {
       this->m_Time_OutputPort[0].invoke( _logTime);
    }
    
    FwEventIdType _id = static_cast<FwEventIdType>(0);
    
    _id = this->getIdBase() + EVENTID_PACKETCHECKSUMERROR;

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
          static_cast<U8>(sizeof(id))
      );
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

      _status = _logBuff.serialize(id);
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
        "(%s) %s: Packet ID %d had checksum error";
#else
      const char* _formatString =
        "%s: Packet ID %d had checksum error";
#endif

      char _textBuffer[FW_LOG_TEXT_BUFFER_SIZE];

      (void) snprintf(
          _textBuffer,
          FW_LOG_TEXT_BUFFER_SIZE,
          _formatString,
#if FW_OBJECT_NAMES == 1
          this->m_objName,
#endif
          "PacketChecksumError "
        , id
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
  

  void RecvBuffComponentBase ::
    log_ACTIVITY_LO_BuffRecvParameterUpdated(
        U32 id
    )
  {
  

    // Get the time
    Fw::Time _logTime;
    if (this->m_Time_OutputPort[0].isConnected()) {
       this->m_Time_OutputPort[0].invoke( _logTime);
    }
    
    FwEventIdType _id = static_cast<FwEventIdType>(0);
    
    _id = this->getIdBase() + EVENTID_BUFFRECVPARAMETERUPDATED;

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
          static_cast<U8>(sizeof(id))
      );
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );
#endif

      _status = _logBuff.serialize(id);
      FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
      );


      this->m_Log_OutputPort[0].invoke(
          _id,
          _logTime,Fw::LOG_ACTIVITY_LO,
          _logBuff
      );

    }

    // Emit the event on the text log port
#if FW_ENABLE_TEXT_LOGGING
    if (this->m_LogText_OutputPort[0].isConnected()) {

#if FW_OBJECT_NAMES == 1
      const char* _formatString =
        "(%s) %s: BuffRecv Parameter %d was updated";
#else
      const char* _formatString =
        "%s: BuffRecv Parameter %d was updated";
#endif

      char _textBuffer[FW_LOG_TEXT_BUFFER_SIZE];

      (void) snprintf(
          _textBuffer,
          FW_LOG_TEXT_BUFFER_SIZE,
          _formatString,
#if FW_OBJECT_NAMES == 1
          this->m_objName,
#endif
          "BuffRecvParameterUpdated "
        , id
      );

      // Null terminate
      _textBuffer[FW_LOG_TEXT_BUFFER_SIZE-1] = 0;
      Fw::TextLogString _logString = _textBuffer;
      this->m_LogText_OutputPort[0].invoke(
          _id,
          _logTime,Fw::TEXT_LOG_ACTIVITY_LO,
          _logString
      );

    }
#endif

  }
  

  // ----------------------------------------------------------------------
  // Calls for invocations received on typed input ports
  // ----------------------------------------------------------------------

  void RecvBuffComponentBase ::
    m_p_Data_in(
        Fw::PassiveComponentBase* callComp,
        NATIVE_INT_TYPE portNum,
        Drv::DataBuffer &buff
    )
  {
    FW_ASSERT(callComp);
    RecvBuffComponentBase* compPtr =
      (RecvBuffComponentBase*) callComp;
    compPtr->Data_handlerBase(portNum, buff);
  }

  // ----------------------------------------------------------------------
  // Port handler base-class functions for typed input ports
  // ----------------------------------------------------------------------

  void RecvBuffComponentBase ::
    Data_handlerBase(
        NATIVE_INT_TYPE portNum,
        Drv::DataBuffer &buff
    )
  {

    // Make sure port number is valid
    FW_ASSERT(portNum < this->getNum_Data_InputPorts(),static_cast<AssertArg>(portNum));

    // Down call to pure virtual handler method implemented in Impl class
    this->Data_handler(portNum, buff);

  }

} // end namespace Ref
