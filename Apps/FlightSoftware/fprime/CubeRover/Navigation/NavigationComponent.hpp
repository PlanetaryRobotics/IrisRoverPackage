// ======================================================================
// \title  NavigationComponentImpl.hpp
// \author michael
// \brief  hpp file for Navigation component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Navigation_HPP
#define Navigation_HPP

#include "CubeRover/Navigation/NavigationComponentAc.hpp"

namespace CubeRover {

  class NavigationComponentImpl :
    public NavigationComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object Navigation
      //!
      NavigationComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object Navigation
      //!
      void init(const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
                const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/);

      //! Destroy object Navigation
      //!
      ~NavigationComponentImpl(void);

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for PingIn
      //!
      void PingIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                          U32 key /*!< Value to return to pinger*/);

      //! Handler implementation for schedIn
      //!
      void schedIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                           NATIVE_UINT_TYPE context /*!< The call order*/);

      //! Handler implementation for MotorDataOut
      //!
      void motorDataIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                bool movement_finished /*!<  A simple boolean to tell Nav motion is finished */);

      //! Handler implementation for IMUDataOut
      //!
      void IMUDataIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                              I16 x_acc, 
                              I16 y_acc, 
                              I16 z_acc, 
                              I16 x_gyro, 
                              I16 y_gyro, 
                              I16 z_gyro);

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for Nav_DriveForward command handler
      //! Forward movement command
      void Nav_DriveForward_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                       const U32 cmdSeq, /*!< The command sequence number*/
                                       U8 Distance, /*!< 0x00 - 0x5A meaning 0-30 cm*/
                                       U8 Speed, /*!< 0x00 - 0x0A meaning 0-10 cm/s*/
                                       U16 Callback_ID);

      //! Implementation for Nav_DriveBackwards command handler
      //! Backward movement command
      void Nav_DriveBackwards_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                         const U32 cmdSeq, /*!< The command sequence number*/
                                         U8 Distance, /*!< 0x00 - 0x5A meaning 0-30 cm*/
                                         U8 Speed, /*!< 0x00 - 0x0A meaning 0-10 cm/s*/
                                         U16 Callback_ID);

      //! Implementation for Nav_RotateRight command handler
      //! Clockwise rotation command
      void Nav_RotateRight_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                      const U32 cmdSeq, /*!< The command sequence number*/
                                      U8 Distance, /*!< 0x00 - 0x5A meaning 0-90 deg*/
                                      U8 Speed, /*!< 0x00 - 0x0A meaning 0-10 deg/s*/
                                      U16 Callback_ID);

      //! Implementation for Nav_RotateLeft command handler
      //! Counter-clockwise rotation command
      void Nav_RotateLeft_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                     const U32 cmdSeq, /*!< The command sequence number*/
                                     U8 Distance, /*!< 0x00 - 0x5A meaning 0-90 deg*/
                                     U8 Speed, /*!< 0x00 - 0x0A meaning 0-10 deg/s*/
                                     U16 Callback_ID);

      //! Implementation for Nav_StopMovement command handler
      //! Stops all movement
      void Nav_StopMovement_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                       const U32 cmdSeq /*!< The command sequence number*/);
    };

} // end namespace CubeRover

#endif
