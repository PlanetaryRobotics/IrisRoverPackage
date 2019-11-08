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
      m_accDataConfig.CS_HOLD = false;
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
    float accX = 0.0f;
    float accY = 0.0f;
    float accZ = 0.0f;
    float gyroX = 0.0f;
    float gyroY = 0.0f;
    float gyroZ = 0.0f;

    readAccelerations(&accX, &accY, &accZ);

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
  ImuError ImuComponentImpl :: setup(spiBASE_t *spi){
     uint16_t deviceId = 0;
     uint16_t dataFormat = 0;
     ImuError err;

     if(m_setup)
      return IMU_NO_ERROR;

    m_spi = spi;
    m_setup = true;


    err = accReadData(AdxlRegister::DEVICE_ID, &deviceId, 1);

    if(deviceId !=  ADXL_DEVICE_ID){
        return IMU_UNEXPECTED_ERROR;
    }
    //to do implement the rest of the configuration

    return err;
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
  ImuError ImuComponentImpl :: accReadData(const AdxlRegister regStartAddr, uint16_t *rxData, const uint8_t length){

      m_spiTxBuff[0] = (uint8_t) regStartAddr;
      m_spiTxBuff[0] |= SET_ADXL_SPI_MULTITRANS(m_spiTxBuff[0]);
      m_spiTxBuff[0] |= SET_ADXL_SPI_READ_BIT (m_spiTxBuff[0]);

      // todo return an error
      if(length > SPI_RX_BUFFER_SIZE)
          return IMU_WRONG_DATA_SIZE;

      gioSetBit(spiPORT3, 1, 0);
      spiTransmitData(m_spi, &m_accDataConfig, 1, (uint16_t *)&m_spiTxBuff);
      spiReceiveData(m_spi, &m_accDataConfig, length, (uint16_t *)&m_spiRxBuff);
      gioSetBit(spiPORT3, 1, 1);

      memcpy(rxData, m_spiRxBuff + 1 , length);

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
  ImuError ImuComponentImpl :: accWriteData(const AdxlRegister regStartAddr, uint16_t *txData, const uint8_t length){

      m_spiTxBuff[0] = (uint8_t) regStartAddr;
      m_spiTxBuff[0] |= SET_ADXL_SPI_MULTITRANS(m_spiTxBuff[0]);
      m_spiTxBuff[0] |= SET_ADXL_SPI_READ_BIT (m_spiTxBuff[0]);

      // todo return an error
      if(length+1 > SPI_TX_BUFFER_SIZE)
          return IMU_UNEXPECTED_ERROR;

      memcpy(m_spiTxBuff+1, txData, length);

      spiTransmitData(m_spi, &m_accDataConfig, 1+length, (uint16_t *)&m_spiTxBuff);

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
      uint16_t tmp;
      ImuError err = IMU_NO_ERROR;

      // read all acceleration data
      err = accReadData(AdxlRegister::DATAX0, rxBuffer, sizeof(rxBuffer));

      tmp = rxBuffer[0] + rxBuffer[1] << 8;
      *accX = (float32)(tmp) * 0.0009765625 * 12.0f /*normalize to 1.0 then maximum g range*/;

      tmp = rxBuffer[2] + rxBuffer[3] << 8;
      *accY = (float32)(tmp) * 0.0009765625 * 12.0f;

      tmp = rxBuffer[4] + rxBuffer[5] << 8;
      *accZ = (float32)(tmp) * 0.0009765625 * 12.0f;

      return err;
  }


} // end namespace CubeRover
