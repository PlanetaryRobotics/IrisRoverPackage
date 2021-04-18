#include "CY15B102Q.hpp"
#include "App/SPI.h"

CY15B102Q :: CY15B102Q(){
  // External FRAM SPI data configuration

  // Refer to datasheet, default number of dummy cycles between a SDI and SDO
  // is set by default to 8 clock cycles
  m_readLatencyCycles = DEFAULT_DUMMY_CYCLES;
  m_framSpi = SPI_REG_FRAM;
  m_framDataConfig.CS_HOLD = 0;
  m_framDataConfig.WDEL = 0;
  m_framDataConfig.DFSEL = SPI_FMT_0;
  m_framDataConfig.CSNR = 0b11111110;  // Each index corresponds to CS[i]. Value represents the CS level when a transaction is occurring (1: hign, 0: low). SPIDEF sets the non-transaction CS level.
                               // Note that SPI3 only has 6 CS pins, the upper 2 bits are don't care.
}

// Destructor: DO NOT USE
//CY15B102Q :: ~CY15B102Q(){}

/**
 * @brief      The function sets up the flash memory component
 *
 * @return     True if setup was fine, False if setup was incorrect
 */
CY15B102Q :: CY15B102QError CY15B102Q :: setupDevice()
{
  uint32_t totalBytesToRead;

  // Create Recieve buffer for ID
  uint8_t id[9];
  // Set bytes to size of id
  totalBytesToRead = 9;

  // Set Tx Buffer to Device ID Opcode
  FRAMSpiCommands id_opcode = RDID;
  m_spiTxBuff[0] = id_opcode & 0xff;


  framSpiReadData(id_opcode, (uint8_t *)id, totalBytesToRead, 0 /*no address needed*/);

  // Check that the device is connected and correct
  uint8_t id_cmp[9];
  // Must split device ID up as too big  to copy all at once
  uint64_t first_id = DEVICE_ID_1;
  memcpy(&id_cmp, &first_id, sizeof(first_id));
  // Add last byte to array
  id_cmp[8] = DEVICE_ID_2;

  if(!memcmp(&id, &id_cmp, 9))
      return CY15B102Q_INCORRECT_FLASH_MEMORY;

  return CY15B102Q_NO_ERROR;
}

/**
* @brief      Gets the read dummy cycles.
*
* @param[in]  cmd   The command
*
* @return     The read dummy cycles.
*/
uint8_t CY15B102Q :: getReadDummyCycles(const CY15B102Q::FRAMSpiCommands cmd){
  switch(cmd)
  {
    case     CY15B102Q :: WREN:    // Set Write Enable Latch
    case     CY15B102Q :: WRDI:    // Reset Write Enable Latch
    case     CY15B102Q :: RDSR:    // Read Status Register
    case     CY15B102Q :: WRSR:    // Write Status Register
    case     CY15B102Q :: READ:    // Read Memory Data
    case     CY15B102Q :: WRITE:   // Write Memory Data
    case     CY15B102Q :: SLEEP:   // Enter Sleep Mode
    case     CY15B102Q :: RDID:    // Read Device ID
      return 0;
    case     CY15B102Q :: FSRTD:   // Fast Read Memory Data
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
 * @param[in]  dataReadLength  The data read length
 * @param[in]  regStartAddr    The register start address
 * @param[in]  length          The length
 *
 * @return     CY15B102QError
 */
CY15B102Q :: CY15B102QError CY15B102Q :: framSpiReadData(const CY15B102Q::FRAMSpiCommands cmd,
                                                          uint8_t *rxData,
                                                          const uint16_t sizeOfRxData,
                                                          const uint32_t address)
{
  uint16_t addressLength;
  uint32_t totalBytesToTransmit;
  uint32_t totalBytesToRead;
  uint32_t i;

  if(rxData == NULL){
    return CY15B102Q_UNEXPECTED_ERROR;
  }

  totalBytesToRead = sizeOfRxData;

  // at first, we just send command + any dummy cycles (converted to number of bytes)
  totalBytesToTransmit = 1;
  m_spiTxBuff[0] = (uint8_t) cmd;

  if(cmd == RDID)
  	addressLength = 0;
  else
  	addressLength = AddressLengthByte;
  totalBytesToTransmit += addressLength;

  // Address must be defined if addressLength greater than 0
  if(addressLength > 0){
    if(address == ADDRESS_NOT_DEFINED){
      return CY15B102Q_UNEXPECTED_ERROR;
    }

  // Add dummy cycles required for some commands
  // Convert dummy cycles to number of bytes
  // HERCULES SPI ONLY GENERATE NUMBER OF DUMMY CYCLES MULTIPLE OF 8
  // Default value set by S25FL064 is 8
  //totalBytesToTransmit += getReadDummyCycles(cmd) >> 3;

  // Likely don't need this check
  if(totalBytesToTransmit > SPI_TX_BUFFER_MAX_LENGTH_FRAM)
      return CY15B102Q_WRONG_DATA_SIZE;

   // copy address to Tx buffer
   for(i=0; i<addressLength; i++){
       m_spiTxBuff[i+1] = address >> i*8 & 0xff;
   }
  }

  // Set CS low
  m_framDataConfig.CS_HOLD = 1;
  // Send transmission data

  // Special Command for Single Byte Send
  if (totalBytesToTransmit <= 1)
      spiTransmitOneByte(m_framSpi, &m_framDataConfig, (uint16_t *)&m_spiTxBuff);
  // Normal Command
  else
      spiTransmitData(m_framSpi, &m_framDataConfig, totalBytesToTransmit, (uint16_t *)&m_spiTxBuff);

  // Set CS high
  m_framDataConfig.CS_HOLD = 0;

  // Special Command for Single Byte Receive
  if (totalBytesToRead <= 1)
      spiReceiveOneByte(m_framSpi, &m_framDataConfig, (uint16_t *)&rxData);
  // Normal Command
  else
      spiReceiveData(m_framSpi, &m_framDataConfig, totalBytesToRead, (uint16_t *)rxData);

  return CY15B102Q_NO_ERROR;
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
CY15B102Q :: CY15B102QError CY15B102Q :: framSpiWriteData(const CY15B102Q::FRAMSpiCommands cmd,
                                                              uint8_t *txData,
                                                              const uint16_t sizeOfTxData,
                                                              const uint32_t address)
{
  uint16_t addressLength;
  uint16_t totalBytesToTransmit; 
  uint16_t i;

  if(txData == NULL && sizeOfTxData > 0){
    return CY15B102Q_UNEXPECTED_ERROR;
  }

  addressLength = AddressLengthByte;
  m_spiTxBuff[0] = (uint8_t) cmd;
  totalBytesToTransmit = sizeOfTxData + 1 /*command*/ + addressLength;

  if(addressLength > 0){
    if(address == ADDRESS_NOT_DEFINED){
      return CY15B102Q_UNEXPECTED_ERROR;
    }

    // Likely Don't need this check
    if(totalBytesToTransmit > SPI_TX_BUFFER_MAX_LENGTH_FRAM){
      return CY15B102Q_WRONG_DATA_SIZE;
    }

    // copy address to Tx buffer
    for(i=0; i<addressLength; i++){
        m_spiTxBuff[i+1] = address >> i*8 & 0xff;
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
  m_framDataConfig.CS_HOLD = 1;
  // Send transmission data

  // Special Command for Single Byte Transfer
  if (totalBytesToTransmit <= 8)
      spiTransmitOneByte(m_framSpi, &m_framDataConfig, (uint16_t *)&m_spiTxBuff);
  // Normal Command
  else
      spiTransmitData(m_framSpi, &m_framDataConfig, totalBytesToTransmit, (uint16_t *)&m_spiTxBuff);

  // Set CS high
  m_framDataConfig.CS_HOLD = 0;

  return CY15B102Q_NO_ERROR;
}
