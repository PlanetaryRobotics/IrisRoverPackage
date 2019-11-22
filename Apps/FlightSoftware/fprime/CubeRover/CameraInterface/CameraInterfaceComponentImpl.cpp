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
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <string.h>

#include "Fw/Types/BasicTypes.hpp"
#include "CubeRoverConfig.hpp"
#include "gio.h"
#include "spi.h"

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

      // Refer to datasheet, default number of dummy cycles between a SDI and SDO
      // is set by default to 8 clock cycles
      m_readLatencyCycles = DEFAULT_DUMMY_CYCLES;

      m_setup = false;
      m_flashSpi = NULL;
      m_fpgaSpi = NULL;
      m_memAllocPointer = 0;
      // default setting of the external memory
      m_addressLengthFormat = CameraInterface::S25fl064l::ADDRESS_LENGTH_3_BYTES; 
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

  CameraError CameraInterfaceComponentImpl :: setup(spiBASE_t *flashSpi, spiBASE_t *fpgaSpi){
    CameraError err = CAMERA_NO_ERROR;

    if(flashSpi == NULL || fpgaSpi == NULL){
      return CAMERA_UNEXPECTED_ERROR;
    }

    m_flashSpi = flashSpi;
    m_fpgaSpi = fpgaSpi;

    if(m_setup)
      return err;

    err = setupExternalFlash();

    if(err != CAMERA_NO_ERROR)
      return err;

    m_setup = true;

    return err;
  }


  /**
   * @brief      Setup external flash (S25FL064L)
   *
   * @param      spi   The spi
   *
   * @return     The camera error code
   */
  CameraError CameraInterfaceComponentImpl :: setupExternalFlash(){

    CameraError err = CAMERA_NO_ERROR;

    if(m_flashSpi == NULL){
      return CAMERA_UNEXPECTED_ERROR; 
    }

    uint16_t id[3];
    err = flashSpiReadData(CameraInterface::S25fl064l::RDID, id, 3);

    if(err != CAMERA_NO_ERROR)
        return err;

    return err;
  }

  CameraError CameraInterfaceComponentImpl :: setupFPGAInterface(){

      return CAMERA_NO_ERROR;
  }

  /**
   * @brief      Gets the address length byte.
   *
   * @param[in]  cmd   The command
   *
   * @return     The address length byte.
   */
  uint16_t CameraInterfaceComponentImpl :: getAddressLengthByte(const CameraInterface::S25fl064l::FlashSpiCommands cmd){
    switch(cmd){
      case CameraInterface::S25fl064l::RDID:   // Read ID
      case CameraInterface::S25fl064l::RDQID:   // Read Quad ID
      case CameraInterface::S25fl064l::RUID:   // Read Unique ID
      case CameraInterface::S25fl064l::RDSR1:   // Read Status Register 1
      case CameraInterface::S25fl064l::RDSR2:   // Read Status Register 2
      case CameraInterface::S25fl064l::RDCR1:   // Read Control Register 1
      case CameraInterface::S25fl064l::RDCR2:   // Read Control Register 2
      case CameraInterface::S25fl064l::RDCR3:   // Read Control Register 3
      case CameraInterface::S25fl064l::WRR:     // Write Register (Status -1 and configuration -1,2,3)
      case CameraInterface::S25fl064l::WRDI:   // Write Disable
      case CameraInterface::S25fl064l::WREN:   // Write enable for non volatile data change
      case CameraInterface::S25fl064l::WRENV:   // Write enable for volatile status and configuration registers
      case CameraInterface::S25fl064l::CLSR:   // Clear status register
      case CameraInterface::S25fl064l::_4BEN:   // Enter 4 byte address mode
      case CameraInterface::S25fl064l::_4BEX:   // Exit 4 byte address mode
      case CameraInterface::S25fl064l::SBEL:   // Set burst length
      case CameraInterface::S25fl064l::QPIEN:   // Enter QPI
      case CameraInterface::S25fl064l::QPIEX:   // Exit QPI
      case CameraInterface::S25fl064l::DLPRD:   // Data learning pattern read
      case CameraInterface::S25fl064l::PDLRNV:   // Program NV data learning register
      case CameraInterface::S25fl064l::WDLRV:   // Write volatile data learning center
      case CameraInterface::S25fl064l::CE:   // Chip erase
      case CameraInterface::S25fl064l::EPS:   // Erase / program suspend
      case CameraInterface::S25fl064l::EPR:   // Erase / program resume
      case CameraInterface::S25fl064l::GBL:   // Global IBL block
      case CameraInterface::S25fl064l::GBUL:   // Global IBL unblock
      case CameraInterface::S25fl064l::RSTEN:        // Software reset enable
      case CameraInterface::S25fl064l::RST:        // Software reset
      case CameraInterface::S25fl064l::MBR:        // Mode bit reset
      case CameraInterface::S25fl064l::DPD:        // Deep power down
      case CameraInterface::S25fl064l::RES:     //from deep power down / device idcase 
        return 0;
      case CameraInterface::S25fl064l::RSFDP:   // Read JEDEC Serial Flash Discoverable parameters
      case CameraInterface::S25fl064l::RDAR: 
      case CameraInterface::S25fl064l::WRAR:   // Write any register
      case CameraInterface::S25fl064l::READ:   // Read
      case CameraInterface::S25fl064l::FAST_READ:   // Fast read
      case CameraInterface::S25fl064l::DOR:   // Dual output read
      case CameraInterface::S25fl064l::QOR:   // Quad output read
      case CameraInterface::S25fl064l::QIOR:   // Quad I/O read
      case CameraInterface::S25fl064l::DIOR:   // Dual I/O read
      case CameraInterface::S25fl064l::DDRQIOR:   // DDR Quad I/O read
      case CameraInterface::S25fl064l::PP:   // Page program
      case CameraInterface::S25fl064l::QPP:   // Quad page program
      case CameraInterface::S25fl064l::SE:   // Sector erase
      case CameraInterface::S25fl064l::HBE:   // Half-block erase
      case CameraInterface::S25fl064l::BE:   // Block erase
      case CameraInterface::S25fl064l::SECRE:   // Security region erase
      case CameraInterface::S25fl064l::SECRP:   // Security region program
      case CameraInterface::S25fl064l::SECRR:   // Security region read
      case CameraInterface::S25fl064l::IBLRD:   // IBL read
      case CameraInterface::S25fl064l::IBL:   // IBL lock
      case CameraInterface::S25fl064l::IBUL:   // IBL unblock
      case CameraInterface::S25fl064l::SPRP:   // Set pointer region protection
        return m_addressLengthFormat;
      case CameraInterface::S25fl064l::_4READ:   // Read
      case CameraInterface::S25fl064l::_4FAST_READ:   // Read
      case CameraInterface::S25fl064l::_4DOR:   // Dual output read
      case CameraInterface::S25fl064l::_4QOR:   // Dual output read
      case CameraInterface::S25fl064l::_4DIOR:   // Dual I/O read
      case CameraInterface::S25fl064l::_4QIOR:   // Quad I/O read
      case CameraInterface::S25fl064l::_4DDRQIOR:   // DDR Quad I/O read
      case CameraInterface::S25fl064l::_4PP:   // Page program
      case CameraInterface::S25fl064l::_4QPP:   // Quad page program
      case CameraInterface::S25fl064l::_4SE:   // Sector erase
      case CameraInterface::S25fl064l::_4HBE:   // Half-block erase
      case CameraInterface::S25fl064l::_4BE:   // Block erase
      case CameraInterface::S25fl064l::_4IBLRD:   // IBL read
      case CameraInterface::S25fl064l::_4IBUL:   // IBL unblock
      case CameraInterface::S25fl064l::_4IBL:   // IBL lock
      case CameraInterface::S25fl064l::_4SPRP:   // Set pointer region protection
        return 4;
      default:
        return 0;
      }
  }

  /**
   * @brief      Read spi register from external flash
   *
   * @param[in]  cmd           The command
   * @param      rxData        The receive data
   * @param[in]  sizeOfRxData  The size of the receive data
   * @param[in]  address       The address
   * @param[in]  dataReadLength  The data read length
   * @param[in]  regStartAddr    The register start address
   * @param[in]  length          The length
   *
   * @return     The camera error code
   */
  CameraError CameraInterfaceComponentImpl :: flashSpiReadData(const CameraInterface::S25fl064l::FlashSpiCommands cmd,
                                                            uint16_t *rxData,
                                                            const uint16_t sizeOfRxData,
                                                            const CameraInterface::S25fl064l::Address address){
    uint16_t addressLength;
    uint16_t totalBytesToTransmit; 

    if(rxData == NULL){
      return CAMERA_UNEXPECTED_ERROR;
    }

    // at first, we just send command + any dummy cycles (converted to number of bytes)
    totalBytesToTransmit = 1;
    m_spiTxBuff[0] = (uint8_t) cmd;

    addressLength = getAddressLengthByte(cmd);
    totalBytesToTransmit += addressLength;

    if(sizeOfRxData > SPI_RX_BUFFER_MAX_LENGTH)
        return CAMERA_WRONG_DATA_SIZE;

    if(addressLength > 0){
      if(address == ADDRESS_NOT_DEFINED){
        return CAMERA_UNEXPECTED_ERROR;
      }

    // Add dummy cycles required for commands having an address
    totalBytesToTransmit += (m_readLatencyCycles >> 3);

    if(totalBytesToTransmit > SPI_TX_BUFFER_MAX_LENGTH)
        return CAMERA_WRONG_DATA_SIZE;

      memcpy(m_spiTxBuff+1, (uint8_t *)address, addressLength);
    }

    // Set CS low
    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_EXT_FLASH, 0);

    // Send transmission data + a number of dummy cyles (m_readLatencyCycles) required by the device
    // The number of cycles is a multiple of 8, one byte = 8 cycles
    spiTransmitData(m_flashSpi, &m_flashDataConfig, totalBytesToTransmit, (uint16_t *)&m_spiTxBuff);
    spiReceiveData(m_flashSpi, &m_flashDataConfig, sizeOfRxData, (uint16_t *)&m_spiRxBuff);

    // Set CS high
    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_EXT_FLASH, 1);

    memcpy(rxData, m_spiRxBuff, sizeOfRxData);

    return CAMERA_NO_ERROR;
  }


  /**
   * @brief      Write data flash device over SPI
   *
   * @param[in]  cmd     The command
   * @param      txData  The transmit data
   * @param[in]  length  The length
   *
   * @return     The camera error code
   */
  CameraError CameraInterfaceComponentImpl :: flashSpiWriteData(const CameraInterface::S25fl064l::FlashSpiCommands cmd,
                                                                uint16_t *txData, 
                                                                const uint16_t sizeOfTxData,
                                                                CameraInterface::S25fl064l::Address address){
    uint16_t addressLength;
    uint16_t totalBytesToTransmit; 

    if(txData == NULL && sizeOfTxData > 0){
      return CAMERA_UNEXPECTED_ERROR;
    }

    addressLength = getAddressLengthByte(cmd);
    m_spiTxBuff[0] = (uint8_t) cmd;
    totalBytesToTransmit = sizeOfTxData + 1 /*command*/ + addressLength;

    if(addressLength > 0){
      if(address == ADDRESS_NOT_DEFINED){
        return CAMERA_UNEXPECTED_ERROR;
      }

      if(totalBytesToTransmit > SPI_TX_BUFFER_MAX_LENGTH){
        return CAMERA_WRONG_DATA_SIZE;
      }

      // Copy the address section to the transmit buffer
      memcpy(m_spiTxBuff+1, (uint8_t *)&address, addressLength);     

      if(txData == NULL){
        return CAMERA_UNEXPECTED_ERROR;
      }

      if(txData != NULL && sizeOfTxData > 0){
        // Copy data to transmit buffer
        memcpy(m_spiTxBuff + 1 /*command */ + addressLength, txData, sizeOfTxData);
      }
    }

    // Set CS low
    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_EXT_FLASH, 0);

    spiTransmitData(m_flashSpi, &m_flashDataConfig, totalBytesToTransmit, (uint16_t *)&m_spiTxBuff);

    // Set CS high
    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_EXT_FLASH, 1);

    return CAMERA_NO_ERROR;
  }

  /**
   * @brief      Suspend an erase or program operation
   *
   * @return     The camera error code
   */
  CameraError CameraInterfaceComponentImpl :: programEraseSuspend(){
    CameraInterface::S25fl064l::StatusRegister1 status1;
    CameraInterface::S25fl064l::StatusRegister2 status2;
    CameraError err;

    // Read status register to check if program/erase is on-going
    err = flashSpiReadData(CameraInterface::S25fl064l::RDSR1, (uint16_t *)&status1.all, 1);
    if(err != CAMERA_NO_ERROR){
      return err;
    }

    err = flashSpiReadData(CameraInterface::S25fl064l::RDSR2, (uint16_t *)&status2.all, 1);
    if(err != CAMERA_NO_ERROR){
      return err;
    }

    // Check if erase or program are in progress, if not, there is nothing to
    // suspend Check if an erase suspend or program suspend is already
    // happening, if so, don't do anything.
    if(status1.bit.wip == 0 && (status2.bit.es || status2.bit.ps)){
      return CAMERA_NO_ERROR;
    }

    // Send sector erase The flash spi write doesn't require to write any data,
    // only address is required
    err = flashSpiWriteData(CameraInterface::S25fl064l::EPS);

    return err;
  }


  /**
   * @brief      Resume programming / erase
   *
   * @return     The camera error code
   */
  CameraError CameraInterfaceComponentImpl :: programEraseResume(){
    CameraInterface::S25fl064l::StatusRegister1 status1;
    CameraInterface::S25fl064l::StatusRegister2 status2;
    CameraError err;

    // Read status register to check if program/erase is on-going
    err = flashSpiReadData(CameraInterface::S25fl064l::RDSR1, (uint16_t *)&status1.all, 1);
    if(err != CAMERA_NO_ERROR){
      return err;
    }

    err = flashSpiReadData(CameraInterface::S25fl064l::RDSR2, (uint16_t *)&status2.all, 1);
    if(err != CAMERA_NO_ERROR){
      return err;
    }

    // If nothing is in progress or suspended, return immediately
    if(status1.bit.wip == 0 && status2.bit.es == 0 && status2.bit.ps == 0){
      return CAMERA_NO_ERROR;
    }

    // Send sector erase The flash spi write doesn't require to write any data,
    // only address is required
    err = flashSpiWriteData(CameraInterface::S25fl064l::EPR);

    return err;
  } 

  /**
   * @brief      Perform a soft reset of the device
   *
   * @return     The camera error code
   */
  CameraError CameraInterfaceComponentImpl :: resetDevice(){
    CameraError err;

    // Enable reset command
    err = flashSpiWriteData(CameraInterface::S25fl064l::RSTEN);
    if(err != CAMERA_NO_ERROR){
      return err;
    }

    // Perform a soft reset of the device
    err = flashSpiWriteData(CameraInterface::S25fl064l::RST);

    return err;
  }

 /**
   * @brief      Erase the chip memory
   *
   * @return     The camera error
   */
  CameraError CameraInterfaceComponentImpl :: chipErase(){
    CameraInterface::S25fl064l::StatusRegister1 status1;
    CameraInterface::S25fl064l::StatusRegister2 status2;
    CameraError err;
    uint16_t tries = 100;

    // Enable writing to the device
    err = flashSpiWriteData(CameraInterface::S25fl064l::WREN);

    if(err != CAMERA_NO_ERROR){
      return err;
    }

    // Send sector erase
    // The flash spi write doesn't require to write any data, only address is required
    err = flashSpiWriteData(CameraInterface::S25fl064l::CE);

    // Read status register to check that write is enabled
    err = flashSpiReadData(CameraInterface::S25fl064l::RDSR1, (uint16_t *)&status1.all, 1);
    
    if(err != CAMERA_NO_ERROR){
      return err;
    }

    // Write Enable Latch must be set at that point
    if(status1.bit.wel == 0){
      return CAMERA_FAIL_ERASE_CHIP;
    }

    // Poll the device to check the "work in progress" flag from the status register 1
    while(tries > 0){
      err = flashSpiReadData(CameraInterface::S25fl064l::RDSR1, (uint16_t *)&status1.all, 1);
      if(err != CAMERA_NO_ERROR){
        return err;
      }

      if(status1.bit.wip == 1){
        tries--;
      }
      else{
        break;
      }
    }

    if(tries == 0){
      return CAMERA_FAIL_ERASE_CHIP;
    }

    // Check if an error occured at completion of the programming
    err = flashSpiReadData(CameraInterface::S25fl064l::RDSR2, (uint16_t *)&status2.all, 1);

    if(err != CAMERA_NO_ERROR){
      return err;
    }

    if(status2.bit.e_err){
      return CAMERA_FAIL_PAGE_PROGRAM;
    }

    // Disable writing to the device
    err = flashSpiWriteData(CameraInterface::S25fl064l::WRDI);

    return err;
  }

  /**
   * @brief      Erase memry block
   *
   * @param[in]  block  The block of memory to erase
   *
   * @return     The camera error
   */
  CameraError CameraInterfaceComponentImpl :: blockErase(const CameraInterface::S25fl064l::Block block){
    CameraInterface::S25fl064l::Address address;
    CameraInterface::S25fl064l::StatusRegister1 status1;
    CameraInterface::S25fl064l::StatusRegister2 status2;
    CameraError err;
    uint16_t tries = 100;

    if(block > MAX_BLOCK_RANGE){
      return CAMERA_UNEXPECTED_ERROR;
    }

    // Enable writing to the device
    err = flashSpiWriteData(CameraInterface::S25fl064l::WREN);

    if(err != CAMERA_NO_ERROR){
      return err;
    }

    address = block * BLOCK_SIZE;

    // Send sector erase
    // The flash spi write doesn't require to write any data, only address is required
    err = flashSpiWriteData(CameraInterface::S25fl064l::BE, NULL, 0, address);

    // Read status register to check that write is enabled
    err = flashSpiReadData(CameraInterface::S25fl064l::RDSR1, (uint16_t *)&status1.all, 1);
    
    if(err != CAMERA_NO_ERROR){
      return err;
    }

    // Write Enable Latch must be set at that point
    if(status1.bit.wel == 0){
      return CAMERA_FAIL_SECTOR_ERASE;
    }

    // Poll the device to check the "work in progress" flag from the status register 1
    while(tries > 0){
      err = flashSpiReadData(CameraInterface::S25fl064l::RDSR1, (uint16_t *)&status1.all, 1);
      if(err != CAMERA_NO_ERROR){
        return err;
      }

      if(status1.bit.wip == 1){
        tries--;
      }
      else{
        break;
      }
    }

    if(tries == 0){
      return CAMERA_FAIL_BLOCK_ERASE;
    }

    // Check if an error occured at completion of the programming
    err = flashSpiReadData(CameraInterface::S25fl064l::RDSR2, (uint16_t *)&status2.all, 1);

    if(err != CAMERA_NO_ERROR){
      return err;
    }

    if(status2.bit.e_err){
      return CAMERA_FAIL_PAGE_PROGRAM;
    }

    // Disable writing to the device
    err = flashSpiWriteData(CameraInterface::S25fl064l::WRDI);

    return err;
  }
  
  /**
   * @brief      Erase half-block of memory
   *
   * @param[in]  block  The block
   *
   * @return     The camera error
   */
  CameraError CameraInterfaceComponentImpl :: halfBlockErase(const CameraInterface::S25fl064l::HalfBlock halfBlock){
    CameraInterface::S25fl064l::Address address;
    CameraInterface::S25fl064l::StatusRegister1 status1;
    CameraInterface::S25fl064l::StatusRegister2 status2;
    CameraError err;
    uint16_t tries = 100;

    if(halfBlock > MAX_HALF_BLOCK_RANGE){
      return CAMERA_UNEXPECTED_ERROR;
    }

    // Enable writing to the device
    err = flashSpiWriteData(CameraInterface::S25fl064l::WREN);

    if(err != CAMERA_NO_ERROR){
      return err;
    }

    address = halfBlock * HALF_BLOCK_SIZE;

    // Send sector erase
    // The flash spi write doesn't require to write any data, only address is required
    err = flashSpiWriteData(CameraInterface::S25fl064l::HBE, NULL, 0, address);

    // Read status register to check that write is enabled
    err = flashSpiReadData(CameraInterface::S25fl064l::RDSR1, (uint16_t *)&status1.all, 1);
    
    if(err != CAMERA_NO_ERROR){
      return err;
    }

    // Write Enable Latch must be set at that point
    if(status1.bit.wel == 0){
      return CAMERA_FAIL_HALF_BLOCK_ERASE;
    }

    // Poll the device to check the "work in progress" flag from the status register 1
    while(tries > 0){
      err = flashSpiReadData(CameraInterface::S25fl064l::RDSR1, (uint16_t *)&status1.all, 1);
      if(err != CAMERA_NO_ERROR){
        return err;
      }

      if(status1.bit.wip == 1){
        tries--;
      }
      else{
        break;
      }
    }

    if(tries == 0){
      return CAMERA_FAIL_HALF_BLOCK_ERASE;
    }

    // Check if an error occured at completion of the erase
    err = flashSpiReadData(CameraInterface::S25fl064l::RDSR2, (uint16_t *)&status2.all, 1);

    if(err != CAMERA_NO_ERROR){
      return err;
    }

    if(status2.bit.e_err){
      return CAMERA_FAIL_PAGE_PROGRAM;
    }

    // Disable writing to the device
    err = flashSpiWriteData(CameraInterface::S25fl064l::WRDI);

    return err;
  }

  /**
   * @brief      Erase a sector
   *
   * @param[in]  sector  The sector
   *
   * @return     Return a camera error
   */
  CameraError CameraInterfaceComponentImpl :: sectorErase(const CameraInterface::S25fl064l::Sector sector){
    CameraInterface::S25fl064l::Address address;
    CameraInterface::S25fl064l::StatusRegister1 status1;
    CameraInterface::S25fl064l::StatusRegister2 status2;
    CameraError err;
    uint16_t tries = 100;

    if(sector > MAX_SECTOR_RANGE){
      return CAMERA_UNEXPECTED_ERROR;
    }

    // Enable writing to the device
    err = flashSpiWriteData(CameraInterface::S25fl064l::WREN);

    if(err != CAMERA_NO_ERROR){
      return err;
    }

    address = sector * SECTOR_SIZE;

    // Send sector erase
    // The flash spi write doesn't require to write any data, only address is required
    err = flashSpiWriteData(CameraInterface::S25fl064l::SE, NULL, 0, address);

    // Read status register to check that write is enabled
    err = flashSpiReadData(CameraInterface::S25fl064l::RDSR1, (uint16_t *)&status1.all, 1);
    
    if(err != CAMERA_NO_ERROR){
      return err;
    }

    // Write Enable Latch must be set at that point
    if(status1.bit.wel == 0){
      return CAMERA_FAIL_SECTOR_ERASE;
    }

    // Poll the device to check the "work in progress" flag from the status register 1
    while(tries > 0){
      err = flashSpiReadData(CameraInterface::S25fl064l::RDSR1, (uint16_t *)&status1.all, 1);
      if(err != CAMERA_NO_ERROR){
        return err;
      }

      if(status1.bit.wip == 1){
        tries--;
      }
      else{
        break;
      }
    }

    if(tries == 0){
      return CAMERA_FAIL_SECTOR_ERASE;
    }

    // Check if an error occured at completion of the erase
    err = flashSpiReadData(CameraInterface::S25fl064l::RDSR2, (uint16_t *)&status2.all, 1);

    if(err != CAMERA_NO_ERROR){
      return err;
    }

    if(status2.bit.e_err){
      return CAMERA_FAIL_PAGE_PROGRAM;
    }

    // Disable writing to the device
    err = flashSpiWriteData(CameraInterface::S25fl064l::WRDI);

    return err;
  }


  /**
   * @brief      Allocate flash memory. This is a simple implementation of a
   *             malloc function. It reserves memory aligned with sectors and
   *             blocks. It update the reserved size argument and update address
   *             pointer with a valid address or NULL and an error code if
   *             address cannot be reserved.
   *
   * @param[in]  size   The size
   * @param      alloc  The allocated memory structure
   *
   * @return     The camera error
   */
  CameraError CameraInterfaceComponentImpl :: allocateFlashMemory(CameraInterface::S25fl064l::MemAlloc *alloc,
                                                                  const uint32_t size){

    CameraInterface::S25fl064l::Address tmp = m_memAllocPointer;

    // Reset reserved memory to zero
    alloc->reservedSize = 0;

    // Calculate new address pointer by PAGE_SIZE increment
    tmp += (size / PAGE_SIZE) * PAGE_SIZE;
    tmp += (size % PAGE_SIZE) ? PAGE_SIZE : 0;

    // Check that new memory allocation pointer is within memory range 
    if(tmp > MAX_MEMORY_ADDRESS){
      return CAMERA_FAIL_MEM_ALLOCATION;
    }

    //Update address with valid address pointer
    alloc->startAddress = m_memAllocPointer;

    // Update the reserved size (larger or equal to size requested)
    alloc->reservedSize = tmp - m_memAllocPointer;

    //Increment address pointer for next memory allocation
    m_memAllocPointer += tmp;

    return CAMERA_NO_ERROR;
  }


  /**
   * @brief      Writes a data to flash.
   *
   * @param      alloc     The allocate
   * @param      data      The data
   * @param[in]  dataSize  The data size
   *
   * @return     The camera error
   */
  CameraError CameraInterfaceComponentImpl :: writeDataToFlash(CameraInterface::S25fl064l::MemAlloc *alloc,
                                                               uint8_t *data,
                                                               const uint16_t dataSize){
    CameraError err;
    CameraInterface::S25fl064l::Address sectorAddress;
    uint16_t i;
    uint16_t sectorOverlaps = 0;
    uint16_t totalBytesToWrite = dataSize;

    // Check if memory allocation is valid
    if(alloc->reservedSize == 0){
      return CAMERA_FAIL_WRITE_DATA_FLASH;
    }

    // Check if desired data write will fit in device memory range
    if(alloc->startAddress + dataSize > MAX_MEMORY_ADDRESS){
      return CAMERA_FAIL_WRITE_DATA_FLASH;
    }

    //loop here until there is data to write or an error occurs
    while(1){
      // Calculate the block address that belong to data to write.
      // Go to next sector if the data to write overlaps sectors
      sectorAddress = alloc->startAddress / SECTOR_SIZE * (SECTOR_SIZE + sectorOverlaps);

      // Back-up sector content to a back-up buffer
      err = flashSpiReadData( CameraInterface::S25fl064l::READ,
                              (uint16_t *)&m_sectorBackup,
                              sizeof(m_sectorBackup),
                              sectorAddress);

      // Erase sector address
      err = sectorErase((CameraInterface::S25fl064l::Sector) sectorAddress);

      // Copy data to scratchpad
      uint16_t bytesToCopy = (totalBytesToWrite <= sectorAddress + SECTOR_SIZE - alloc->startAddress) ? 
                              totalBytesToWrite : sectorAddress + SECTOR_SIZE - alloc->startAddress;


      // Overwrite data in the m_sectorBackup. Don't copy data that overlap
      // sectors
      memcpy(m_sectorBackup + alloc->startAddress + (sectorOverlaps*SECTOR_SIZE) - sectorAddress,
             data,
             bytesToCopy);


      // Program the whole using page programming with updated data
      for(i=0; i< SECTOR_SIZE / PAGE_SIZE; i++){
        err = pageProgram(alloc->startAddress, (uint16_t *)(m_sectorBackup + i*PAGE_SIZE), PAGE_SIZE);
        if(err != CAMERA_NO_ERROR){
          return err;
        }
      }

      //Substract the number of byte programmed from the number of bytes
      totalBytesToWrite -= bytesToCopy;

      // if there is still some bytes to write, go to next memory sector
      if(totalBytesToWrite > 0){
        sectorOverlaps++;
      }
      else{
        break;  // write completed
      }
    } // end of while loop

    return CAMERA_NO_ERROR;
  }


  /**
   * @brief      Reads a data from flash.
   *
   * @param      alloc     The allocate
   * @param      data      The data
   * @param[in]  dataSize  The data size
   *
   * @return     The camera error
   */
  CameraError CameraInterfaceComponentImpl :: readDataFromFlash(CameraInterface::S25fl064l::MemAlloc *alloc,
                                uint8_t *data,
                                const uint16_t dataSize){
    CameraError err;
    uint16_t totalBytesToRead = dataSize;
    uint16_t overflow = 0;

    while(totalBytesToRead){
      uint16_t bytesToRead = (totalBytesToRead < PAGE_SIZE) ? totalBytesToRead : PAGE_SIZE;

      err = flashSpiReadData( CameraInterface::S25fl064l::READ,
                              (uint16_t *)data + PAGE_SIZE * overflow,
                              bytesToRead,
                              alloc->startAddress + PAGE_SIZE * overflow);

      totalBytesToRead -= bytesToRead;
      overflow++;
    }

    return err;
  }

  /**
   * @brief      Program a page
   *
   * @param[in]  address  The address
   * @param      txData   The transmit data
   * @param[in]  size     The size
   *
   * @return     The error code.
   */
  CameraError CameraInterfaceComponentImpl :: pageProgram(CameraInterface::S25fl064l::Address address,
                                                          uint16_t *txData,
                                                          const uint16_t size){
    CameraInterface::S25fl064l::StatusRegister1 status1;
    CameraInterface::S25fl064l::StatusRegister2 status2;
    CameraError err;
    uint16_t tries = 100;

    // the size of a page is 256 bytes
    if(size > PAGE_SIZE){
      return CAMERA_UNEXPECTED_ERROR;
    }

    if(txData == NULL){
      return CAMERA_UNEXPECTED_ERROR;
    }

    // Enable writing to the device
    err = flashSpiWriteData(CameraInterface::S25fl064l::WREN);

    if(err != CAMERA_NO_ERROR){
      return err;
    }

    // Read status register
    err = flashSpiReadData(CameraInterface::S25fl064l::RDSR1, (uint16_t *)&status1.all, 1);
    
    if(err != CAMERA_NO_ERROR){
      return err;
    }

    // Write Enable Latch must be set at that point
    if(status1.bit.wel == 0){
      return CAMERA_FAIL_PAGE_PROGRAM;
    }

    // Send data to perform page programming
    err = flashSpiWriteData(CameraInterface::S25fl064l::PP, txData, size, address);
    
    if(err != CAMERA_NO_ERROR){
      return err;
    }

    // Poll the device to check the "work in progress" flag from the status register 1
    while(tries > 0){
      err = flashSpiReadData(CameraInterface::S25fl064l::RDSR1, (uint16_t *)&status1.all, 1);
      if(err != CAMERA_NO_ERROR){
        return err;
      }

      if(status1.bit.wip == 1){
        tries--;
      }
      else{
        break;
      }
    }

    // Check if an error occured at completion of the programming
    err = flashSpiReadData(CameraInterface::S25fl064l::RDSR2, (uint16_t*) &status2.all, 1);

    if(err != CAMERA_NO_ERROR){
      return err;
    }

    if(status2.bit.p_err){
      return CAMERA_FAIL_PAGE_PROGRAM;
    }

    // Disable writing to the device
    err = flashSpiWriteData(CameraInterface::S25fl064l::WRDI);

    return err;
  }

//  void CameraInterfaceComponentImpl :: fpgaSpiWrite(){
//    uint16_t addressLength;
//    uint16_t totalBytesToTransmit;
//
//    if(txData == NULL && sizeOfTxData > 0){
//      return CAMERA_UNEXPECTED_ERROR;
//    }
//
//    addressLength = getAddressLengthByte(cmd);
//    m_spiTxBuff[0] = (uint8_t) cmd;
//    totalBytesToTransmit = sizeOfTxData + 1 /*command*/ + addressLength;
//
//    if(addressLength > 0){
//      if(address == ADDRESS_NOT_DEFINED){
//        return CAMERA_UNEXPECTED_ERROR;
//      }
//
//      if(totalBytesToTransmit > SPI_TX_BUFFER_MAX_LENGTH){
//        return CAMERA_WRONG_DATA_SIZE;
//      }
//
//      // Copy the address section to the transmit buffer
//      memcpy(m_spiTxBuff+1, (uint8_t *)&address, addressLength);
//
//      if(txData == NULL){
//        return CAMERA_UNEXPECTED_ERROR;
//      }
//
//      if(txData != NULL && sizeOfTxData > 0){
//        // Copy data to transmit buffer
//        memcpy(m_spiTxBuff + 1 /*command */ + addressLength, txData, sizeOfTxData);
//      }
//    }
//
//    // Set CS low
//    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_FPGA, 0);
//
//    spiTransmitData(m_spi, &m_fpgaDataConfig, totalBytesToTransmit, (uint16_t *)&m_spiTxBuff);
//
//    // Set CS high
//    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_FPGA, 1);
//
//    return CAMERA_NO_ERROR;
//  }

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
