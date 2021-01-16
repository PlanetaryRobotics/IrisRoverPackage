// ======================================================================
// \title  NavigationComponentImpl.cpp
// \author michael
// \brief  cpp file for Navigation component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/Navigation/NavigationComponent.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  /**
   * @brief      Constructs a new instance.
   *
   * @param[in]  compName  The component name
   */
  NavigationComponentImpl ::
#if FW_OBJECT_NAMES == 1
    NavigationComponentImpl(
        const char *const compName
    ) :
      NavigationComponentBase(compName)
#else
    NavigationComponentImpl(void)
#endif
  {

  }

  /**
   * @brief      Initialize the motor control component
   *
   * @param[in]  queueDepth  The queue depth
   * @param[in]  instance    The instance
   */
  void NavigationComponentImpl :: init(const NATIVE_INT_TYPE queueDepth,const NATIVE_INT_TYPE instance)
  {
    NavigationComponentBase::init(queueDepth, instance);
  }

  /**
   * @brief      Destroys the object.
   */
  NavigationComponentImpl :: ~NavigationComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  /**
   * @brief      Handler implementation for ping (health)
   *
   * @param[in]  portNum  The port number
   * @param[in]  key      Value to return to pinger
   */
  void NavigationComponentImpl :: PingIn_handler(const NATIVE_INT_TYPE portNum, U32 key)
  {
    this->PingOut_out(portNum, key);
  }

  /**
   * @brief      Scheduler port handler.
   *
   * @param[in]  portNum  The port number
   * @param[in]  context  ????
   */
  void NavigationComponentImpl :: schedIn_handler(const NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context)
  {
    // TODO
  }

  /**
   * @brief      Implementation for Motor Control to pass information back to Navigation
   *
   * @param[in]  portNum            The port number
   * @param[in]  movement_finished  Flag for movement being finished
   */
  void NavigationComponentImpl :: motorDataIn_handler(const NATIVE_INT_TYPE portNum, bool movement_finished)
  {
    // TODO
  }

  /**
   * @brief      Implementation for Motor Control to pass information back to Navigation
   *
   * @param[in]  portNum  The port number
   */
  void NavigationComponentImpl :: IMUDataIn_handler(const NATIVE_INT_TYPE portNum,
                                                     I16 x_acc,
                                                     I16 y_acc,
                                                     I16 z_acc,
                                                     I16 x_gyro,
                                                     I16 y_gyro,
                                                     I16 z_gyro)
  {
    // TODO
  }

  /**
   * @brief      Implementation for forward driving command
   *
   * @param[in]  portNum  The port number
   */
  void NavigationComponentImpl :: Nav_DriveForward_cmdHandler(const FwOpcodeType opCode,
                                                              const U32 cmdSeq,
                                                              U8 Distance,
                                                              U8 Speed,
                                                              U16 Callback_ID)
  {
    motorCommandOut_out(0,MC_DrivingConfiguration, MC_Forward, Distance, Speed);
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        CubeRoverPorts::MC_CommandType command_type, /*!<  Selector for which command should be called */
        CubeRoverPorts::MC_MovementType movement_type, /*!<  Selector how movement should be interpreted */
        U8 Distance, 
        U8 Speed 

    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Implementation for backward driving command
   *
   * @param[in]  portNum  The port number
   */
  void NavigationComponentImpl :: Nav_DriveBackwards_cmdHandler(const FwOpcodeType opCode,
                                                                const U32 cmdSeq,
                                                                U8 Distance,
                                                                U8 Speed,
                                                                U16 Callback_ID)
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Implementation for clockwise rotating command
   *
   * @param[in]  portNum  The port number
   */
  void NavigationComponentImpl :: Nav_RotateRight_cmdHandler(const FwOpcodeType opCode,
                                                             const U32 cmdSeq,
                                                             U8 Distance,
                                                             U8 Speed,
                                                             U16 Callback_ID)
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Implementation for counter-clockwise rotating command
   *
   * @param[in]  portNum  The port number
   */
  void NavigationComponentImpl :: Nav_RotateLeft_cmdHandler(const FwOpcodeType opCode,
                                                            const U32 cmdSeq,
                                                            U8 Distance,
                                                            U8 Speed,
                                                            U16 Callback_ID)
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Implementation for stopping moving command
   *
   * @param[in]  portNum  The port number
   */
  void NavigationComponentImpl :: Nav_StopMovement_cmdHandler(const FwOpcodeType opCode,
                                                              const U32 cmdSeq)
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

} // end namespace CubeRover
