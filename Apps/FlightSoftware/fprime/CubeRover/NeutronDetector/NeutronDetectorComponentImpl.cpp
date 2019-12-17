// ======================================================================
// \title  NeutronDetectorComponentImpl.cpp
// \author cedric
// \brief  cpp file for NeutronDetector component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/NeutronDetector/NeutronDetectorComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace CubeRover {

  // Global variable required to process etpwm interrupt
  NeutronDetector::MuxPortMap g_readInput;
  NeutronDetector::MuxPortMap g_clockOutput;

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  NeutronDetectorComponentImpl ::
#if FW_OBJECT_NAMES == 1
    NeutronDetectorComponentImpl(
        const char *const compName
    ) :
      NeutronDetectorComponentBase(compName)
#else
    NeutronDetectorComponentImpl(void)
#endif
  {

  }


  /**
   * @brief      Initialize the f-prime component
   *
   * @param[in]  queueDepth  The queue depth
   * @param[in]  instance    The instance
   */
  void NeutronDetectorComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    NeutronDetectorComponentBase::init(queueDepth, instance);
  }


  /**
   * @brief      Destroys the object.
   */
  NeutronDetectorComponentImpl ::
    ~NeutronDetectorComponentImpl(void)
  {

  }


  NeutronDetector::Error NeutronDetectorComponentImpl :: resetMultiplexer(){
    spiWriteRegister(NeutronDetector::IoExpanderRegAddress::GPIOA, 0xFF);
    spiWriteRegister(NeutronDetector::IoExpanderRegAddress::GPIOB, 0xFF);

    return NeutronDetector::ND_NO_ERROR;
  }

  /**
   * @brief      Sets the multiplexer.
   *
   * @param[in]  sensor       The sensor
   * @param[in]  sensorPlate  The sensor plate
   *
   * @return     The neutron detector error
   */
  NeutronDetector::Error NeutronDetectorComponentImpl :: setMultiplexer(const NeutronDetector::SensorIterator sensor,
                                                                        const NeutronDetector::SensorPlateIterator sensorPlate){
    // Plate selection is done by activating one output at a time
    // First disable all outputs
    spiWriteRegister(NeutronDetector::IoExpanderRegAddress::GPIOB, m_decoderLookUpTable[sensor]);
    spiWriteRegister(NeutronDetector::IoExpanderRegAddress::GPIOA, m_plateLookUpTable[sensorPlate]);

    return NeutronDetector::ND_NO_ERROR;
  }

  /**
   * @brief      Setup the detector
   *
   * @return     The neutron detector error
   */
  NeutronDetector::Error NeutronDetectorComponentImpl :: setupDetector(){

    // PWM timer used to generate the clock signal
    // output is toggled from the PWM ISR on period event
    etpwmInit();
    spiInit();

    m_spiDataConfigHandler.CS_HOLD = false;
    m_spiDataConfigHandler.DFSEL = SPI_FMT_0;
    m_spiDataConfigHandler.WDEL = false;
    m_spiDataConfigHandler.CSNR = 0;

    // define port/bit used to read data from sensor
    g_readInput.port = gioPORTA;
    g_readInput.bit = 0;

    // define port/bit used to send clock signal to sensor
    g_clockOutput.port = gioPORTA;
    g_clockOutput.bit = 1;

    setupGioExpander();

    // Stop the timer interrupt
    etpwmDisableInterrupt(TIMER_EPWM_REG);

    // Start timer B clock
    etpwmStartTBCLK();

    return NeutronDetector::ND_NO_ERROR;
  }
  
  NeutronDetector::Error NeutronDetectorComponentImpl :: spiWriteRegister(const NeutronDetector::IoExpanderRegAddress addr, const uint8_t val){

      m_spiTxBuff[0] = GIO_DEV_PREFIX_ADDRESS + GIO_DEV_ADDRESS + 0x00 /* write */;
      m_spiTxBuff[1] = addr;
      m_spiTxBuff[2] = val;

      // set CS low
      gioSetBit(SPI_REG_PORT, SPI3_CS_BIT, 0);

      // send data
      spiTransmitData(SPI_REG, &m_spiDataConfigHandler, 3 /*3 bytes to send */, (uint16_t *)m_spiTxBuff);

      // set CS high
      gioSetBit(SPI_REG_PORT, SPI3_CS_BIT, 1);

      return NeutronDetector::ND_NO_ERROR;
  }

  NeutronDetector::Error NeutronDetectorComponentImpl :: spiReadRegister(const NeutronDetector::IoExpanderRegAddress addr,
                                                                         uint8_t *val,
                                                                         const uint8_t sizeOfData){

      m_spiTxBuff[0] = GIO_DEV_PREFIX_ADDRESS + GIO_DEV_ADDRESS + 0x01 /* read */;
      m_spiTxBuff[1] = addr;

      if(sizeOfData > SPI_RX_MAX_PACKET_SIZE_BYTE){
          return NeutronDetector::ND_DATA_SIZE_ERROR;
      }

      // set CS low
      gioSetBit(SPI_REG_PORT, SPI3_CS_BIT, 0);

      // send/receive data
      spiTransmitData(SPI_REG, &m_spiDataConfigHandler, 2 /*2 bytes to send */, (uint16_t *)m_spiTxBuff);
      spiReceiveData(SPI_REG, &m_spiDataConfigHandler, sizeOfData, (uint16_t *)m_spiRxBuff);

      // set CS high
      gioSetBit(SPI_REG_PORT, SPI3_CS_BIT, 1);

      for(uint8_t i=0; i<sizeOfData; i++){
       *val = m_spiRxBuff[i];
       val++;
      }

      return NeutronDetector::ND_NO_ERROR;
  }

  NeutronDetector::Error NeutronDetectorComponentImpl :: setupGioExpander(){
      // Configure all pints as outputs
      spiWriteRegister(NeutronDetector::IoExpanderRegAddress::IODIRA, 0x00 /* all outputs */);
      spiWriteRegister(NeutronDetector::IoExpanderRegAddress::IODIRB, 0x00 /* all outputs */);
      spiWriteRegister(NeutronDetector::IoExpanderRegAddress::IPOLA, 0x00 /* no polarity change */);
      spiWriteRegister(NeutronDetector::IoExpanderRegAddress::IPOLB, 0x00 /* no polarity change  */);
      spiWriteRegister(NeutronDetector::IoExpanderRegAddress::GPIOA, 0xFF /* all outputs set high*/);
      spiWriteRegister(NeutronDetector::IoExpanderRegAddress::GPIOB, 0xFF /* all outputs set high*/);

      // Plate look-up is active low
      m_plateLookUpTable[0] = 0b11110;
      m_plateLookUpTable[1] = 0b11101;
      m_plateLookUpTable[2] = 0b11011;
      m_plateLookUpTable[3] = 0b10111;
      m_plateLookUpTable[4] = 0b01111;

      // Decoder look-up table per RDT documentation
      m_decoderLookUpTable[0] = 12;
      m_decoderLookUpTable[1] = 8;
      m_decoderLookUpTable[2] = 4;
      m_decoderLookUpTable[3] = 0;
      m_decoderLookUpTable[4] = 13;
      m_decoderLookUpTable[5] = 9;
      m_decoderLookUpTable[6] = 5;
      m_decoderLookUpTable[7] = 1;
      m_decoderLookUpTable[8] = 14;
      m_decoderLookUpTable[9] = 10;
      m_decoderLookUpTable[10] = 6;
      m_decoderLookUpTable[11] = 2;
      m_decoderLookUpTable[12] = 15;
      m_decoderLookUpTable[13] = 11;
      m_decoderLookUpTable[14] = 7;
      m_decoderLookUpTable[15] = 3;

      return NeutronDetector::ND_NO_ERROR;
  }

  /**
   * @brief      Gets the sensor array.
   *
   * @param[in]  array  The array
   *
   * @return     The sensor array.
   */
  NeutronDetector::Error NeutronDetectorComponentImpl ::
    getSensorArray(NeutronDetector::NeutronSensorArray array){

    NeutronDetector::SensorIterator sensor;
    NeutronDetector::SensorPlateIterator sensorPlate;

    resetMultiplexer();

    // Scan through each sensor
    for(sensorPlate = 0; sensorPlate < TOTAL_MSND_PLATE; sensorPlate++){
     for(sensor = 0; sensor < TOTAL_MSND_PER_PLATE; sensor++){
        // Set the multiplexer to address the correct sensor
        setMultiplexer(sensor, sensorPlate);

        // Read the data from a single sensor and store it at the correct location
        readSensorData(array + sensor + sensorPlate*TOTAL_MSND_PER_PLATE);

        resetMultiplexer();
     }
   }
    return NeutronDetector::ND_NO_ERROR;
  }

static uint8_t g_bitToRead;
static uint8_t *g_msndByte = NULL;
bool g_readCompleted;

// triggers on period event
extern "C"{
  void etpwmNotification(etpwmBASE_t *node)
  {
      // First read the bit
      if(g_bitToRead > 0){
         *g_msndByte |= gioGetBit(g_readInput.port, g_readInput.bit) << (g_bitToRead - 1);
          g_bitToRead--;

          // toggle output
          gioToggleBit(g_clockOutput.port, g_clockOutput.bit);
      }
      else{
          g_readCompleted = true; // no more bit to read
          //no more toggling
          gioSetBit(g_clockOutput.port, g_clockOutput.bit, 1);
          // Stop the timer interrupt
          etpwmDisableInterrupt(TIMER_EPWM_REG);
      }
  }
}

  /**
   * @brief      Read sensor data
   *
   * @param      data  The data
   *
   * @return     The Neutron sensor detector
   */
  NeutronDetector::Error NeutronDetectorComponentImpl ::
    readSensorData(NeutronDetector::NeutronSensorData *data){

    uint16_t totalBytesToTransmit = MSND_TX_PACKET_SIZE_BYTE;
    uint16_t i;

    g_msndByte = (uint8_t *)m_msndBuff;

    for(i=0 ; i<totalBytesToTransmit; i++){
        *g_msndByte = 0; // reset value to read
        g_bitToRead = 8; // 8 bits per byte
        g_readCompleted = false;

        // Enable capture compare. Set event interrupt flag when compare B is triggered
        // on first event
        // Clear event interrupt flag
        etpwmClearEventFlag(TIMER_EPWM_REG, Event_Interrupt);
        etpwmEnableInterrupt(TIMER_EPWM_REG, CTR_PRD, EventPeriod_FirstEvent);

        // block until all bits have been pushed
        while(g_readCompleted == false);

        // increment pointer
        g_msndByte++;
    }

    // Copy data from buffer to data to return
    for(i=0; i< sizeof(m_msndBuff); i++){
        *data = m_msndBuff[i];
        data++;
    }

    return NeutronDetector::ND_NO_ERROR;
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void NeutronDetectorComponentImpl ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
      getSensorArray((NeutronDetector::NeutronSensorArray)m_neutronSensorArray);
    // TODO
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void NeutronDetectorComponentImpl ::
    ND_CollectData_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

} // end namespace CubeRover
