// ======================================================================
// \title  ImuComponentImpl.cpp
// \author cedric
// \brief  cpp file for Imu component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/IMU/ImuComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  ImuComponentImpl ::
#if FW_OBJECT_NAMES == 1
    ImuComponentImpl(
        const char *const compName
    ) :
      ImuComponentBase(compName)
#else
    ImuComponentImpl(void)
#endif
  {
    m_setup = false;
  }

  void ImuComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    ImuComponentBase::init(queueDepth, instance);
  }

  ImuComponentImpl ::
    ~ImuComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void ImuComponentImpl :: schedIn_handler(const NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context){
    float accX = 0.0f;
    float accY = 0.0f;
    float accZ = 0.0f;
    float gyroX = 0.0f;
    float gyroY = 0.0f;
    float gyroZ = 0.0f;

    tlmWrite_Acc_X(accX);
    tlmWrite_Acc_Y(accY);
    tlmWrite_Acc_Z(accZ);
    tlmWrite_Gyro_X(gyroX);
    tlmWrite_Gyro_Y(gyroY);
    tlmWrite_Gyro_Z(gyroZ);

    if(accY > 0.5){
      log_WARNING_HI_Imu_AngleWarning();
    }
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void ImuComponentImpl :: Imu_ReportData_cmdHandler( const FwOpcodeType opCode, const U32 cmdSeq){
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }


  /**
   * @brief      { function_description }
   */
  void ImuComponentImpl :: setup(){
    if(m_setup) 
      return;

    m_setup = true;
  }

  /**
   * @brief      { function_description }
   *
   * @param      i2c     I 2 c
   * @param[in]  sadd    The sadd
   * @param[in]  length  The length
   * @param      data    The data
   *
   * @return     { description_of_the_return_value }
   */
  ImuError ImuComponentImpl :: i2cMasterTransmit(i2cBASE_t *i2c,
                                                const ImuI2cSlaveAddress sadd,
                                                const uint32_t length,
                                                uint8_t * data){
    if(i2c == NULL)
      return IMU_UNEXPECTED_ERROR;

    if(data == NULL)
      return IMU_UNEXPECTED_ERROR;

    /* Configure address of Slave to talk to */
    i2cSetSlaveAdd(i2c, sadd);

    /* Set direction to Transmitter */
    i2cSetDirection(i2c, I2C_TRANSMITTER);

    /* Configure Data count */
    i2cSetCount(i2c, length);

    /* Set mode as Master */
    i2cSetMode(i2c, I2C_MASTER);

    /* Set Stop after programmed Count */
    i2cSetStop(i2c);

    /* Transmit Start Condition */
    i2cSetStart(i2c);

    /* Transmit DATA_COUNT number of data in Polling mode */
    i2cSend(i2c, length, data);

    /* Wait until Bus Busy is cleared */
    while(i2cIsBusBusy(i2c) == true);

    /* Wait until Stop is detected */
    while(i2cIsStopDetected(i2c) == 0);

    /* Clear the Stop condition */
    i2cClearSCD(i2c);

    return IMU_NO_ERROR;
  }


   /**
    * @brief      { function_description }
    *
    * @param      i2c     I 2 c
    * @param[in]  sadd    The sadd
    * @param[in]  length  The length
    * @param      data    The data
    *
    * @return     { description_of_the_return_value }
    */
   ImuError ImuComponentImpl :: i2cMasterReceive(i2cBASE_t *i2c,
                                                const ImuI2cSlaveAddress sadd,
                                                const uint32_t length,
                                                uint8_t * data){
    if(i2c == NULL)
      return IMU_UNEXPECTED_ERROR;

    if(data == NULL)
      return IMU_UNEXPECTED_ERROR;

    /* Configure address of Slave to talk to */
    i2cSetSlaveAdd(i2c, sadd);

    /* Set direction to receiver */
    i2cSetDirection(i2c, I2C_RECEIVER);

    /* Configure Data count */
    i2cSetCount(i2c, length);

    /* Set mode as Master */
    i2cSetMode(i2c, I2C_MASTER);

    /* Set Stop after programmed Count */
    i2cSetStop(i2c);

    /* Transmit Start Condition */
    i2cSetStart(i2c);

    /* Transmit DATA_COUNT number of data in Polling mode */
    i2cReceive(i2c, length, data);

    /* Wait until Bus Busy is cleared */
    // TODO add timeout
    while(i2cIsBusBusy(i2c) == true);

    /* Wait until Stop is detected */
    while(i2cIsStopDetected(i2c) == 0);

    /* Clear the Stop condition */
    i2cClearSCD(i2c);

    return IMU_NO_ERROR;
  }

} // end namespace CubeRover
