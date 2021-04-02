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

    // Single-byte Gyrometer data configuration
    m_gyroDataSingleByteConfig.CS_HOLD = false;
    m_gyroDataSingleByteConfig.DFSEL = SPI_FMT_2;
    m_gyroDataSingleByteConfig.WDEL = false;
    m_gyroDataSingleByteConfig.CSNR = 0;

    // Double-byte Gyrometer data configuration
    m_gyroDataDoubleByteConfig.CS_HOLD = false;
    m_gyroDataDoubleByteConfig.DFSEL = SPI_FMT_3;
    m_gyroDataDoubleByteConfig.WDEL = false;
    m_gyroDataDoubleByteConfig.CSNR = 0;
      
    m_spi = NULL;

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
    // setupGyroscope(spi);
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
    // Danger floats!
    float gyroX = 0.0f;
    float gyroY = 0.0f;
    float gyroZ = 0.0f;
    float roll = 0.0f;
    float pitch = 0.0f;
    // float yaw = 0.0f;  TODO: Is this going to be used????

    if(accel_get_data(&m_acc) != ACCEL_STATUS_OK)
    {
        log_WARNING_HI_IMU_CommunicationFailure();
        return;
    }

    /*if((readAngularRates(&gyroX, &gyroY, &gyroZ)) != IMU_NO_ERROR)
    {
        log_WARNING_HI_IMU_CommunicationFailure();
        return;
    }*/

    tlmWrite_X_Acc(m_acc.raw[0]);
    tlmWrite_Y_Acc(m_acc.raw[1]);
    tlmWrite_Z_Acc(m_acc.raw[2]);
    /*tlmWrite_X_Ang(gyroX);
    tlmWrite_Y_Ang(gyroY);
    tlmWrite_Z_Ang(gyroZ);*/

    computePitchRoll(&pitch, &roll);

    if(fabs(pitch) > MAX_ROVER_PITCH_ANGLE || fabs(roll) > MAX_ROVER_ROLL_ANGLE)
    {
      log_WARNING_HI_IMU_AngleWarning();
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
  void IMUComponentImpl :: computePitchRoll(float32 *pitch,
                                            float32 *roll)
  {
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

  /**
   * @brief      Setup the gyroscope operating parameters
   *
   * @param      spi   The hercules spi register address
   *
   * @return     error code specific to the IMU
   */
  IMUError IMUComponentImpl :: setupGyroscope(spiBASE_t *spi)
  {
    IMUError err = IMU_NO_ERROR;
    uint16_t deviceId;
    L3gd20h::FifoCtlReg fifoReg;
    L3gd20h::Ctl1Reg ctl1Reg;
    L3gd20h::Ctl4Reg ctl4Reg;

    if(spi == NULL){
      return IMU_UNEXPECTED_ERROR;
    }

    // Check that the gyro is functioning by reading WHO_AM_I register
    err = gyroReadData(L3gd20h::L3gd20hRegister::WHO_AM_I, (uint16_t *)&deviceId, 1);
    if(err != IMU_NO_ERROR)
        return err;

    if(deviceId != L3GD20H_DEVICE_ID){
        return IMU_UNEXPECTED_ERROR;
    }

    fifoReg.all = 0;  //clear register
    fifoReg.bit.fifo_mode = L3gd20h::FifoMode::BYPASS; // configure FIFO buffer to be by-passed, collect instant data
    err = gyroWriteData(L3gd20h::L3gd20hRegister::FIFO_CTL, (uint16_t *)&fifoReg.all, 1);

    if(err != IMU_NO_ERROR)
        return err;

    ctl1Reg.all = 0; //clear register
    ctl1Reg.bit.pd = 1; // enable normal mode
    ctl1Reg.bit.xen = 1; // enable x-axis
    ctl1Reg.bit.yen = 1; // enable y-axis
    ctl1Reg.bit.zen = 1; // enable z-axis
    err = gyroWriteData(L3gd20h::L3gd20hRegister::CTRL1, (uint16_t *)&ctl1Reg.all, 1);

    if(err != IMU_NO_ERROR)
        return err;

    ctl4Reg.all = 0;    // clear register
    ctl4Reg.bit.fs = SPI_GYRO_RANGE; // range detection, check ImuComponentImpl.hpp for value
    err = gyroWriteData(L3gd20h::L3gd20hRegister::CTRL4, (uint16_t *)&ctl4Reg.all, 1);

    return err;
  }

  // Gyroscope code
  // -------------------------------------------------------------------------

  /**
   * @brief      Read some data from gyroscope register(s)
   *
   * @param[in]  regStartAddr  The register start address
   * @param      rxData        The receive data
   * @param[in]  length        The length
   *
   * @return     error code specific to the IMU
   */
  IMUError IMUComponentImpl :: gyroReadData(const L3gd20h::L3gd20hRegister regStartAddr, uint16_t *rxData, 
                                            const uint8_t length)
  {
    if(rxData == NULL){
      return IMU_UNEXPECTED_ERROR;
    }

    m_spiTxBuff[0] = (uint8_t) regStartAddr;
    m_spiTxBuff[0] |= 0x40; // multi-bytes read
    m_spiTxBuff[0] |= 0x80; // read

    if(length > SPI_RX_BUFFER_SIZE)
        return IMU_WRONG_DATA_SIZE;

    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_L3GD20H, 0);
    spiTransmitData(m_spi, &m_gyroDataSingleByteConfig, 1, (uint16_t *)&m_spiTxBuff);
    spiReceiveData(m_spi, &m_gyroDataSingleByteConfig, length, (uint16_t *)&m_spiRxBuff);
    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_L3GD20H, 1);

    memcpy(rxData, m_spiRxBuff, length);

    return IMU_NO_ERROR;
  }

  /**
   * @brief      Write some data to gyroscope register(s)
   *
   * @param[in]  regStartAddr  The register start address
   * @param      txData        The transmit data
   * @param[in]  length        The length
   *
   * @return     error code specific to the IMU
   */
  IMUError IMUComponentImpl :: gyroWriteData(const L3gd20h::L3gd20hRegister regStartAddr, uint16_t *txData, 
                                             const uint8_t length)
  {
    if(txData == NULL)
      return IMU_UNEXPECTED_ERROR;

    m_spiTxBuff[0] = (uint8_t) regStartAddr;
    //m_spiTxBuff[0] |= 0x40; // multibyte write

    if(length+1 > SPI_TX_BUFFER_SIZE)
        return IMU_WRONG_DATA_SIZE;

    memcpy(m_spiTxBuff+1, txData, length);

    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_L3GD20H, 0);
    spiTransmitData(m_spi, &m_gyroDataSingleByteConfig, length+1, (uint16_t *)&m_spiTxBuff);
    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_L3GD20H, 1);

    return IMU_NO_ERROR;
  }


  /**
   * @brief      Reads angular rates.
   *
   * @param      gyrX  The gyr x
   * @param      gyrY  The gyr y
   * @param      gyrZ  The gyr z
   *
   * @return     error code specific to the IMU
   */
  IMUError IMUComponentImpl :: readAngularRates(float32 *gyrX, float32 *gyrY,  float32 *gyrZ)
  {
    uint16_t rxBuffer[6]; // 3 x 2 bytes
    int16_t tmp;
    IMUError err = IMU_NO_ERROR;

    // read all angular rates data for x y z axis
    err = gyroReadData(L3gd20h::L3gd20hRegister::OUT_X_L, rxBuffer, sizeof(rxBuffer));

    tmp = rxBuffer[0] | rxBuffer[1] << 8;
    *gyrX = (float32)(tmp) * GYRO_SENSITIVITY /*normalize to 1.0 then maximum g range*/;

    tmp = rxBuffer[2] | rxBuffer[3] << 8;
    *gyrY = (float32)(tmp) * GYRO_SENSITIVITY;

    tmp = rxBuffer[4] | rxBuffer[5] << 8;
    *gyrZ = (float32)(tmp) * GYRO_SENSITIVITY;

    return err;
  }
} // end namespace CubeRover
