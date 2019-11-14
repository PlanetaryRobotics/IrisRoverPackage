// ======================================================================
// \title  CameraInterfaceComponentImpl.cpp
// \author cedric
// \brief  cpp file for CameraInterface component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/CameraInterface/CameraInterfaceComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  CameraInterfaceComponentImpl ::
#if FW_OBJECT_NAMES == 1
    CameraInterfaceComponentImpl(
        const char *const compName
    ) :
      CameraInterfaceComponentBase(compName)
#else
    CameraInterfaceComponentImpl(void)
#endif
  {
      // External Flash SPI data configuration
      m_flashDataConfig.CS_HOLD = false;
      m_flashDataConfig.DFSEL = SPI_FMT_0;
      m_flashDataConfig.WDEL = false;
      m_flashDataConfig.CSNR = 0;

      m_spi = NULL;
      m_setup = false;
  }

  void CameraInterfaceComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    CameraInterfaceComponentBase::init(queueDepth, instance);
  }

  CameraInterfaceComponentImpl ::
    ~CameraInterfaceComponentImpl(void)
  {

  }

  CameraError CameraInterfaceComponentImpl :: setup(spiBASE_t *spi){
    CameraError err = CAMERA_NO_ERROR;

    if(spi == NULL){
      return CAMERA_UNEXPECTED_ERROR;
    }

    if(m_setup)
      return err;

    m_spi = spi;
    m_setup = true;

    err = setupExternalFlash(spi);

    if(err != CAMERA_NO_ERROR)
      return err;
  }


  /**
   * @brief      Setup external flash (S25FL064L)
   *
   * @param      spi   The spi
   *
   * @return     The camera error code
   */
  CameraError CameraInterfaceComponentImpl :: setupExternalFlash(spiBASE_t *spi){

    CameraError err = CAMERA_NO_ERROR;

    if(spi == NULL){
      return CAMERA_UNEXPECTED_ERROR; 
    }


    err = flashReadData(,,);

    if(err != CAMERA_NO_ERROR)
        return err;

    return err;
  }


  /**
   * @brief      Read spi register from external flash 
   *
   * @param[in]  regStartAddr  The register start address
   * @param      rxData        The receive data
   * @param[in]  length        The length
   *
   * @return     The camera error code
   */
  CameraError CameraInterfaceComponentImpl :: flashReadData(const S25fl064l::FlashSpiRegister regStartAddr,
                                                            uint16_t *rxData, 
                                                            const uint8_t length){

    if(rxData == NULL){
      return CAMERA_UNEXPECTED_ERROR;
    }

    m_spiTxBuff[0] = (uint8_t) regStartAddr;

    if(length > SPI_RX_BUFFER_SIZE)
        return CAMERA_WRONG_DATA_SIZE;

    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_EXT_FLASH, 0);
    spiTransmitData(m_spi, &m_flashDataConfig, 1, (uint16_t *)&m_spiTxBuff);
    spiReceiveData(m_spi, &m_flashDataConfig, length, (uint16_t *)&m_spiRxBuff);
    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_EXT_FLASH, 1);

    memcpy(rxData, m_spiRxBuff, length);

    return CAMERA_NO_ERROR;
  }


  /**
   * @brief      Write spi register to external flash
   *
   * @param[in]  regStartAddr  The register start address
   * @param      txData        The transmit data
   * @param[in]  length        The length
   *
   * @return     The camera error code
   */
  CameraError CameraInterfaceComponentImpl :: flashWriteData(const S25fl064l::FlashSpiRegister regStartAddr,
                                                             uint16_t *txData, 
                                                             const uint8_t length){

    if(txData == NULL){
      return CAMERA_UNEXPECTED_ERROR;
    }

    m_spiTxBuff[0] = (uint8_t) regStartAddr;
    m_spiTxBuff[0] |= 0x40; // multibyte write

    if(length+1 > SPI_TX_BUFFER_SIZE)
        return CAMERA_WRONG_DATA_SIZE;

    memcpy(m_spiTxBuff+1, txData, length);

    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_EXT_FLASH, 0);
    spiTransmitData(m_spi, &m_flashDataConfig, length+1, (uint16_t *)&m_spiTxBuff);
    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_EXT_FLASH, 1);

    return CAMERA_NO_ERROR;
  }


  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void CameraInterfaceComponentImpl ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void CameraInterfaceComponentImpl ::
    CAM_CaptureImage_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void CameraInterfaceComponentImpl ::
    CAM_FetchImage_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

} // end namespace CubeRover
