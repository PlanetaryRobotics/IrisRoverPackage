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
#include <string.h>

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
      // Gyrometer data configuration
      m_gyroDataConfig.CS_HOLD = false;
      m_gyroDataConfig.DFSEL = SPI_FMT_0;
      m_gyroDataConfig.WDEL = false;
      m_gyroDataConfig.CSNR = 0;

      // Accelerometer data configuration
      m_accDataConfig.CS_HOLD = true;
      m_accDataConfig.DFSEL = SPI_FMT_0;
      m_accDataConfig.WDEL = false;
      m_accDataConfig.CSNR = 0;

      m_spi = NULL;
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
    ImuError err;
    float accX = 0.0f;
    float accY = 0.0f;
    float accZ = 0.0f;
    float gyroX = 0.0f;
    float gyroY = 0.0f;
    float gyroZ = 0.0f;

    if((readAccelerations(&accX, &accY, &accZ)) != IMU_NO_ERROR){
        //@todo see how to pass argument to log events
        log_WARNING_HI_Imu_CommunicationFailure();
        return;
    }

    if((readAngularRates(&gyroX, &gyroY, &gyroZ)) != IMU_NO_ERROR){
        //@todo see how to pass argument to log events
        log_WARNING_HI_Imu_CommunicationFailure();
        return;
    }

    tlmWrite_Acc_X(accX);
    tlmWrite_Acc_Y(accY);
    tlmWrite_Acc_Z(accZ);
    tlmWrite_Gyro_X(gyroX);
    tlmWrite_Gyro_Y(gyroY);
    tlmWrite_Gyro_Z(gyroZ);

    // dummy example
    // @TODO compute rover angle here
    if(accY > 0.5){
      log_WARNING_HI_Imu_AngleWarning();
    }
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  /**
   * @brief      { function_description }
   *
   * @param[in]  opCode  The operation code
   * @param[in]  cmdSeq  The command sequence
   */
  void ImuComponentImpl :: Imu_ReportData_cmdHandler( const FwOpcodeType opCode, const U32 cmdSeq){
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }


  /**
   * @brief      { function_description }
   *
   * @param      spi   The spi
   *
   * @return     { description_of_the_return_value }
   */
  ImuError ImuComponentImpl :: setupAccelerometer(spiBASE_t *spi){
    uint16_t deviceId = 0;
    ImuError err;

    if((err = accReadData(Adxl312::AdxlRegister::DEVICE_ID, &deviceId, 1)) != IMU_NO_ERROR)
        return err;

    if(deviceId !=  ADXL_DEVICE_ID){
        return IMU_UNEXPECTED_ERROR;
    }

    //@TODO implement the rest of the configuration
    //@TODO move that to command handling to support turn on and off of the device

    Adxl312::PowerCtlReg powerCtl;
    powerCtl.all = 0;
    powerCtl.bit.measure = 1; // turn on measure
    if((err = accWriteData(Adxl312::AdxlRegister::POWER_CTL, (uint16_t *)&powerCtl.all, 1)) != IMU_NO_ERROR)
        return err;

    Adxl312::DataFormatReg format;
    format.all = 0;
    format.bit.fullRes = 0;
    format.bit.range = Adxl312::RANGE_12G;
    if((err = accWriteData(Adxl312::AdxlRegister::DATA_FORMAT, (uint16_t *)&format.all, 1)) != IMU_NO_ERROR)
        return err;

    Adxl312::FifoCtlReg fifoCtl;
    fifoCtl.all = 0;
    fifoCtl.bit.fifo_mode = Adxl312::BYPASS;
    if((err = accWriteData(Adxl312::AdxlRegister::FIFO_CTL, (uint16_t *)&fifoCtl.all, 1)) != IMU_NO_ERROR)
        return err;

    return IMU_NO_ERROR;
  }

  ImuError ImuComponentImpl :: setupGyroscope(spiBASE_t *spi){
    ImuError err;
    uint16_t deviceId;

    if((err = gyroReadData(L3gd20h::L3gd20hRegister::WHO_AM_I, (uint16_t *)&deviceId, 1)) != IMU_NO_ERROR)
        return err;

    if(deviceId != L3GD20H_DEVICE_ID){
        return IMU_UNEXPECTED_ERROR;
    }

    L3gd20h::FifoCtlReg fifoReg;
    fifoReg.all = 0;
    fifoReg.bit.fifo_mode = 0; // bypass

    if((err = gyroWriteData(L3gd20h::L3gd20hRegister::FIFO_CTL, (uint16_t *)&fifoReg.all, 1)) != IMU_NO_ERROR)
        return err;

    return IMU_NO_ERROR;
  }

  /**
   * @brief      { function_description }
   */
  ImuError ImuComponentImpl :: setup(spiBASE_t *spi){
     ImuError err;

     if(m_setup)
      return IMU_NO_ERROR;

    m_spi = spi;
    m_setup = true;

    err = setupAccelerometer(spi);

    if(err != IMU_NO_ERROR)
        return err;

    err = setupGyroscope(spi);

    if(err != IMU_NO_ERROR)
        return err;

    return IMU_NO_ERROR;
  }


  /**
   * @brief      { function_description }
   *
   * @param[in]  regStartAddr  The register start address
   * @param      rxData        The receive data
   * @param[in]  length        The length
   *
   * @return     { description_of_the_return_value }
   */
  ImuError ImuComponentImpl :: accReadData(const Adxl312::AdxlRegister regStartAddr, uint16_t *rxData, const uint8_t length){

      m_spiTxBuff[0] = (uint8_t) regStartAddr;
      m_spiTxBuff[0] |= 0x40; // multi-bytes read
      m_spiTxBuff[0] |= 0x80; // read

      if(length > SPI_RX_BUFFER_SIZE)
          return IMU_WRONG_DATA_SIZE;

      gioSetBit(spiPORT3, CS_SPIPORT3_BIT_ADXL, 0);
      spiTransmitData(m_spi, &m_accDataConfig, 1, (uint16_t *)&m_spiTxBuff);
      spiReceiveData(m_spi, &m_accDataConfig, length, (uint16_t *)&m_spiRxBuff);
      gioSetBit(spiPORT3, CS_SPIPORT3_BIT_ADXL, 1);

      memcpy(rxData, m_spiRxBuff, length);

      return IMU_NO_ERROR;
  }


  /**
   * @brief      { function_description }
   *
   * @param[in]  regStartAddr  The register start address
   * @param      txData        The transmit data
   * @param[in]  length        The length
   *
   * @return     { description_of_the_return_value }
   */
  ImuError ImuComponentImpl :: accWriteData(const Adxl312::AdxlRegister regStartAddr, uint16_t *txData, const uint8_t length){

      m_spiTxBuff[0] = (uint8_t) regStartAddr;
      m_spiTxBuff[0] |= 0x40; // multibyte write

      // todo return an error
      if(length+1 > SPI_TX_BUFFER_SIZE)
          return IMU_WRONG_DATA_SIZE;

      memcpy(m_spiTxBuff+1, txData, length);

      gioSetBit(spiPORT3, CS_SPIPORT3_BIT_ADXL, 0);
      spiTransmitData(m_spi, &m_accDataConfig, 1+length, (uint16_t *)&m_spiTxBuff);
      gioSetBit(spiPORT3, CS_SPIPORT3_BIT_ADXL, 1);

      return IMU_NO_ERROR;
  }


  /**
   * @brief      Reads accelerations.
   *
   * @param      accX  The acc x
   * @param      accY  The acc y
   * @param      accZ  The acc z
   *
   * @return     { description_of_the_return_value }
   */
  ImuError ImuComponentImpl :: readAccelerations(float32 *accX, float32 *accY,  float32 *accZ){
      uint16_t rxBuffer[6]; // 3 x 2 bytes
      int16_t tmp;
      ImuError err = IMU_NO_ERROR;

      // read all acceleration data
      err = accReadData(Adxl312::AdxlRegister::DATAX0, rxBuffer, sizeof(rxBuffer));

      tmp = rxBuffer[0] | rxBuffer[1] << 8;
      *accX = (float32)(tmp) * 0.0009765625 * ACCELEROMETER_RANGE /*normalize to 1.0 then maximum g range*/;

      tmp = rxBuffer[2] | rxBuffer[3] << 8;
      *accY = (float32)(tmp) * 0.0009765625 * ACCELEROMETER_RANGE;

      tmp = rxBuffer[4] | rxBuffer[5] << 8;
      *accZ = (float32)(tmp) * 0.0009765625 * ACCELEROMETER_RANGE;

      return err;
  }

  // Gyroscope code -------------------------------------------------------------------------

  /**
   * @brief      { function_description }
   *
   * @param[in]  regStartAddr  The register start address
   * @param      rxData        The receive data
   * @param[in]  length        The length
   *
   * @return     { description_of_the_return_value }
   */
  ImuError ImuComponentImpl :: gyroReadData(const L3gd20h::L3gd20hRegister regStartAddr, uint16_t *rxData, const uint8_t length){

    m_spiTxBuff[0] = (uint8_t) regStartAddr;
    m_spiTxBuff[0] |= 0x40; // multi-bytes read
    m_spiTxBuff[0] |= 0x80; // read

    if(length > SPI_RX_BUFFER_SIZE)
        return IMU_WRONG_DATA_SIZE;

    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_L3GD20H, 0);
    spiTransmitData(m_spi, &m_gyroDataConfig, 1, (uint16_t *)&m_spiTxBuff);
    spiReceiveData(m_spi, &m_gyroDataConfig, length, (uint16_t *)&m_spiRxBuff);
    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_L3GD20H, 1);

    memcpy(rxData, m_spiRxBuff, length);

    return IMU_NO_ERROR;
  }

  /**
   * @brief      { function_description }
   *
   * @param[in]  regStartAddr  The register start address
   * @param      txData        The transmit data
   * @param[in]  length        The length
   *
   * @return     { description_of_the_return_value }
   */
  ImuError ImuComponentImpl :: gyroWriteData(const L3gd20h::L3gd20hRegister regStartAddr, uint16_t *txData, const uint8_t length){
    m_spiTxBuff[0] = (uint8_t) regStartAddr;
    m_spiTxBuff[0] |= 0x40; // multibyte write

    if(length+1 > SPI_TX_BUFFER_SIZE)
        return IMU_WRONG_DATA_SIZE;

    memcpy(m_spiTxBuff+1, txData, length);

    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_L3GD20H, 0);
    spiTransmitData(m_spi, &m_gyroDataConfig, 1+length, (uint16_t *)&m_spiTxBuff);
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
   * @return     { description_of_the_return_value }
   */
  ImuError ImuComponentImpl :: readAngularRates(float32 *gyrX, float32 *gyrY,  float32 *gyrZ){
    uint16_t rxBuffer[6]; // 3 x 2 bytes
    int16_t tmp;
    ImuError err = IMU_NO_ERROR;

    // read all acceleration data
    err = gyroReadData(L3gd20h::L3gd20hRegister::OUT_X_L, rxBuffer, sizeof(rxBuffer));

    tmp = rxBuffer[0] | rxBuffer[1] << 8;
    *gyrX = (float32)(tmp) * 0.0009765625 * GYRO_RANGE /*normalize to 1.0 then maximum g range*/;

    tmp = rxBuffer[2] | rxBuffer[3] << 8;
    *gyrY = (float32)(tmp) * 0.0009765625 * GYRO_RANGE;

    tmp = rxBuffer[4] | rxBuffer[5] << 8;
    *gyrZ = (float32)(tmp) * 0.0009765625 * GYRO_RANGE;

    return err;
  }


} // end namespace CubeRover
