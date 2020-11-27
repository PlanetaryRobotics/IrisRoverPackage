#include <S25fl512l.hpp>

S25fl512l :: S25fl512l(){
  // External Flash SPI data configuration
  // Clock configuration need to be:
  // Clock polarity unchecked
  // Clock phase checked
  m_flashDataConfig.CS_HOLD = false;
  m_flashDataConfig.DFSEL = SPI_FMT_0;
  m_flashDataConfig.WDEL = false;
  m_flashDataConfig.CSNR = 0;

  // Refer to datasheet, default number of dummy cycles between a SDI and SDO
  // is set by default to 8 clock cycles
  m_readLatencyCycles = DEFAULT_DUMMY_CYCLES;

  m_flashSpi = (spiBASE_t *)NULL;

  m_memAllocPointer = 0;
  // default setting of the external memory
  m_addressLengthFormat = S25fl512l::ADDRESS_LENGTH_3_BYTES;
}


/**
 * @brief      The fucntion sets up the flash memory component
 *
 * @return     The device error code
 */
S25fl512l::S25fl512lError S25fl512l :: setupDevice(){

  S25fl512lError err = S25fl512l_NO_ERROR;

  m_addressLengthFormat = S25fl512l::ADDRESS_LENGTH_3_BYTES;

  m_flashSpi = SPI_REG;

  // Check that the correct device is connected to the MCU
  uint8_t id[2];
  err = flashSpiReadData(S25fl512l::REMS, id, sizeof(id), 0);

  if(err != S25fl512l_NO_ERROR)
      return err;

  // Check that the device is connected and correct
  if(id[0] != FLASH_MANUFACTURER_ID ||
     id[1] != FLASH_DEVICE_ID)
      return S25fl512l_INCORRECT_FLASH_MEMORY;

  // Check read latency cycles. Read latency cycles are required for some read
  // operation on the device.

  ConfigurationRegister1 configRegister1;
  err = flashSpiReadData(S25fl512l::RDCR1, &configRegister1.all, 1);
  if(err != S25fl512l_NO_ERROR){
    return err;
  }
  m_readLatencyCycles = configRegister1.bit.lc;

  // TODO investigate why 4 byte is buggy
  // Set configuration register to write in 4-byte mode addressing.
  //uint8_t brwr = 0x80; // enable 4-byte addressing for writing
  //err = flashSpiWriteData(S25fl512l::BRWR, &brwr, sizeof(brwr));

  //m_addressLengthFormat = ADDRESS_LENGTH_4_BYTES;

  return err;
}

   /**
    * @brief      Gets the address length byte.
    *
    * @param[in]  cmd   The command
    *
    * @return     The address length byte.
    */
uint16_t S25fl512l :: getAddressLengthByte(const S25fl512l::FlashSpiCommands cmd){
  switch(cmd){
    case S25fl512l::RDID:   // Read ID
    case S25fl512l::RDQID:   // Read Quad ID
    case S25fl512l::RUID:   // Read Unique ID
    case S25fl512l::RDSR1:   // Read Status Register 1
    case S25fl512l::RDSR2:   // Read Status Register 2
    case S25fl512l::RDCR1:   // Read Control Register 1
    case S25fl512l::WRR:     // Write Register (Status -1 and configuration -1,2,3)
    case S25fl512l::WRDI:   // Write Disable
    case S25fl512l::WREN:   // Write enable for non volatile data change
    case S25fl512l::WRENV:   // Write enable for volatile status and configuration registers
    case S25fl512l::CLSR:   // Clear status register
    case S25fl512l::_4BEN:   // Enter 4 byte address mode
    case S25fl512l::_4BEX:   // Exit 4 byte address mode
    case S25fl512l::SBEL:   // Set burst length
    case S25fl512l::QPIEN:   // Enter QPI
    case S25fl512l::QPIEX:   // Exit QPI
    case S25fl512l::DLPRD:   // Data learning pattern read
    case S25fl512l::PDLRNV:   // Program NV data learning register
    case S25fl512l::WDLRV:   // Write volatile data learning center
    case S25fl512l::EPS:   // Erase / program suspend
    case S25fl512l::EPR:   // Erase / program resume
    case S25fl512l::GBL:   // Global IBL block
    case S25fl512l::GBUL:   // Global IBL unblock
    case S25fl512l::RSTEN:        // Software reset enable
    case S25fl512l::RST:        // Software reset
    case S25fl512l::MBR:        // Mode bit reset
    case S25fl512l::DPD:        // Deep power down
    case S25fl512l::RES:     //from deep power down / device idcase
    case S25fl512l::BRWR:
      return 0;
    case S25fl512l::RSFDP:   // Read JEDEC Serial Flash Discoverable parameters
    case S25fl512l::RDAR:
    case S25fl512l::WRAR:   // Write any register
    case S25fl512l::READ:   // Read
    case S25fl512l::FAST_READ:   // Fast read
    case S25fl512l::DOR:   // Dual output read
    case S25fl512l::QOR:   // Quad output read
    case S25fl512l::QIOR:   // Quad I/O read
    case S25fl512l::DIOR:   // Dual I/O read
    case S25fl512l::DDRQIOR:   // DDR Quad I/O read
    case S25fl512l::PP:   // Page program
    case S25fl512l::QPP:   // Quad page program
    case S25fl512l::SE:   // Sector erase
    case S25fl512l::BE:   // Block erase
    case S25fl512l::SECRE:   // Security region erase
    case S25fl512l::SECRP:   // Security region program
    case S25fl512l::SECRR:   // Security region read
    case S25fl512l::IBLRD:   // IBL read
    case S25fl512l::IBL:   // IBL lock
    case S25fl512l::IBUL:   // IBL unblock
    case S25fl512l::SPRP:   // Set pointer region protection
    case S25fl512l::REMS:
      return m_addressLengthFormat;
    case S25fl512l::_4READ:   // Read
    case S25fl512l::_4FAST_READ:   // Read
    case S25fl512l::_4DOR:   // Dual output read
    case S25fl512l::_4QOR:   // Dual output read
    case S25fl512l::_4DIOR:   // Dual I/O read
    case S25fl512l::_4QIOR:   // Quad I/O read
    case S25fl512l::_4DDRQIOR:   // DDR Quad I/O read
    case S25fl512l::_4PP:   // Page program
    case S25fl512l::_4QPP:   // Quad page program
    case S25fl512l::_4SE:   // Sector erase
    case S25fl512l::_4IBLRD:   // IBL read
    case S25fl512l::_4IBUL:   // IBL unblock
    case S25fl512l::_4IBL:   // IBL lock
    case S25fl512l::_4SPRP:   // Set pointer region protection
      return 4;
    default:
      return 0;
  }
}

/**
* @brief      Gets the read dummy cycles.
*
* @param[in]  cmd   The command
*
* @return     The read dummy cycles.
*/
uint8_t S25fl512l :: getReadDummyCycles(const S25fl512l::FlashSpiCommands cmd){
  switch(cmd){
    case S25fl512l::RDID:   // Read ID
    case S25fl512l::REMS:   // Read ID EMS
    case S25fl512l::RDQID:   // Read Quad ID
    case S25fl512l::RUID:   // Read Unique ID
    case S25fl512l::RDSR1:   // Read Status Register 1
    case S25fl512l::RDSR2:   // Read Status Register 2
    case S25fl512l::RDCR1:   // Read Control Register 1
    case S25fl512l::WRR:     // Write Register (Status -1 and configuration -1,2,3)
    case S25fl512l::WRDI:   // Write Disable
    case S25fl512l::WREN:   // Write enable for non volatile data change
    case S25fl512l::WRENV:   // Write enable for volatile status and configuration registers
    case S25fl512l::CLSR:   // Clear status register
    case S25fl512l::_4BEN:   // Enter 4 byte address mode
    case S25fl512l::_4BEX:   // Exit 4 byte address mode
    case S25fl512l::SBEL:   // Set burst length
    case S25fl512l::QPIEN:   // Enter QPI
    case S25fl512l::QPIEX:   // Exit QPI
    case S25fl512l::PDLRNV:   // Program NV data learning register
    case S25fl512l::WDLRV:   // Write volatile data learning center
    case S25fl512l::EPS:   // Erase / program suspend
    case S25fl512l::EPR:   // Erase / program resume
    case S25fl512l::GBL:   // Global IBL block
    case S25fl512l::GBUL:   // Global IBL unblock
    case S25fl512l::RSTEN:        // Software reset enable
    case S25fl512l::RST:        // Software reset
    case S25fl512l::MBR:        // Mode bit reset
    case S25fl512l::DPD:        // Deep power down
    case S25fl512l::RES:     //from deep power down / device idcase
    case S25fl512l::WRAR:   // Write any register
    case S25fl512l::READ:   // Read
    case S25fl512l::PP:   // Page program
    case S25fl512l::QPP:   // Quad page program
    case S25fl512l::SE:   // Sector erase
    case S25fl512l::BE:   // Block erase
    case S25fl512l::SECRE:   // Security region erase
    case S25fl512l::SECRP:   // Security region program
    case S25fl512l::IBLRD:   // IBL read
    case S25fl512l::IBL:   // IBL lock
    case S25fl512l::IBUL:   // IBL unblock
    case S25fl512l::SPRP:   // Set pointer region protection
    case S25fl512l::_4READ:   // Read
    case S25fl512l::_4PP:   // Page program
    case S25fl512l::_4QPP:   // Quad page program
    case S25fl512l::_4SE:   // Sector erase
    case S25fl512l::_4IBLRD:   // IBL read
    case S25fl512l::_4IBUL:   // IBL unblock
    case S25fl512l::_4IBL:   // IBL lock
    case S25fl512l::_4SPRP:   // Set pointer region protection
    case S25fl512l::DLPRD:   // Data learning pattern read
      return 0;
    case S25fl512l::RSFDP:   // Read JEDEC Serial Flash Discoverable parameters
    case S25fl512l::SECRR:   // Security region read
    case S25fl512l::RDAR:
    case S25fl512l::DDRQIOR:   // DDR Quad I/O read
    case S25fl512l::_4DDRQIOR:   // DDR Quad I/O read
    case S25fl512l::_4QIOR:   // Quad I/O read
    case S25fl512l::QIOR:   // Quad I/O read
    case S25fl512l::DIOR:   // Dual I/O read
    case S25fl512l::_4DIOR:   // Dual I/O read
    case S25fl512l::DOR:   // Dual output read
    case S25fl512l::_4DOR:   // Dual output read
    case S25fl512l::QOR:   // Quad output read
    case S25fl512l::_4QOR:   // Dual output read
    case S25fl512l::FAST_READ:   // Fast read
    case S25fl512l::_4FAST_READ:   // Read
      return m_readLatencyCycles;
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
 * @return     The device error code
 */
S25fl512l::S25fl512lError S25fl512l :: flashSpiReadData(const S25fl512l::FlashSpiCommands cmd,
                                                          uint8_t *rxData,
                                                          const uint16_t sizeOfRxData,
                                                          const S25fl512l::Address address){
  uint16_t addressLength;
  uint32_t totalBytesToTransmit;
  uint32_t totalBytesToRead;
  uint32_t i;

  if(rxData == NULL){
    return S25fl512l_UNEXPECTED_ERROR;
  }

  totalBytesToRead = sizeOfRxData;

  // at first, we just send command + any dummy cycles (converted to number of bytes)
  totalBytesToTransmit = 1;
  m_spiTxBuff[0] = (uint8_t) cmd;

  addressLength = getAddressLengthByte(cmd);
  totalBytesToTransmit += addressLength;

  // Address must be defined if addressLength greater than 0
  if(addressLength > 0){
    if(address == ADDRESS_NOT_DEFINED){
      return S25fl512l_UNEXPECTED_ERROR;
    }

  // Add dummy cycles required for some commands
  // Convert dummy cycles to number of bytes
  // HERCULES SPI ONLY GENERATE NUMBER OF DUMMY CYCLES MULTIPLE OF 8
  // Default value set by S25FL064 is 8
  totalBytesToTransmit += getReadDummyCycles(cmd) >> 3;

  if(totalBytesToTransmit > SPI_TX_BUFFER_MAX_LENGTH)
      return S25fl512l_WRONG_DATA_SIZE;

   // copy address to Tx buffer
   for(i=0; i<addressLength; i++){
       m_spiTxBuff[i+1] = address >> (addressLength-i-1)*8 & 0xff;
   }
  }

  // Set CS low
  gioSetBit(CS_SPI_PORT, CS_SPI_BIT, 0);

  // Send transmission data
  spiTransmitData(m_flashSpi, &m_flashDataConfig, totalBytesToTransmit, (uint16_t *)&m_spiTxBuff);

  while(totalBytesToRead > 0){
      // min(totalBytesToRead, SPI_RX_BUFFER_MAX_LENGTH)
      uint16_t bytesToRead = (totalBytesToRead < SPI_RX_BUFFER_MAX_LENGTH) ? 
                              totalBytesToRead : SPI_RX_BUFFER_MAX_LENGTH;
      spiReceiveData(m_flashSpi, &m_flashDataConfig, bytesToRead, (uint16_t *)m_spiRxBuff);

      // remove bytes to read
      totalBytesToRead -= bytesToRead;

      // copy bytes from rx buffer to destination buffer
      for(i=0; i<bytesToRead; i++){
          *rxData = m_spiRxBuff[i] & 0xff;
          rxData++;
      }
  }

  // Set CS high
  gioSetBit(CS_SPI_PORT, CS_SPI_BIT, 1);

  return S25fl512l_NO_ERROR;
}


/**
 * @brief      Write data flash device over SPI
 *
 * @param[in]  cmd     The command
 * @param      txData  The transmit data
 * @param[in]  length  The length
 *
 * @return     The device error code
 */
S25fl512l::S25fl512lError S25fl512l :: flashSpiWriteData(const S25fl512l::FlashSpiCommands cmd,
                                                              uint8_t *txData,
                                                              const uint16_t sizeOfTxData,
                                                              S25fl512l::Address address){
  uint16_t addressLength;
  uint16_t totalBytesToTransmit; 
  uint16_t i;

  if(txData == NULL && sizeOfTxData > 0){
    return S25fl512l_UNEXPECTED_ERROR;
  }

  addressLength = getAddressLengthByte(cmd);
  m_spiTxBuff[0] = (uint8_t) cmd;
  totalBytesToTransmit = sizeOfTxData + 1 /*command*/ + addressLength;

  if(addressLength > 0){
    if(address == ADDRESS_NOT_DEFINED){
      return S25fl512l_UNEXPECTED_ERROR;
    }

    if(totalBytesToTransmit > SPI_TX_BUFFER_MAX_LENGTH){
      return S25fl512l_WRONG_DATA_SIZE;
    }

    // copy address to Tx buffer
    for(i=0; i<addressLength; i++){
        m_spiTxBuff[i+1] = address >> (addressLength-i-1)*8 & 0xff;
    }

    if(txData != NULL && sizeOfTxData > 0){
      // Copy data to transmit buffer
      // copy address to Tx buffer
      for(i=0; i<sizeOfTxData; i++){
        m_spiTxBuff[i+1+addressLength] = *txData & 0xff;
        txData++;
      }
    }
  }

  // Set CS low
  gioSetBit(CS_SPI_PORT, CS_SPI_BIT, 0);

  spiTransmitData(m_flashSpi, &m_flashDataConfig, totalBytesToTransmit, (uint16_t *)&m_spiTxBuff);

  // Set CS high
  gioSetBit(CS_SPI_PORT, CS_SPI_BIT, 1);

  return S25fl512l_NO_ERROR;
}

/**
 * @brief      Suspend an erase or program operation
 *
 * @return     The device error code
 */
S25fl512l::S25fl512lError S25fl512l :: programEraseSuspend(){
  S25fl512l::StatusRegister1 status1;
  S25fl512l::StatusRegister2 status2;
  S25fl512lError err;

  // Read status register to check if program/erase is on-going
  err = flashSpiReadData(S25fl512l::RDSR1, &status1.all, 1);
  if(err != S25fl512l_NO_ERROR){
    return err;
  }

  err = flashSpiReadData(S25fl512l::RDSR2, &status2.all, 1);
  if(err != S25fl512l_NO_ERROR){
    return err;
  }

  // Check if erase or program are in progress, if not, there is nothing to
  // suspend Check if an erase suspend or program suspend is already
  // happening, if so, don't do anything.
  if(status1.bit.wip == 0 && (status2.bit.es || status2.bit.ps)){
    return S25fl512l_NO_ERROR;
  }

  // Send sector erase The flash spi write doesn't require to write any data,
  // only address is required
  err = flashSpiWriteData(S25fl512l::EPS);

  return err;
}


/**
 * @brief      Resume programming / erase
 *
 * @return     The device error code
 */
S25fl512l::S25fl512lError S25fl512l :: programEraseResume(){
  S25fl512l::StatusRegister1 status1;
  S25fl512l::StatusRegister2 status2;
  S25fl512lError err;

  // Read status register to check if program/erase is on-going
  err = flashSpiReadData(S25fl512l::RDSR1, &status1.all, 1);
  if(err != S25fl512l_NO_ERROR){
    return err;
  }

  err = flashSpiReadData(S25fl512l::RDSR2, &status2.all, 1);
  if(err != S25fl512l_NO_ERROR){
    return err;
  }

  // If nothing is in progress or suspended, return immediately
  if(status1.bit.wip == 0 && status2.bit.es == 0 && status2.bit.ps == 0){
    return S25fl512l_NO_ERROR;
  }

  // Send sector erase The flash spi write doesn't require to write any data,
  // only address is required
  err = flashSpiWriteData(S25fl512l::EPR);

  return err;
}

/**
 * @brief      Perform a soft reset of the device
 *
 * @return     The device error code
 */
S25fl512l::S25fl512lError S25fl512l :: resetDevice(){
  S25fl512lError err;

  // Enable reset command
  err = flashSpiWriteData(S25fl512l::RSTEN);
  if(err != S25fl512l_NO_ERROR){
    return err;
  }

  // Perform a soft reset of the device
  err = flashSpiWriteData(S25fl512l::RST);

  return err;
}

/**
 * @brief      Erase a sector
 *
 * @param[in]  sector  The sector
 *
 * @return     Return a device error
 */
S25fl512l::S25fl512lError S25fl512l :: sectorErase(const S25fl512l::Sector sector){
  S25fl512l::Address address;
  S25fl512l::StatusRegister1 status1;
  S25fl512lError err;
  uint32_t tries = __INT_MAX;

  if(sector > MAX_SECTOR_RANGE){
    return S25fl512l_UNEXPECTED_ERROR;
  }

  // Enable writing to the device
  err = flashSpiWriteData(S25fl512l::WREN);

  if(err != S25fl512l_NO_ERROR){
    return err;
  }

  // Read status register to check that write is enabled
  err = flashSpiReadData(S25fl512l::RDSR1, &status1.all, 1);
  
  if(err != S25fl512l_NO_ERROR){
    return err;
  }

  // Write Enable Latch must be set at that point
  if(status1.bit.wel == 0){
    return S25fl512l_FAIL_SECTOR_ERASE;
  }

  // Send sector erase
  // The flash spi write doesn't require to write any data, only address is required
  address = sector * SECTOR_SIZE;
  err = flashSpiWriteData(((m_addressLengthFormat == ADDRESS_LENGTH_4_BYTES) ? S25fl512l::_4SE :S25fl512l::SE), (uint8_t *)NULL, 0, address);

  if(err != S25fl512l_NO_ERROR){
    return err;
  }

  // Poll the device to check the "work in progress" flag from the status register 1
  while(tries > 0){
    err = flashSpiReadData(S25fl512l::RDSR1, &status1.all, 1);
    if(err != S25fl512l_NO_ERROR){
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
    return S25fl512l_FAIL_SECTOR_ERASE;
  }

  if(status1.bit.e_err){
    return S25fl512l_FAIL_PAGE_PROGRAM;
  }

  // Disable writing to the device
  err = flashSpiWriteData(S25fl512l::WRDI);

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
 * @return     The device error
 */
S25fl512l::S25fl512lError S25fl512l :: allocateFlashMemory(S25fl512l::MemAlloc *alloc,
                                                                const uint32_t size){

  S25fl512l::Address tmp = m_memAllocPointer;

  // Reset reserved memory to zero
  alloc->reservedSize = 0;

  // Calculate new address pointer by PAGE_SIZE increment
  tmp += (size / PAGE_SIZE) * PAGE_SIZE;
  tmp += (size % PAGE_SIZE) ? PAGE_SIZE : 0;

  // Check that new memory allocation pointer is within memory range 
  if(tmp > MAX_MEMORY_ADDRESS){
    return S25fl512l_FAIL_MEM_ALLOCATION;
  }

  //Update address with valid address pointer
  alloc->startAddress = m_memAllocPointer;

  // Update the reserved size (larger or equal to size requested)
  alloc->reservedSize = tmp - m_memAllocPointer;

  //Increment address pointer for next memory allocation
  m_memAllocPointer += tmp;

  return S25fl512l_NO_ERROR;
}


/**
 * @brief      Writes a data to flash.
 *
 * @param      alloc     The allocate
 * @param[in]  offset    The offset from the start address of the memory
 *                       allocation
 * @param      data      The data
 * @param[in]  dataSize  The data size
 *
 * @return     The device error
 */
S25fl512l::S25fl512lError S25fl512l :: writeDataToFlash(S25fl512l::MemAlloc *alloc,
                                                             const uint32_t offset,
                                                             uint8_t *data,
                                                             const uint16_t dataSize){
  S25fl512lError err;
  S25fl512l::Address sectorNumber;
  uint16_t sectorOverlaps = 0;
  uint16_t totalBytesToWrite = dataSize;

  // Check if memory allocation is valid
  if(alloc->reservedSize == 0){
    return S25fl512l_FAIL_WRITE_DATA_FLASH;
  }

  // Check if desired data write will fit in device memory range
  if(alloc->startAddress + offset + dataSize > MAX_MEMORY_ADDRESS){
    return S25fl512l_FAIL_WRITE_DATA_FLASH;
  }

  // Check if data to write fits allocated size
  if(dataSize > alloc->reservedSize){
    return S25fl512l_FAIL_WRITE_DATA_FLASH;
  }

  uint16_t pageWriteCounter = 0;
  //loop here until there is data to write or an error occurs
  while(1){

     uint16_t bytesToCopy = (totalBytesToWrite > PAGE_SIZE) ? PAGE_SIZE : totalBytesToWrite;

     for(int i=0; i< sizeof(m_writeScratchpad); i++){
         m_writeScratchpad[i] = 0;
     }

     memcpy(m_writeScratchpad, data, bytesToCopy);

    err = pageProgram(alloc->startAddress + pageWriteCounter*PAGE_SIZE, m_writeScratchpad, PAGE_SIZE);

    if(err != S25fl512l_NO_ERROR){
      return err;
    }

    //Substract the number of byte programmed from the number of bytes
    totalBytesToWrite -= bytesToCopy;

    // if there is still some bytes to write, go to next memory sector
    if(totalBytesToWrite > 0){
      pageWriteCounter++;
    }
    else{
      break;  // write completed
    }
  } // end of while loop

  return S25fl512l_NO_ERROR;
}


/**
 * @brief      Reads a data from flash.
 *
 * @param      alloc     The allocate
 * @param[in]  offset    The offset
 * @param      data      The data
 * @param[in]  dataSize  The data size
 *
 * @return     The device error
 */
S25fl512l::S25fl512lError S25fl512l :: readDataFromFlash(S25fl512l::MemAlloc *alloc,
                                                              const uint32_t offset,
                                                              uint8_t *data,
                                                              const uint16_t dataSize){
  S25fl512lError err;
  uint16_t totalBytesToRead = dataSize;
  uint16_t overflow = 0;

  while(totalBytesToRead){
    uint16_t bytesToRead = (totalBytesToRead < PAGE_SIZE) ? totalBytesToRead : PAGE_SIZE;

    // Read data from flash memory
    // Send the READ command
    err = flashSpiReadData( ((m_addressLengthFormat == ADDRESS_LENGTH_4_BYTES) ? S25fl512l::_4READ : S25fl512l::READ),
                            data + PAGE_SIZE * overflow,
                            bytesToRead,
                            alloc->startAddress + PAGE_SIZE * overflow + offset);

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
S25fl512l::S25fl512lError S25fl512l :: pageProgram(S25fl512l::Address address,
                                                        uint8_t *txData,
                                                        const uint16_t size){
  S25fl512l::StatusRegister1 status1;
  S25fl512lError err;
  uint32_t tries = __INT_MAX;

  // the size of a page is 256 bytes
  if(size > PAGE_SIZE){
    return S25fl512l_UNEXPECTED_ERROR;
  }

  // Check if pointer to transmit buffer exists
  if(txData == NULL){
    return S25fl512l_UNEXPECTED_ERROR;
  }

  // Enable writing to the device
  err = flashSpiWriteData(S25fl512l::WREN);

  if(err != S25fl512l_NO_ERROR){
    return err;
  }

  // Read status register
  err = flashSpiReadData(S25fl512l::RDSR1, &status1.all, 1);
  
  if(err != S25fl512l_NO_ERROR){
    return err;
  }

  // Write Enable Latch must be set at that point
  if(status1.bit.wel == 0){
    return S25fl512l_FAIL_PAGE_PROGRAM;
  }

  // Send data to perform page programming
  err = flashSpiWriteData(((m_addressLengthFormat == ADDRESS_LENGTH_4_BYTES) ? S25fl512l::_4PP : S25fl512l::PP) , txData, size, address);
  
  if(err != S25fl512l_NO_ERROR){
    return err;
  }

  // Poll the device to check the "work in progress" flag from the status
  // register 1
  while(tries > 0){
    err = flashSpiReadData(S25fl512l::RDSR1, &status1.all, 1);
    if(err != S25fl512l_NO_ERROR){
      return err;
    }

    if(status1.bit.wip == 1){
      tries--;
    }
    else{
      break;
    }
  }

  if(status1.bit.p_err){
    return S25fl512l_FAIL_PAGE_PROGRAM;
  }

  // Disable writing to the device
  err = flashSpiWriteData(S25fl512l::WRDI);

  return err;
}
