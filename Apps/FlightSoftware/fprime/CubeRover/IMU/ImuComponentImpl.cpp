// ======================================================================
// \title  ImuComponentImpl.cpp
// @author     cedric
// @brief      cpp file for Imu component implementation class
//
// @copyright  Copyright 2009-2015, by the California Institute of Technology.
//             ALL RIGHTS RESERVED.  United States Government Sponsorship
//             acknowledged.
//
// ======================================================================


#include <CubeRover/IMU/ImuComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include <string.h>
#include <math.h>

namespace CubeRover {

  /**
   * @brief      Constructs a new instance.
   *
   * @param[in]  compName  The component name
   */
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
  void ImuComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    ImuComponentBase::init(queueDepth, instance);
  }


  /**
   * @brief      Destroys the object.
   */
  ImuComponentImpl ::
    ~ImuComponentImpl(void)
  {

  }

  /**
   * @brief      Handler implementations for user-defined typed input ports
   *
   * @param[in]  portNum  The port number
   * @param[in]  context  The context
   */
  void ImuComponentImpl :: schedIn_handler(const NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context){

    float accX = 0.0f;
    float accY = 0.0f;
    float accZ = 0.0f;
    float gyroX = 0.0f;
    float gyroY = 0.0f;
    float gyroZ = 0.0f;
    float roll = 0.0f;
    float pitch = 0.0f;

    if((readAccelerations(&accX, &accY, &accZ)) != IMU_NO_ERROR){
        // @todo       see how to pass argument to log events
        log_WARNING_HI_Imu_CommunicationFailure();
        return;
    }

    if((readAngularRates(&gyroX, &gyroY, &gyroZ)) != IMU_NO_ERROR){
        log_WARNING_HI_Imu_CommunicationFailure();
        return;
    }

    tlmWrite_Acc_X(accX);
    tlmWrite_Acc_Y(accY);
    tlmWrite_Acc_Z(accZ);
    tlmWrite_Gyro_X(gyroX);
    tlmWrite_Gyro_Y(gyroY);
    tlmWrite_Gyro_Z(gyroZ);

    computePitchRoll(&pitch, &roll, accX, accY, accZ);

    tlmWrite_Pitch(pitch);
    tlmWrite_Roll(roll);

    if(fabs(pitch) > MAX_ROVER_PITCH_ANGLE || fabs(roll) > MAX_ROVER_ROLL_ANGLE){
      log_WARNING_HI_Imu_AngleWarning();
    }
  }

  /**
   * @brief      Command handler implementations
   *
   * @param[in]  opCode  The operation code
   * @param[in]  cmdSeq  The command sequence
   */
  void ImuComponentImpl :: Imu_ReportData_cmdHandler( const FwOpcodeType opCode, const U32 cmdSeq){
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
  void ImuComponentImpl :: computePitchRoll(float32 *pitch,
                                            float32 *roll,
                                            const float32 accX,
                                            const float32 accY,
                                            const float32 accZ){

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
   * @brief      Setup the accelerometer operating parameters
   *
   * @param      spi   The hercules spi register address
   *
   * @return     error code specific to the IMU
   */
  ImuError ImuComponentImpl :: setupAccelerometer(spiBASE_t *spi){
    uint16_t deviceId = 0;
    Adxl312::PowerCtlReg powerCtl;
    Adxl312::DataFormatReg format;
    Adxl312::FifoCtlReg fifoCtl;
    ImuError err = IMU_NO_ERROR;

    if(spi == NULL){
      return IMU_UNEXPECTED_ERROR; 
    }


    err = accReadData(Adxl312::AdxlRegister::DEVICE_ID, &deviceId, 1);

    if(err != IMU_NO_ERROR)
        return err;

    if(deviceId !=  ADXL_DEVICE_ID){
        return IMU_UNEXPECTED_ERROR;
    }

    // @TODO implement the rest of the configuration @TODO move that to command
    // handling to support turn on and off of the device

    // Configure the Adxl312 accelerometer power control register Consult
    // Adxl312 datasheet for further details.
    powerCtl.all = 0;         // clear register
    powerCtl.bit.measure = 1; // turn on measure mode

    err = accWriteData(Adxl312::AdxlRegister::POWER_CTL, (uint16_t *)&powerCtl.all, 1);
      return err;

    if(err != IMU_NO_ERROR)
      return err;

    // Configure the Adxl312 accelerometer data format register Consult Adxl312
    // datasheet for further details.
    format.all = 0;         // clear register
    format.bit.fullRes = 0; // report accelerometer data in a 10-bit resolution
    format.bit.range = Adxl312::RANGE_12G; // use +/-12g range
    err = accWriteData(Adxl312::AdxlRegister::DATA_FORMAT, (uint16_t *)&format.all, 1);
    if(err != IMU_NO_ERROR)
        return err;

    // Configure the Adxl312 accelerometer FIFO register Consult Adxl312
    // datasheet for further details.
    fifoCtl.all = 0; // clear register
    fifoCtl.bit.fifo_mode = Adxl312::BYPASS; // configure FIFO buffer to be by-passed, only collect instant data
    err = accWriteData(Adxl312::AdxlRegister::FIFO_CTL, (uint16_t *)&fifoCtl.all, 1);

    return err;
  }


  /**
   * @brief      Setup the gyroscope operating parameters
   *
   * @param      spi   The hercules spi register address
   *
   * @return     error code specific to the IMU
   */
  ImuError ImuComponentImpl :: setupGyroscope(spiBASE_t *spi){
    ImuError err = IMU_NO_ERROR;
    uint16_t deviceId;
    L3gd20h::FifoCtlReg fifoReg;

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
    return err;
  }

  /**
   * @brief      Setup the accelerometer and gyroscope
   *
   * @param      spi   The hercules spi register address
   *
   * @return     error code specific to the IMU
   */
  ImuError ImuComponentImpl :: setup(spiBASE_t *spi){
    ImuError err = IMU_NO_ERROR;

    if(spi == NULL){
      return IMU_UNEXPECTED_ERROR;
    }

    if(m_setup)
      return err;

    m_spi = spi;
    m_setup = true;

    err = setupAccelerometer(spi);

    if(err != IMU_NO_ERROR)
      return err;

    err = setupGyroscope(spi);
    return err;
  }


  /**
   * @brief      Read some data from the accelerometer register(s)
   *
   * @param[in]  regStartAddr  The register start address
   * @param      rxData        The receive data
   * @param[in]  length        The length
   *
   * @return     error code specific to the IMU
   */
  ImuError ImuComponentImpl :: accReadData(const Adxl312::AdxlRegister regStartAddr, uint16_t *rxData, 
                                           const uint8_t length){

    if(rxData == NULL){
      return IMU_UNEXPECTED_ERROR;
    }

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
   * @brief      Write some data to the accelerometer register(s)
   *
   * @param[in]  regStartAddr  The register start address
   * @param      txData        The transmit data
   * @param[in]  length        The length
   *
   * @return     error code specific to the IMU
   */
  ImuError ImuComponentImpl :: accWriteData(const Adxl312::AdxlRegister regStartAddr, uint16_t *txData, 
                                            const uint8_t length){

    if(txData == NULL){
      return IMU_UNEXPECTED_ERROR;
    }

    m_spiTxBuff[0] = (uint8_t) regStartAddr;
    m_spiTxBuff[0] |= 0x40; // multibyte write

    if(length+1 > SPI_TX_BUFFER_SIZE)
        return IMU_WRONG_DATA_SIZE;

    memcpy(m_spiTxBuff+1, txData, length);

    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_ADXL, 0);
    spiTransmitData(m_spi, &m_accDataConfig, length+1, (uint16_t *)&m_spiTxBuff);
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
   * @return     error code specific to the IMU
   */
  ImuError ImuComponentImpl :: readAccelerations(float32 *accX, float32 *accY,  float32 *accZ){

    uint16_t rxBuffer[6]; // 3 x 2 bytes
    int16_t tmp;
    ImuError err = IMU_NO_ERROR;

    // read all acceleration data
    err = accReadData(Adxl312::AdxlRegister::DATAX0, rxBuffer, sizeof(rxBuffer));

    tmp = rxBuffer[0] | rxBuffer[1] << 8;
    *accX = (float32)(tmp) * ONE_OVER_1024 * ACCELEROMETER_RANGE /*normalize to 1.0 then maximum g range*/;

    tmp = rxBuffer[2] | rxBuffer[3] << 8;
    *accY = (float32)(tmp) * ONE_OVER_1024 * ACCELEROMETER_RANGE;

    tmp = rxBuffer[4] | rxBuffer[5] << 8;
    *accZ = (float32)(tmp) * ONE_OVER_1024 * ACCELEROMETER_RANGE;

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
  ImuError ImuComponentImpl :: gyroReadData(const L3gd20h::L3gd20hRegister regStartAddr, uint16_t *rxData, 
                                            const uint8_t length){

    if(rxData == NULL){
      return IMU_UNEXPECTED_ERROR;
    }

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
   * @brief      Write some data to gyroscope register(s)
   *
   * @param[in]  regStartAddr  The register start address
   * @param      txData        The transmit data
   * @param[in]  length        The length
   *
   * @return     error code specific to the IMU
   */
  ImuError ImuComponentImpl :: gyroWriteData(const L3gd20h::L3gd20hRegister regStartAddr, uint16_t *txData, 
                                             const uint8_t length){
    if(txData == NULL){
      return IMU_UNEXPECTED_ERROR;
    }

    m_spiTxBuff[0] = (uint8_t) regStartAddr;
    m_spiTxBuff[0] |= 0x40; // multibyte write

    if(length+1 > SPI_TX_BUFFER_SIZE)
        return IMU_WRONG_DATA_SIZE;

    memcpy(m_spiTxBuff+1, txData, length);

    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_L3GD20H, 0);
    spiTransmitData(m_spi, &m_gyroDataConfig, length+1, (uint16_t *)&m_spiTxBuff);
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
  ImuError ImuComponentImpl :: readAngularRates(float32 *gyrX, float32 *gyrY,  float32 *gyrZ){
    uint16_t rxBuffer[6]; // 3 x 2 bytes
    int16_t tmp;
    ImuError err = IMU_NO_ERROR;

    // read all angular rates data for x y z axis
    err = gyroReadData(L3gd20h::L3gd20hRegister::OUT_X_L, rxBuffer, sizeof(rxBuffer));

    tmp = rxBuffer[0] | rxBuffer[1] << 8;
    *gyrX = (float32)(tmp) * ONE_OVER_1024 * GYRO_RANGE /*normalize to 1.0 then maximum g range*/;

    tmp = rxBuffer[2] | rxBuffer[3] << 8;
    *gyrY = (float32)(tmp) * ONE_OVER_1024 * GYRO_RANGE;

    tmp = rxBuffer[4] | rxBuffer[5] << 8;
    *gyrZ = (float32)(tmp) * ONE_OVER_1024 * GYRO_RANGE;

    return err;
  }


} // end namespace CubeRover
