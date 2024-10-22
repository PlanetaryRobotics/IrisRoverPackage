// ======================================================================
// \title   ImuComponentImpl.hpp
// \author  cedric
// \editted michael
// \brief   hpp file for Imu component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef IMU_HPP
#define IMU_HPP

#include <stdlib.h>
#include "CubeRover/IMU/IMUComponentAc.hpp"
#include "spi.h"
#include "gio.h"

#include "adxl312.h"
#include "l3gd20h.h"

#include <Os/Mutex.hpp>

// TODO: Make these configureable in flight
#define IMU_LPF_COEFF 0.01
#define MAX_ROVER_PITCH_ANGLE 40
#define MAX_ROVER_ROLL_ANGLE 40

namespace CubeRover
{

  class IMUComponentImpl : public IMUComponentBase
  {
  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object Imu
    //!
    IMUComponentImpl(
#if FW_OBJECT_NAMES == 1
        const char *const compName /*!< The component name*/
#else
        void
#endif
    );

    //! Initialize object Imu
    //!
    void init(
        const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    //! Destroy object Imu
    //!
    ~IMUComponentImpl(void);

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for schedIn
    //!
    void schedIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                         NATIVE_UINT_TYPE context /*!< The call order*/);

    //! Handler implementation for PingIn
    //!
    void PingIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                        U32 key /*!< Value to return to pinger*/);

    // ----------------------------------------------------------------------
    // Command handler implementations
    // ----------------------------------------------------------------------

    //! Implementation for Imu_ReportData command handler
    //! A command to force an IMU data.
    void IMU_ReportData_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                   const U32 cmdSeq /*!< The command sequence number*/);

    //! Implementation for IMU_TurnOn command handler
    //! A command to turn on the hardware
    void IMU_TurnOn_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                               const U32 cmdSeq /*!< The command sequence number*/);

    //! Implementation for IMU_TurnOff command handler
    //! A command to turn off the hardware
    void IMU_TurnOff_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                const U32 cmdSeq /*!< The command sequence number*/);

  private:
    void computePitchRoll(float32 *pitch, float32 *roll);

    struct AccVector m_acc;
    struct GyroVector m_gyro;
    float32 m_lpfAccX;
    float32 m_lpfAccY;
    float32 m_lpfAccZ;

    // Mutex allowing external agents to safely access IMU data while
    // circumventing FPrime:
    ::Os::Mutex m_extMutex;
    // Protected, externally accessible data:
    // Raw accelerometer data:
    int16_t m_extAccRaw[3];

  public:
    // Getters and Setters for Mutex-protected data:
    void setExtAccRaw(int16_t *srcAccRaw);
    void getExtAccRaw(int16_t *dstAccRaw);
  };

} // end namespace CubeRover

#endif
