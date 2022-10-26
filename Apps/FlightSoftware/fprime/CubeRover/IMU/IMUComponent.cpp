// ======================================================================
// \title  ImuComponentImpl.cpp
// @author     cedric
// @editted    michael
// @brief      cpp file for Imu component implementation class
//
// @copyright  Copyright 2009-2015, by the California Institute of Technology.
//             ALL RIGHTS RESERVED.  United States Government Sponsorship
//             acknowledged.
//
// ======================================================================


#include <CubeRover/IMU/IMUComponent.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include <string.h>
#include <math.h>

#define ONE_OVER_PI                   0.3183098861837


namespace CubeRover {

  /**
   * @brief      Constructs a new instance.
   *
   * @param[in]  compName  The component name
   */
  IMUComponentImpl ::
#if FW_OBJECT_NAMES == 1
    IMUComponentImpl(
        const char *const compName
    ) :
      IMUComponentBase(compName)
#else
      IMUComponentImpl(void)
#endif
  {

    m_lpfAccX = 0.0;
    m_lpfAccY = 0.0;
    m_lpfAccZ = 0.0;
  }


  /**
   * @brief      Initialize the f-prime component
   *
   * @param[in]  queueDepth  The queue depth
   * @param[in]  instance    The instance
   */
  void IMUComponentImpl :: init(const NATIVE_INT_TYPE instance)
  {
    IMUComponentBase::init(instance);
    accel_init();
    gyro_init();
  }


  /**
   * @brief      Destroys the object.
   */
  IMUComponentImpl :: ~IMUComponentImpl(void)
  {

  }

  /**
   * @brief      Handler implementations for user-defined typed input ports
   *
   * @param[in]  portNum  The port number
   * @param[in]  context  The context
   */
  void IMUComponentImpl :: schedIn_handler(const NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context)
  {
    float roll = 0.0f;
    float pitch = 0.0f;
    // float yaw = 0.0f;  TODO: Is this going to be used????

    if (accel_get_data(&m_acc) != ACCEL_STATUS_OK) {
        log_WARNING_HI_IMU_CommunicationFailure();
        return;
    }

    if (gyro_get_data(&m_gyro) != GYRO_STATUS_OK) {
        log_WARNING_HI_IMU_CommunicationFailure();
        return;
    }

    tlmWrite_X_Acc(m_acc.raw[0]);
    tlmWrite_Y_Acc(m_acc.raw[1]);
    tlmWrite_Z_Acc(m_acc.raw[2]);
    tlmWrite_X_Ang(m_gyro.raw[0]);
    tlmWrite_Y_Ang(m_gyro.raw[1]);
    tlmWrite_Z_Ang(m_gyro.raw[2]);

    computePitchRoll(&pitch, &roll);

    if(fabs(pitch) > MAX_ROVER_PITCH_ANGLE || fabs(roll) > MAX_ROVER_ROLL_ANGLE)
    {
//        log_WARNING_HI_IMU_AngleWarning();
        assert(true);
    }
  }

  /**
   * @brief      PingIn handler implementations
   *
   * @param[in]  portNum  The port number
   * @param[in]  key      Value to return to pinger
   */
  void IMUComponentImpl :: PingIn_handler(const NATIVE_INT_TYPE portNum, U32 key)
  {
    this->PingOut_out(portNum, key);
  }

  /**
   * @brief      Command handler implementations
   *
   * @param[in]  opCode  The operation code
   * @param[in]  cmdSeq  The command sequence
   */
  void IMUComponentImpl :: IMU_ReportData_cmdHandler( const FwOpcodeType opCode, const U32 cmdSeq)
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Command handler implementations
   *
   * @param[in]  opCode  The operation code
   * @param[in]  cmdSeq  The command sequence
   */
  void IMUComponentImpl :: IMU_TurnOn_cmdHandler(const FwOpcodeType opCode,
                                                 const U32 cmdSeq)
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  /**
   * @brief      Command handler implementations
   *
   * @param[in]  opCode  The operation code
   * @param[in]  cmdSeq  The command sequence
   */
  void IMUComponentImpl :: IMU_TurnOff_cmdHandler(const FwOpcodeType opCode,
                                                  const U32 cmdSeq)
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }
  
  /**
   * @brief      Calculates the pitch roll. See example of implementation in
   *             cache.freescale.com/files/sensors/doc/app_note/AN361.pdf
   *
   * @param      pitch  The pitch
   * @param      roll   The roll
   * @param[in]  accX   The acc x
   * @param[in]  accY   The acc y
   * @param[in]  accZ   The acc z
   */
  void IMUComponentImpl::computePitchRoll(float32 *pitch, float32 *roll) {
    float accX = m_acc.acc[0];
    float accY = m_acc.acc[1];
    float accZ = m_acc.acc[2];
    
    // Low-pass filter
    m_lpfAccX = accX * IMU_LPF_COEFF +  m_lpfAccX * (1.0 - IMU_LPF_COEFF);
    m_lpfAccY = accY * IMU_LPF_COEFF +  m_lpfAccY * (1.0 - IMU_LPF_COEFF);
    m_lpfAccZ = accZ * IMU_LPF_COEFF +  m_lpfAccZ * (1.0 - IMU_LPF_COEFF);

    // Calculation of the pitch
    *pitch = atan2(m_lpfAccY, sqrtf(m_lpfAccX*m_lpfAccX + m_lpfAccZ*m_lpfAccZ)) * 180.0f * ONE_OVER_PI;

    // Calculation of the roll
    *roll = atan2(-m_lpfAccX, m_lpfAccZ) * 180.0f * ONE_OVER_PI;
  }

} // end namespace CubeRover
