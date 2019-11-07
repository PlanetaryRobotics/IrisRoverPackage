// ======================================================================
// \title  ImuComponentImpl.hpp
// \author cedric
// \brief  hpp file for Imu component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Imu_HPP
#define Imu_HPP

#include "CubeRover/IMU/IMUComponentAc.hpp"
#include "i2c.h"

namespace CubeRover {

  typedef enum{
    IMU_NO_ERROR,
    IMU_UNEXPECTED_ERROR,
    IMU_MAX_ENUM_SIZE
  }ImuError;

  typedef uint8_t ImuI2cSlaveAddress;

  class ImuComponentImpl :
    public ImuComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object Imu
      //!
      ImuComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object Imu
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object Imu
      //!
      ~ImuComponentImpl(void);

      ImuError i2cMasterTransmit(i2cBASE_t *i2c, const ImuI2cSlaveAddress sadd, const uint32_t length, uint8_t * data);
      ImuError i2cMasterReceive(i2cBASE_t *i2c, const ImuI2cSlaveAddress sadd, const uint32_t length, uint8_t * data);
      void setup();
      
    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for schedIn
      //!
      void schedIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for Imu_ReportData command handler
      //! A command to force an IMU data.
      void Imu_ReportData_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

    private:
      bool m_setup;


    };

} // end namespace CubeRover

#endif
