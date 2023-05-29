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
#include "Include/settingsConfig.h"

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
    m_currState = NAV_IDLE;
    m_currBtSchema = NAV_INBETWEEN_STEPS;
    m_currMoving = false;
    m_imagingInProgress = false;

    for(int i = 0; i < NAV_MOVE_BUFFER_LIMIT; i++)
    {
      m_moveRecord[i].Callback_ID = 0;
      m_moveRecord[i].Distance = 0;
      m_moveRecord[i].Speed = 0;
      m_moveRecord[i].MoveType = CubeRoverPorts::MC_NONE;
    }
    m_currMove = NAV_MOVE_BUFFER_LIMIT;
    m_lastestMove = NAV_MOVE_BUFFER_LIMIT;
    m_distanceToGo = 0;
    m_maxStepSize = 0x1E; // THIS ISN'T IN CM, IT IS 1/3 CMs
    m_tlm_Forward_Disp = 0;
    m_tlm_Backwards_Disp = 0;
    m_tlm_Left_Disp = 0;
    m_tlm_Right_Disp = 0;
    m_Image_Size = 0;
    m_lastCamera = 0;
  }

  /**
   * @brief      Initialize the motor control component
   *
   * @param[in]  queueDepth  The queue depth
   * @param[in]  instance    The instance
   */
  void NavigationComponentImpl :: init(const NATIVE_INT_TYPE queueDepth, const NATIVE_INT_TYPE instance)
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
    // ASK MOM IF WE ARE STILL GOOD TO MOVE

    switch(m_currState)
    {
      case NAV_IDLE:
        Idle_Task();
        break;

      case NAV_ACTIVELY_DRIVING:
#ifdef TESTING
        driveTicks();
#else
        Actively_Drive_Task();
#endif
        break;

      case NAV_IMAGEING:
        Imaging_Task();
        break;

      case NAV_BACKTRACKING:
        Backtracking_Task();
        break;

      case NAV_SERVICE_MOVING:
        Service_Task();
        break;

      default:
        // THIS IS REALLY BAD! 
        // We will just change to the idle state though.
        Change_State(NAV_IDLE);
        break;
    }
  }

  /**
   * @brief      Implementation for Motor Control to pass information back to Navigation
   *
   * @param[in]  portNum            The port number
   * @param[in]  movement_finished  Flag for movement being finished
   */
  void NavigationComponentImpl :: motorDataIn_handler(const NATIVE_INT_TYPE portNum, bool movement_finished)
  {
    // Were we moving?
    if(m_currMoving && movement_finished)
    {
      m_currMoving = false;
      Change_State(NAV_IMAGEING);
    }

    // We don't care otherwise
  }

  /**
   * @brief      Implementation for IMU to pass information back to Navigation
   *
   * @param[in]  portNum            The port number
   * @param[in]  movement_finished  Flag for movement being finished
   */
  void NavigationComponentImpl :: IMUDataIn_handler(const NATIVE_INT_TYPE portNum,
                                                    I16 roll_ptr,
                                                    I16 pitch_ptr,
                                                    I16 yaw_ptr)
  {
    m_roll = roll_ptr;
    m_pitch = pitch_ptr;
    m_yaw = yaw_ptr;
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

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
    // Add it to the record, then transition our state
    if(Create_Move(Distance, Speed, Callback_ID, CubeRoverPorts::MC_Forward))
    {
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
    }
    else
    {
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
    }
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
    // Add it to the record, then transition our state
    if(Create_Move(Distance, Speed, Callback_ID, CubeRoverPorts::MC_Backward))
    {
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
    }
    else
    {
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
    }
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
    // Add it to the record, then transition our state
    if(Create_Move(Distance, Speed, Callback_ID, CubeRoverPorts::MC_Right))
    {
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
    }
    else
    {
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
    }
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
    // Add it to the record, then transition our state
    if(Create_Move(Distance, Speed, Callback_ID, CubeRoverPorts::MC_Left))
    {
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
    }
    else
    {
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
    }
  }

  /**
   * @brief      Implementation for stopping moving command
   *
   * @param[in]  portNum  The port number
   */
  void NavigationComponentImpl :: Nav_StopMovement_cmdHandler(const FwOpcodeType opCode,
                                                              const U32 cmdSeq)
  {
    motorCommandOut_out(0, CubeRoverPorts::MC_DrivingConfiguration, CubeRoverPorts::MC_Stop,0,0);
    if(m_currMoving)
    {
      log_COMMAND_Nav_moveInterrupted();
      motorCommandOut_out(0,
                          CubeRoverPorts::MC_DrivingConfiguration,
                          CubeRoverPorts::MC_Stop,
                          0,
                          0);
      Get_Telemetry();
    }
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  // ----------------------------------------------------------------------
  // Handler implementations for state related functions
  // ----------------------------------------------------------------------

#ifdef TESTING
  bool NavigationComponentImpl :: updateCurrMove()
  {
      if(m_currMove != m_lastestMove)
      {
        if(m_currMove >= (NAV_MOVE_BUFFER_LIMIT - 1))
        {
          m_currMove = 0;
        }

        else
        {
          m_currMove++;
        }

        m_distanceToGo = m_moveRecord[m_currMove].Distance;
        return false;
      }
      return true;
  }
  void NavigationComponentImpl :: driveTicks()
  {
      if (!m_currMoving)
      {
          if(updateCurrMove())
          {
              Change_State(NAV_IDLE);
              return;
          }

          m_currMove = m_lastestMove;
          m_distanceToGo = 200;
          motorCommandOut_out(0,
                              CubeRoverPorts::MC_DrivingConfiguration,
                              m_moveRecord[m_currMove].MoveType,
                              m_distanceToGo,
                              m_moveRecord[m_currMove].Speed);
        //      Add_Telem_Disp(m_moveRecord[m_currMove].MoveType, m_distanceToGo);
          m_distanceToGo = 0;
      }
  }
#endif


  /**
   * @brief      Implementation for changing states
   *
   * @param[in]  New_State  The new driving state
   */
  bool NavigationComponentImpl :: Change_State(NAVState New_State)
  {
    // TODO
    // ASK MOM IF THE NAV STATE CAN CHANGE Return False if we cannot
    // MOM might say we are in service and moves are still okay
    // Then go to service move state.

    // Now change the state
    m_currState = New_State;
    return true;
  }

  /**
   * @brief      Implementation for sitting on our hand
   *
   * @param[in]  None
   */
  void NavigationComponentImpl :: Idle_Task()
  {
    // Something on the queue?
    if(m_lastestMove != m_currMove)
    {
      Change_State(NAV_ACTIVELY_DRIVING);
    }
    return;
  }

  /**
   * @brief      Implementation for handling ground movement commands
   *
   * @param[in]  None
   */
  void NavigationComponentImpl :: Actively_Drive_Task()
  {
    // If we should be driving, but aren't tell the motors to stop being lazy
    if (!m_currMoving)
    {
      // Limit by the step size
      if (m_distanceToGo > m_maxStepSize)
      {
        motorCommandOut_out(0,
                            CubeRoverPorts::MC_DrivingConfiguration,
                            m_moveRecord[m_currMove].MoveType,
                            m_maxStepSize,
                            m_moveRecord[m_currMove].Speed);
        Add_Telem_Disp(m_moveRecord[m_currMove].MoveType, m_maxStepSize);
        m_distanceToGo -= m_maxStepSize;
      }

      // Or just do the whole step
      else if (m_distanceToGo != 0)
      {
        motorCommandOut_out(0,
                            CubeRoverPorts::MC_DrivingConfiguration,
                            m_moveRecord[m_currMove].MoveType,
                            m_distanceToGo,
                            m_moveRecord[m_currMove].Speed);
        Add_Telem_Disp(m_moveRecord[m_currMove].MoveType, m_distanceToGo);
        m_distanceToGo = 0;
      }
      // We have no more distance to go!

      else
      {
        // If we are not on the latest move, we should be
        if(m_currMove != m_lastestMove)
        {
          if(m_currMove >= (NAV_MOVE_BUFFER_LIMIT - 1))
          {
            m_currMove = 0;
          }

          else
          {
            m_currMove++;
          }

          m_distanceToGo = m_moveRecord[m_currMove].Distance;
        }

        else
        {
          // Not sure when this would occur. But this is bad!
          // ...not bad? m_currMoving = True ?
        }
      }
    }

    // If we are moving, get the telemetry instead.
    else
    {
      Get_Telemetry();

      // TODO
      // Check the IMU pitch reading to see if we should stop rotating
    }

    return;
  }

  /**
   * @brief      Implementation for back driving
   *
   * @param[in]  None
   */
  void NavigationComponentImpl :: Backtracking_Task()
  {
    //TODO
    if (!m_currMoving)
    {

    }

    return;
  }

  /**
   * @brief      Implementation for taking nav images
   *
   * @param[in]  None
   */
  void NavigationComponentImpl :: Imaging_Task()
  {
    if(!m_imagingInProgress)
    {
      // Flip the imaging flag, and determine the camera chosen
      m_imagingInProgress = true;
      U8 CameraNum;

      // If we move forward, change to forward camera
      if(m_moveRecord[m_currMove].MoveType == CubeRoverPorts::MC_Forward)
        CameraNum = 0;

      // If backwards, change to back camera
      else if(m_moveRecord[m_currMove].MoveType == CubeRoverPorts::MC_Backward)
        CameraNum = 1;

      // Else, just use the last option
      else
        CameraNum = m_lastCamera;

      m_lastCamera = CameraNum;

      // Low Re option
      if(m_distanceToGo > 0)
      {

        // TODO
        // ASK CAMERA TO TAKE A LOW RES IMAGE
        cameraTakePictureOut_out(0, CameraNum, m_moveRecord[m_currMove].Callback_ID);
        m_Image_Size += NAV_LOW_RES_SIZE;
        tlmWrite_Nav_Image_Size(m_Image_Size);
      }

      // Med Res
      else
      {
        // TODO
        // ASK CAMERA TO TAKE A MED RES IMAGE
        cameraTakePictureOut_out(0, CameraNum, m_moveRecord[m_currMove].Callback_ID);
        m_Image_Size += NAV_MED_RES_SIZE;
        tlmWrite_Nav_Image_Size(m_Image_Size);
      }
        
    }

    else
    {
      // Should be automatic, this should never be called
      // Check camera status. MOM maybe?
    }
  }

  /**
   * @brief      Implementation for taking care of service mode requests
   *
   * @param[in]  None
   */
  void NavigationComponentImpl :: Service_Task()
  {
    return;
  }

  // ----------------------------------------------------------------------
  // Handler implementations for helper functions
  // ----------------------------------------------------------------------

  /**
   * @brief      Implementation for getting telemetry from IMU and MCI
   *
   * @param[in]  None
   */
  void NavigationComponentImpl :: Get_Telemetry()
  {
    // IMU Telemetry Read
    // TODO
    
    // MCI Telemetry Call
    motorCommandOut_out(0, CubeRoverPorts::MC_UpdateTelemetry, CubeRoverPorts::MC_NONE, 0, 0);
  }

  /**
   * @brief      Implementation for adding a move to the queue
   *
   * @param[in]  Distance
   * @param[in]  Speed
   * @param[in]  Callback_ID
   * @param[in]  MoveType
   */
  bool NavigationComponentImpl :: Create_Move(U8 Distance, U8 Speed, U16 Callback_ID, NAVMovementType MoveType)
  {
    // Need to ensure the lastest_move isn't about to overwrite any move
    if ((m_lastestMove + 1 == m_currMove) || (m_lastestMove >= (NAV_MOVE_BUFFER_LIMIT - 1) && m_currMove == 0))
    { 
      log_WARNING_HI_Nav_moveQueueLimit();
      return false;
    }

    // Connect the circular buffer
    if(m_lastestMove >= (NAV_MOVE_BUFFER_LIMIT - 1))
      m_lastestMove = 0;

    // Or just increment through the buffer
    else
      m_lastestMove++;

    // TODO Bound checking
    // Here is where bound checking would be done

    m_moveRecord[m_lastestMove].Distance = Distance;
    m_moveRecord[m_lastestMove].Speed = Speed;
    m_moveRecord[m_lastestMove].Callback_ID = Callback_ID;
    m_moveRecord[m_lastestMove].MoveType = MoveType;

    // If everything looks fine, return success.
    return true;
  }

  /**
   * @brief      Implementation for adding a move to the queue
   *
   * @param[in]  MoveType
   * @param[in]  Distance
   */
  void NavigationComponentImpl :: Add_Telem_Disp(NAVMovementType MoveType, U8 Distance)
  {
    switch(MoveType)
    {
      case CubeRoverPorts::MC_Forward:
        m_tlm_Forward_Disp += Convert_Command_Dist_To_Telem(Distance);
        tlmWrite_Nav_Moved_Forward(m_tlm_Forward_Disp);
        break;

      case CubeRoverPorts::MC_Backward:
        m_tlm_Backwards_Disp += Convert_Command_Dist_To_Telem(Distance);
        tlmWrite_Nav_Moved_Backward(m_tlm_Backwards_Disp);
        break;

      case CubeRoverPorts::MC_Left:
        m_tlm_Left_Disp += Convert_Command_Angle_To_Telem(Distance);
        tlmWrite_Nav_Turned_Left(m_tlm_Left_Disp);
        break;

      case CubeRoverPorts::MC_Right:
        m_tlm_Right_Disp += Convert_Command_Angle_To_Telem(Distance);
        tlmWrite_Nav_Turned_Right(m_tlm_Right_Disp);
        break;

      // If it isn't one of these, just don't do anything
      default:
        break;
    }
  }

  /**
   * @brief      Converts the commanded data type into telemetry data type
   *
   * @param[in]  Distance
   */
  uint16_t NavigationComponentImpl ::  Convert_Command_Dist_To_Telem(uint8_t Distance)
  {
    // TODO
    // Confirm the conversion
    return Distance*3;
  }

  /**
   * @brief      Converts the commanded data type into telemetry data type
   *
   * @param[in]  Angle
   */
  uint16_t NavigationComponentImpl ::  Convert_Command_Angle_To_Telem(uint8_t Angle)
  {
    // TODO
    // Confirm the conversion
    return Angle;
  }

} // end namespace CubeRover
