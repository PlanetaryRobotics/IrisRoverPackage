// ======================================================================
// \title  UWBComponentImpl.cpp
// \author alec
// \brief  cpp file for UWB component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/UWB/UWB.hpp>
#include "Fw/Types/BasicTypes.hpp"

#include <spi.h>
#include "gio.h"
#include "reg_spi.h"
#include "reg_het.h"
#include "App/SPI.h"
#include "mibspi.h"

/* SPI configuration */
static spiBASE_t *spi = spiREG5;
static U8 UWB_bit = 0;
static spiDAT1_t dataConfig = {
    .CS_HOLD = 1,
    .WDEL = 0,
    .DFSEL = SPI_FMT_0,
    .CSNR = 0b11111110   // Each index corresponds to CS[i]. Value represents the CS level when a transaction is occurring (1: hign, 0: low). SPIDEF sets the non-transaction CS level.
};

/* If we need to change clock speed */
static spiDAT1_t dataConfig1 = {
    .CS_HOLD = 0,
    .WDEL = 0,
    .DFSEL = SPI_FMT_1,
    .CSNR = 0b11111110   // Each index corresponds to CS[i]. Value represents the CS level when a transaction is occurring (1: hign, 0: low). SPIDEF sets the non-transaction CS level.
};

/* HET configuration */
// static hetBASE_t *het = hetREG1;
static U8 RST_bit = 7;

typedef int decaIrqStatus_t ; // Type for remembering IRQ status

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  UWBComponentImpl ::
#if FW_OBJECT_NAMES == 1
    UWBComponentImpl(
        const char *const compName
    ) :
      UWBComponentBase(compName)
#else
    UWBComponentImpl(void)
#endif
  {

  }

  void UWBComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    UWBComponentBase::init(queueDepth, instance);
    m_callbackId = 0;
    m_bytesSent = 0;
    // txrx_configure();
    // Get_Time_cmdHandler(0x01,0x01);

    /*
    while (true) {
        printf("low\n");
        gioSetBit(spiPORT5, UWB_bit, 0);
        __delay_cycles(12000000);
        printf("high\n");
        gioSetBit(spiPORT5, UWB_bit, 1);
        __delay_cycles(12000000);
    }
    */

    uint16_t spiTxCmd = 0x00;
    spiDAT1_t g_UWBDataConfig;

    uint16_t UWB_data[10];

    g_UWBDataConfig.CS_HOLD = false;
    g_UWBDataConfig.DFSEL = SPI_FMT_0;
    g_UWBDataConfig.WDEL = false;
    g_UWBDataConfig.CSNR = 0;

//    mibspiInit();

    uint16_t tx_buffer[] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};
    uint16_t rx_buffer[10];

//    mibspiSetData(mibspiREG5, 0, tx_buffer);
//    mibspiTransfer(mibspiREG5, 0);
//    while (!(mibspiIsTransferComplete(mibspiREG5, 0)));
//    mibspiGetData(mibspiREG5, 0, rx_buffer);

//    printf("mib Device is %x\n", rx_buffer[0]);

//    gioSetDirection(hetPORT1, 0x01 << 7); // Do we need to make sure it's an output before reseting the UWB?

    gioSetBit(hetPORT1, 7, 0); // Conversely, do we need to not be reseting it?

    gioSetDirection(hetPORT1, 0x00); // Errr we probably don't want to set the *entire* Port1 to input... May fuck with other components.
                                     // That being said... I wonder if another component is fucking with UWB...

    while (true) {
        uint32 rst_val = gioGetBit(hetPORT1, 7);

        printf("rst is %d\n", rst_val);
        // uint8 dev_id = read_register(spiREG5, &dataConfig, 0x00);

        // CS WORKS
        // THIS PULLS CS ACTIVE LOW
        (&g_UWBDataConfig)->CS_HOLD = 1; /* CS_HOLD starts true to send next frame */
        uint16 dataBuf[20];
        uint8 rx_data_buff[20];

        uint16 cmd = 0x00;
        spiTransmitOneByte(spi, &g_UWBDataConfig, &cmd);

        spiReceiveOneByte(spi, &g_UWBDataConfig, dataBuf);
        spiReceiveOneByte(spi, &g_UWBDataConfig, dataBuf + 1);
        spiReceiveOneByte(spi, &g_UWBDataConfig, dataBuf + 2);
        spiReceiveOneByte(spi, &g_UWBDataConfig, dataBuf + 3);
//        spiReceiveData(spi, &g_UWBDataConfig, 4, dataBuf);
        rx_data_buff[0] = (uint8) (dataBuf[0] & 0xFF);
        rx_data_buff[1] = (uint8) (dataBuf[1] & 0xFF);
        rx_data_buff[2] = (uint8) (dataBuf[2] & 0xFF);
        rx_data_buff[3] = (uint8) (dataBuf[3] & 0xFF);

        (&g_UWBDataConfig)->CS_HOLD = 0; /* This is the last frame */

        printf("Device is %x%x%x%x\n", rx_data_buff[0], rx_data_buff[1], rx_data_buff[2], rx_data_buff[3]);

        spiReceiveOneByte(spi, &g_UWBDataConfig, UWB_data);

        printf("Device is \n");



        // THIS PULLS CS inactive HIGH

        // (&g_UWBDataConfig)->CS_HOLD = 0; /* This is the last frame */
        // spiReceiveOneByte(spi, &g_UWBDataConfig, dataBuf);

        // send data
        // spiTransmitData(spiREG5, &g_UWBDataConfig, 1, &spiTxCmd);

        // gioSetBit(spiPORT5, 0, 0); // set CS HIGH

        // UWB_data[0] = 0;

        // spiReceiveOneByte(spiREG5, &g_UWBDataConfig, UWB_data);

        // uint16_t data3 = UWB_data[0];

        // printf("Device is %x\n", data3);

        // printf("help%d\n", dev_id);
        // uint8 id = read_register(spiREG5, &dataConfig, SYS_STATUS_ID);
        // printf("%d\n",id);
        // printf("Value: %x\n", dwt_read32bitreg(SYS_STATUS_ID));
        // __delay_cycles(12000000);
        // printf("check device id\n");
        // printf("Value: %x\n", dwt_readdevid());
         __delay_cycles(12000000);
    }
  }

  UWBComponentImpl ::
    ~UWBComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void UWBComponentImpl ::
    PingIn_handler(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    this->PingOut_out(0, key);
  }

  // ----------------------------------------------------------------------
  // Static Variable Declarations
  // ----------------------------------------------------------------------

  UWBComponentImpl::dwt_config_t UWBComponentImpl::config = {
                                                            5,                /* Channel number. */
                                                            DWT_PRF_64M,      /* Pulse repetition frequency. */
                                                            DWT_PLEN_128,     /* Preamble length. Used in TX only. */
                                                            DWT_PAC8,         /* Preamble acquisition chunk size. Used in RX only. */
                                                            10,               /* TX preamble code. Used in TX only. */
                                                            10,               /* RX preamble code. Used in RX only. */
                                                            0,                /* 0 to use standard SFD, 1 to use non-standard SFD. */
                                                            DWT_BR_6M8,       /* Data rate. */
                                                            DWT_PHRMODE_STD,  /* PHY header mode. */
                                                            (129 + 8 - 8)     /* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
                                                             };

  uint8 UWBComponentImpl::frame_seq_nb = 0;

  /* Hold copy of status register state here for reference so that it can be examined at a debug breakpoint. */
  uint32 UWBComponentImpl::status_reg = 0;

      /* Frames used in the ranging process. See NOTE 2,3 below. */
  uint8 UWBComponentImpl::expected_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 0, MY_ID, 0, 1 - MY_ID, 0xE0, 0, 0};
  uint8 UWBComponentImpl::outgoing_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 0, 1 - MY_ID, 0, MY_ID, 0xE0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  uint8 UWBComponentImpl::final_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 0, 1 - MY_ID, 0, MY_ID, 0xE0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  UWBComponentImpl::dwt_local_data_t UWBComponentImpl::dw1000local[DWT_NUM_DW_DEV];

  UWBComponentImpl::dwt_local_data_t * UWBComponentImpl::pdw1000local = dw1000local;

  uint8 UWBComponentImpl::rx_buffer[RX_BUF_LEN];

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

    /* Function name: readfromspi
   * Input: A uint16 (length of the header), a uint8 pointer (pointer to header buffer),
            a uint32 (number of bytes read) and a uint8 pointer (buffer pointer to return read data)
   * Output: None
   * Returns: An int (returns 0 if successful)
   * Purpose: This function reads the DW1000 device registers using the header provided and
   *          stores the read data in the input buffer
   */

  int UWBComponentImpl ::
      readfromspi(
          uint16 headerLength,
          const uint8 *headerBuffer,
          uint32 readlength,
          uint8 *readBuffer
      )
  {
      uint16 * p1;
      uint32 idatalength = 0;


      /*** Previous Code ***/

      /*

      idatalength = headerLength + readlength;

      //uint8 idatabuf[idatalength];
      //uint8 itempbuf[idatalength];
      uint16 idatabuf[idatalength];
      uint16 itempbuf[idatalength];

      memset(idatabuf, 0, idatalength);
      memset(itempbuf, 0, idatalength);

      p1 = idatabuf;

      for (size_t i = 0; i < headerLength; i++) {
          p1[i] = (uint16)headerBuffer[i];
      }

      p1 += headerLength;
      memset(p1, 0x00, readlength);

      spiTransmitData(spi, &dataConfig, idatalength, idatabuf);

      p1 = itempbuf + headerLength;

      spiReceiveData(spi, &dataConfig, idatalength, itempbuf);

      for (size_t i = 0; i < readlength; i++) {
          readBuffer[i] = (uint8)p1[i];
      }

      */

      /*** End previous code ***/

      uint16 idatabuf[headerLength];
      uint16 itempbuf[readlength];

      memset(idatabuf, 0, headerLength);
      memset(itempbuf, 0, readlength);

      p1 = idatabuf;

      for (size_t i = 0; i < headerLength; i++) {
          p1[i] = (uint16)headerBuffer[i];
      }

      gioSetBit(spiPORT5, UWB_bit, 0);

      // spiTransmitData(spi, &dataConfig, headerLength, idatabuf);

      spiTransmitOneByte(spi, &dataConfig, idatabuf);

      spiReceiveData(spi, &dataConfig, readlength, itempbuf);

      gioSetBit(spiPORT5, UWB_bit, 1);

      for (size_t i = 0; i < readlength; i++) {
          readBuffer[i] = (uint8)p1[i];
       }

      //memcpy(p1,headerBuffer, headerLength);

      //spi_xfer_done = false;
      // Transmit function
      // One Byte: uint32 spiTransmitOneByte(spi, dataConfig, uint16* srcbuff)
      // Multibyte: uint32 spiTransmitData(spi, dataConfig, uint32 blocksize, uint16 * srcbuff);

      // Receive function
      // One Byte: uint32 spiReceiveOneByte(spi, dataConfig, uint16 *destbuff)
      // Multibyte: uint32 spiReceiveData(spi, dataConfig, sizeof(times), static_cast<uint16_t*>(&times));
      // nrf_drv_spi_transfer(&spi, idatabuf, idatalength, itempbuf, idatalength);

      // while (!spi_xfer_done)
      // ;

      //memcpy(readBuffer, p1, readlength);

      return 0;
  }

  /* Function name: writetospi
   * Input: A uint16 (length of the header), a uint8 pointer (pointer to header buffer),
            a uint32 (number of bytes read) and a uint8 pointer (buffer pointer containing data to be written)
   * Output: None
   * Returns: An int (returns 0 if successful)
   * Purpose: This function writes to the DW1000 device registers using the header and data provided
   */
  int UWBComponentImpl ::
      writetospi(
          uint16 headerLength,
          const uint8 *headerBuffer,
          uint32 bodylength,
          uint8 *bodyBuffer
      )
  {
      uint16 * p1;
      uint32 idatalength=0;

      idatalength = headerLength + bodylength;

      //uint8 idatabuf[idatalength];
      //uint8 itempbuf[idatalength];
      uint16 idatabuf[idatalength];
      uint16 itempbuf[idatalength];


      memset(idatabuf, 0, idatalength);
      memset(itempbuf, 0, idatalength);

      p1=idatabuf;
      //memcpy(p1,headerBuffer, headerLength);
      for (size_t i = 0; i < headerLength; i++) {
          p1[i] = (uint16)headerBuffer[i];
      }
      p1 += headerLength;
      //memcpy(p1,bodyBuffer,bodylength);
      for (size_t i = 0; i < bodylength; i++) {
          p1[i] = (uint16)bodyBuffer[i];
      }

      //spi_xfer_done = false;
      //nrf_drv_spi_transfer(&spi, idatabuf, idatalength, itempbuf, idatalength);
      //while(!spi_xfer_done)
      //                        ;
      spiTransmitData(spi, &dataConfig, idatalength, idatabuf);
      /* Test if this is necessary */
      spiReceiveData(spi, &dataConfig, idatalength, itempbuf);

      return 0;
  }

  /* Function name: dwt_readfromdevice
   * Input: A uint16 (register or buffer ID), a uint16 (byte index of register or buffer),
            a uint32 (number of bytes read) and a uint8 pointer (buffer pointer to return read data)
   * Output: None
   * Returns: None
   * Purpose: This function is used to read from the DW1000 device registers
   *          Notes:
   *              1. Firstly we create a header (the first byte is a header byte)
   *                  a. check if sub index is used, if subindexing is used - set bit-6 to 1 to signify that the sub-index address follows the register index byte
   *                  b. set bit-7 (or with 0x80) for write operation
   *                  c. if extended sub address index is used (i.e. if index > 127) set bit-7 of the first sub-index byte following the first header byte
   *              2. Write the header followed by the data bytes to the DW1000 device
   *              3. Store the read data in the input buffer
   */
  void UWBComponentImpl ::
      dwt_readfromdevice(
          uint16  recordNumber,
          uint16  index,
          uint32  length,
          uint8   *buffer
      )
  {
      uint8 header[3]; // Buffer to compose header in
      int cnt = 0; // Counter for length of header

      /* Write message header selecting READ operation and addresses as appropriate (this is one to three bytes long) */
      if (index == 0) {/* For index of 0, no sub-index is required */
          header[cnt++] = (uint8)recordNumber; /* Bit-7 zero is READ operation, bit-6 zero=NO sub-addressing, bits 5-0 is reg file id */
      }
      else {
          header[cnt++] = (uint8)(0x40 | recordNumber); /* Bit-7 zero is READ operation, bit-6 one=sub-address follows, bits 5-0 is reg file id */

          if (index <= 127) { /* For non-zero index < 127, just a single sub-index byte is required */
              header[cnt++] = (uint8) index ; /* Bit-7 zero means no extension, bits 6-0 is index. */
          }
          else {
              header[cnt++] = 0x80 | (uint8)(index); /* Bit-7 one means extended index, bits 6-0 is low seven bits of index. */
              header[cnt++] = (uint8)(index >> 7); /* 8-bit value = high eight bits of index. */
          }
      }

      /* Do the read from the SPI */
      readfromspi(cnt, header, length, buffer);  /* result is stored in the buffer */
  } // end dwt_readfromdevice()

  /* Function name: dwt_writetodevice
   * Input: A uint16 (register or buffer ID), a uint16 (byte index of register or buffer),
            a uint32 (number of bytes read) and a uint8 pointer (buffer pointer containing data to be written)
   * Output: None
   * Returns: None/void
   * Purpose: This function is used to write to the DW1000 device registers
   *          Notes:
   *              1. Firstly we create a header (the first byte is a header byte)
   *                  a. check if sub index is used, if subindexing is used - set bit-6 to 1 to signify that the sub-index address follows the register index byte
   *                  b. set bit-7 (or with 0x80) for write operation
   *                  c. if extended sub address index is used (i.e. if index > 127) set bit-7 of the first sub-index byte following the first header byte
   *              2. Write the header followed by the data bytes to the DW1000 device
   */
  void UWBComponentImpl ::
      dwt_writetodevice(
          uint16  recordNumber,
          uint16  index,
          uint32  length,
          uint8   *buffer
      )
  {
      uint8 header[3] ; // Buffer to compose header in
      int   cnt = 0; // Counter for length of header

      // Write message header selecting WRITE operation and addresses as appropriate (this is one to three bytes long)
      if (index == 0) // For index of 0, no sub-index is required
      {
          header[cnt++] = 0x80 | recordNumber ; // Bit-7 is WRITE operation, bit-6 zero=NO sub-addressing, bits 5-0 is reg file id
      }
      else
      {
          header[cnt++] = 0xC0 | recordNumber ; // Bit-7 is WRITE operation, bit-6 one=sub-address follows, bits 5-0 is reg file id

          if (index <= 127) // For non-zero index < 127, just a single sub-index byte is required
          {
              header[cnt++] = (uint8)index ; // Bit-7 zero means no extension, bits 6-0 is index.
          }
          else
          {
              header[cnt++] = 0x80 | (uint8)(index) ; // Bit-7 one means extended index, bits 6-0 is low seven bits of index.
              header[cnt++] =  (uint8) (index >> 7) ; // 8-bit value = high eight bits of index.
          }
      }

      // Write it to the SPI
      writetospi(cnt,header,length,buffer);
  } // end dwt_writetodevice()

  /* Function name: dwt_read8bitoffsetreg
   * Input: An int (register File ID) and an int (register index)
   * Output: None
   * Returns: An uint8 (register value)
   * Purpose: This function is used to read 8-bit values from the DW1000 device registers
   */
  uint8 UWBComponentImpl ::
    dwt_read8bitoffsetreg(
        int regFileID,
        int regOffset
    )
  {
    uint8 regval;

    dwt_readfromdevice(regFileID, regOffset, 1, &regval);

    return regval ;
  }


  /* Function name: dwt_read16bitoffsetreg
   * Input: An int (register File ID) and an int (register index)
   * Output: None
   * Returns: An uint16 (register value)
   * Purpose: This function is used to read 16-bit values from the DW1000 device registers
   */
  uint16 UWBComponentImpl ::
      dwt_read16bitoffsetreg(
          int regFileID,
          int regOffset
      )
  {
      uint16  regval = 0;
      uint8   buffer[2];

      dwt_readfromdevice(regFileID,regOffset,2,buffer); /* Read 2 bytes (16-bits) register into buffer */

      regval = (buffer[1] << 8) + buffer[0];
      return regval;

  } // end dwt_read16bitoffsetreg()


  /* Function name: dwt_read32bitoffsetreg
   * Input: An int (register File ID) and an int (register index)
   * Output: None
   * Returns: An uint32 (register value)
   * Purpose: This function is used to read 32-bit values from the DW1000 device registers
   */
  uint32 UWBComponentImpl ::
      dwt_read32bitoffsetreg(
          int regFileID,
          int regOffset
      )
  {
      uint32  regval = 0;
      int     j;
      uint8   buffer[4];

      dwt_readfromdevice(regFileID,regOffset,4,buffer); /* Read 4 bytes (32-bits) register into buffer */

      for (j = 3; j >= 0; j--) {
          regval = (regval << 8) + buffer[j];
      }
      return regval;

  } // end dwt_read32bitoffsetreg()

  /* Function name: dwt_write8bitoffsetreg
   * Input: An int (register File ID), an int (register index) and an uint8 (value to write)
   * Output: None
   * Returns: None
   * Purpose: This function is used to write 8-bit values from the DW1000 device registers
   */
  void UWBComponentImpl ::
      dwt_write8bitoffsetreg(
          int regFileID,
          int regOffset,
          uint8 regval
      )
  {
      dwt_writetodevice(regFileID, regOffset, 1, &regval);
  }

  /* Function name: dwt_write16bitoffsetreg
   * Input: An int (register File ID), an int (register index) and an uint16 (value to write)
   * Output: None
   * Returns: None
   * Purpose: This function is used to write 16-bit values from the DW1000 device registers
   */
  void UWBComponentImpl ::
      dwt_write16bitoffsetreg(
          int regFileID,
          int regOffset,
          uint16 regval
      )
  {
      uint8   buffer[2];

      buffer[0] = regval & 0xFF;
      buffer[1] = regval >> 8;

      dwt_writetodevice(regFileID, regOffset, 2, buffer);
  } // end dwt_write16bitoffsetreg()

  /* Function name: dwt_write32bitoffsetreg
   * Input: An int (register File ID), an int (register index) and an uint8 (value to write)
   * Output: None
   * Returns: None
   * Purpose: This function is used to write 32-bit values from the DW1000 device registers
   */
  void UWBComponentImpl ::
      dwt_write32bitoffsetreg(
          int regFileID,
          int regOffset,
          uint32 regval
      )
  {
      int     j;
      uint8   buffer[4];

      for (j = 0 ; j < 4 ; j++) {
          buffer[j] = regval & 0xff ;
          regval >>= 8 ;
      }

      dwt_writetodevice(regFileID, regOffset, 4, buffer);
  } // end dwt_write32bitoffsetreg()

  /* Function name: dwt_readdevid
   * Input: None/void
   * Output: None
   * Returns: An uint32 (the read value which for DW1000 is 0xDECA0130)
   * Purpose: This function is used to return the read device type and revision information of the DW1000 device (MP part is 0xDECA0130)
   */
  uint32 UWBComponentImpl ::
      dwt_readdevid()
  {
      return dwt_read32bitoffsetreg(DEV_ID_ID,0);
  }

  /* Function name: _dwt_enableclocks
   * Input: A int (clocks - set of clocks to enable/disable)
   * Output: None
   * Returns: None/void
   * Purpose: This function enables/disables clocks to particular digital blocks/system
   */
  void UWBComponentImpl ::
      _dwt_enableclocks(
          int clocks
      )
  {
      uint8 reg[2];

      dwt_readfromdevice(PMSC_ID, PMSC_CTRL0_OFFSET, 2, reg);
      switch(clocks)
      {
          case ENABLE_ALL_SEQ:
          {
              reg[0] = 0x00 ;
              reg[1] = reg[1] & 0xfe;
          }
          break;
          case FORCE_SYS_XTI:
          {
              // System and RX
              reg[0] = 0x01 | (reg[0] & 0xfc);
          }
          break;
          case FORCE_SYS_PLL:
          {
              // System
              reg[0] = 0x02 | (reg[0] & 0xfc);
          }
          break;
          case READ_ACC_ON:
          {
              reg[0] = 0x48 | (reg[0] & 0xb3);
              reg[1] = 0x80 | reg[1];
          }
          break;
          case READ_ACC_OFF:
          {
              reg[0] = reg[0] & 0xb3;
              reg[1] = 0x7f & reg[1];
          }
          break;
          case FORCE_OTP_ON:
          {
              reg[1] = 0x02 | reg[1];
          }
          break;
          case FORCE_OTP_OFF:
          {
              reg[1] = reg[1] & 0xfd;
          }
          break;
          case FORCE_TX_PLL:
          {
              reg[0] = 0x20 | (reg[0] & 0xcf);
          }
          break;
          case FORCE_LDE:
          {
              reg[0] = 0x01;
              reg[1] = 0x03;
          }
          break;
          default:
          break;
      }


      // Need to write lower byte separately before setting the higher byte(s)
      dwt_writetodevice(PMSC_ID, PMSC_CTRL0_OFFSET, 1, &reg[0]);
      dwt_writetodevice(PMSC_ID, 0x1, 1, &reg[1]);

  } // end _dwt_enableclocks()

  /* Function name: _dwt_disablesequecing
   * Input: None/Void
   * Output: None
   * Returns: None/Void
   * Purpose: This function disables the TX blocks sequencing, it disables PMSC control of RF blocks, system clock is also set to XTAL
   */
  void UWBComponentImpl ::
      _dwt_disablesequencing() // Disable sequencing and go to state "INIT"
  {
      _dwt_enableclocks(FORCE_SYS_XTI); // Set system clock to XTI

      dwt_write16bitoffsetreg(PMSC_ID, PMSC_CTRL1_OFFSET, PMSC_CTRL1_PKTSEQ_DISABLE); // Disable PMSC ctrl of RF and RX clk blocks
  }

  /* Function name: _dwt_aonarrayupload
   * Input: None/Void
   * Output: None
   * Returns: None/Void
   * Purpose: This function uploads always on (AON) data array and configuration. Thus if this function is used, then _dwt_aonconfigupload
   * is not necessary. The DW1000 will go so SLEEP straight after this if the DWT_SLP_EN has been set.his function resets the DW1000
   */
  void UWBComponentImpl ::
      _dwt_aonarrayupload()
  {
      dwt_write8bitoffsetreg(AON_ID, AON_CTRL_OFFSET, 0x00); // Clear the register
      dwt_write8bitoffsetreg(AON_ID, AON_CTRL_OFFSET, AON_CTRL_SAVE);
  }

  /* Function name: deca_sleep
   * Input: An unsigned int (time in ms)
   * Output: None
   * Returns: None/void
   * Purpose: This function tells the DW1000 to sleep for the specified period of time
   */
  void UWBComponentImpl ::
      deca_sleep(
          unsigned int time_ms
      )
  {
      //nrf_delay_ms(time_ms);
      /* Check if this function works */
      for (size_t i = 0; i < time_ms; i++) {
          __delay_cycles(220000);
      }
  }

  /* Function name: dwt_softreset
   * Input: None/Void
   * Output: None
   * Returns: None/Void
   * Purpose: This function resets the DW1000
   */
  void UWBComponentImpl ::
      dwt_softreset()
  {
      _dwt_disablesequencing();

      // Clear any AON auto download bits (as reset will trigger AON download)
      dwt_write16bitoffsetreg(AON_ID, AON_WCFG_OFFSET, 0x00);
      // Clear the wake-up configuration
      dwt_write8bitoffsetreg(AON_ID, AON_CFG0_OFFSET, 0x00);
      // Upload the new configuration
      _dwt_aonarrayupload();

      // Reset HIF, TX, RX and PMSC
      dwt_write8bitoffsetreg(PMSC_ID, PMSC_CTRL0_SOFTRESET_OFFSET, PMSC_CTRL0_RESET_ALL);

      // DW1000 needs a 10us sleep to let clk PLL lock after reset - the PLL will automatically lock after the reset
      // Could also have polled the PLL lock flag, but then the SPI needs to be < 3MHz !! So a simple delay is easier
      deca_sleep(1);

      // Clear reset
      dwt_write8bitoffsetreg(PMSC_ID, PMSC_CTRL0_SOFTRESET_OFFSET, PMSC_CTRL0_RESET_CLEAR);

      pdw1000local->wait4resp = 0;
  }

  /* Function name: _dwt_otpread
   * Input: A uint32 (address to read at)
   * Output: None
   * Returns: A uint32 (the 32bit of read data)
   * Purpose: This function reads the OTP memory and ensures that MR,MRa,MRb are reset to 0.
   */
  uint32 UWBComponentImpl ::
      _dwt_otpread(
          uint32 address
      )
  {
      uint32 ret_data;

      // Write the address
      dwt_write16bitoffsetreg(OTP_IF_ID, OTP_ADDR, address);

      // Perform OTP Read - Manual read mode has to be set
      dwt_write8bitoffsetreg(OTP_IF_ID, OTP_CTRL, OTP_CTRL_OTPREAD | OTP_CTRL_OTPRDEN);
      dwt_write8bitoffsetreg(OTP_IF_ID, OTP_CTRL, 0x00); // OTPREAD is self clearing but OTPRDEN is not

      // Read read data, available 40ns after rising edge of OTP_READ
      ret_data = dwt_read32bitoffsetreg(OTP_IF_ID, OTP_RDAT);

      // Return the 32bit of read data
      return ret_data;
  }

  /* Function name: dwt_setxtaltrim
   * Input: A uint8 (crystal trim value (in (in range 0x0 to 0x1F) 31 steps (~1.5ppm per step))
   * Output: None
   * Returns: None/Void
   * Purpose: This function adjusts the crystal frequency
   */
  void UWBComponentImpl ::
      dwt_setxtaltrim(
          uint8 value
      )
  {
      // The 3 MSb in this 8-bit register must be kept to 0b011 to avoid any malfunction.
      uint8 reg_val = (3 << 5) | (value & FS_XTALT_MASK);
      dwt_write8bitoffsetreg(FS_CTRL_ID, FS_XTALT_OFFSET, reg_val);
  }

  /* Function name: _dwt_loaducodefromrom
   * Input: None
   * Output: None
   * Returns: None/Void
   * Purpose: This function loads ucode from OTP MEMORY or ROM
   */
  void UWBComponentImpl ::
      _dwt_loaducodefromrom()
  {
      // Set up clocks
      _dwt_enableclocks(FORCE_LDE);

      // Kick off the LDE load
      dwt_write16bitoffsetreg(OTP_IF_ID, OTP_CTRL, OTP_CTRL_LDELOAD); // Set load LDE kick bit

      deca_sleep(1); // Allow time for code to upload (should take up to 120 us)

      // Default clocks (ENABLE_ALL_SEQ)
      _dwt_enableclocks(ENABLE_ALL_SEQ); // Enable clocks for sequencing
  }

  /* Function name: dwt_initialise
   * Input: A uint16 (config)
   *        Configurations:   DWT_LOADUCODE     0x1 - load the LDE microcode from ROM - enabled accurate RX timestamp
   *                           DWT_LOADNONE      0x0 - do not load any values from OTP memory
   * Output: None
   * Returns: An int (DWT_SUCCESS for success, or DWT_ERROR for error)
   * Purpose: This function initiates communications with the DW1000 transceiver
   *          and reads its DEV_ID register (address 0x00) to verify the IC is one supported
   *          by this software (e.g. DW1000 32-bit device ID value is 0xDECA0130).  Then it
   *          does any initial once only device configurations needed for use and initialises
   *          as necessary any static data items belonging to this low-level driver
   *          NOTES:
   *          1.this function needs to be run before dwt_configuresleep, also the SPI frequency has to be < 3MHz
   *          2.it also reads and applies LDO tune and crystal trim values from OTP memory
   */

  int UWBComponentImpl ::
      dwt_initialise(
          uint16 config
      )
  {
    uint16 otp_addr = 0;
    uint32 ldo_tune = 0;

    pdw1000local = dw1000local;

    pdw1000local->dblbuffon = 0; // Double buffer mode off by default
    pdw1000local->wait4resp = 0;
    pdw1000local->sleep_mode = 0;

    pdw1000local->cbTxDone = NULL;
    pdw1000local->cbRxOk = NULL;
    pdw1000local->cbRxTo = NULL;
    pdw1000local->cbRxErr = NULL;

    // Read and validate device ID return -1 if not recognised
    if (DWT_DEVICE_ID != dwt_readdevid()) // MP IC ONLY (i.e. DW1000) FOR THIS CODE
    {
        printf("device id %x\n",dwt_readdevid());
        return DWT_ERROR ;
    }

    // Make sure the device is completely reset before starting initialisation
    dwt_softreset();

    _dwt_enableclocks(FORCE_SYS_XTI); // NOTE: set system clock to XTI - this is necessary to make sure the values read by _dwt_otpread are reliable

    // Configure the CPLL lock detect
    dwt_write8bitoffsetreg(EXT_SYNC_ID, EC_CTRL_OFFSET, EC_CTRL_PLLLCK);

    // Read OTP revision number
    otp_addr = _dwt_otpread(XTRIM_ADDRESS) & 0xffff;        // Read 32 bit value, XTAL trim val is in low octet-0 (5 bits)
    pdw1000local->otprev = (otp_addr >> 8) & 0xff;            // OTP revision is next byte

    // Load LDO tune from OTP and kick it if there is a value actually programmed.
    ldo_tune = _dwt_otpread(LDOTUNE_ADDRESS);
    if((ldo_tune & 0xFF) != 0)
    {
        // Kick LDO tune
        dwt_write8bitoffsetreg(OTP_IF_ID, OTP_SF, OTP_SF_LDO_KICK); // Set load LDE kick bit
        pdw1000local->sleep_mode |= AON_WCFG_ONW_LLDO; // LDO tune must be kicked at wake-up
    }

    // Load Part and Lot ID from OTP
    pdw1000local->partID = _dwt_otpread(PARTID_ADDRESS);
    pdw1000local->lotID = _dwt_otpread(LOTID_ADDRESS);

    // XTAL trim value is set in OTP for DW1000 module and EVK/TREK boards but that might not be the case in a custom design
    pdw1000local->init_xtrim = otp_addr & 0x1F;
    if (!pdw1000local->init_xtrim) // A value of 0 means that the crystal has not been trimmed
    {
        pdw1000local->init_xtrim = FS_XTALT_MIDRANGE ; // Set to mid-range if no calibration value inside
    }
    // Configure XTAL trim
    dwt_setxtaltrim(pdw1000local->init_xtrim);

    // Load leading edge detect code
    if(config & DWT_LOADUCODE)
    {
        _dwt_loaducodefromrom();
        pdw1000local->sleep_mode |= AON_WCFG_ONW_LLDE; // microcode must be loaded at wake-up
    }
    else // Should disable the LDERUN enable bit in 0x36, 0x4
    {
        uint16 rega = dwt_read16bitoffsetreg(PMSC_ID, PMSC_CTRL1_OFFSET+1) ;
        rega &= 0xFDFF ; // Clear LDERUN bit
        dwt_write16bitoffsetreg(PMSC_ID, PMSC_CTRL1_OFFSET+1, rega) ;
    }

    _dwt_enableclocks(ENABLE_ALL_SEQ); // Enable clocks for sequencing

    // The 3 bits in AON CFG1 register must be cleared to ensure proper operation of the DW1000 in DEEPSLEEP mode.
    dwt_write8bitoffsetreg(AON_ID, AON_CFG1_OFFSET, 0x00);

    // Read system register / store local copy
    pdw1000local->sysCFGreg = dwt_read32bitreg(SYS_CFG_ID) ; // Read sysconfig register

    return DWT_SUCCESS ;

  } // end dwt_initialise()

  /* Function name: reset_DW1000
   * Input: None/void
   * Output: None
   * Returns: None/void
   * Purpose: This function resets the digital part of DW1000
   *          Note: DW_RESET pin on DW1000 has 2 functions
   *                In general it is output, but it also can be used to reset the digital
   *                part of DW1000 by driving this pin low.
   *                Note, the DW_RESET pin should not be driven high externally.
   */
  void UWBComponentImpl ::
      reset_DW1000()
  {
    /* Check and change direction value later */
    // gioSetDirection(spiPORT5,0xFFFFFFFF);
    gioSetBit(hetPORT1,RST_bit,0U);
    __delay_cycles(440000); /* Delay of 2ms??? */
    hetPORT1->PULDIS = (uint32)1U << RST_bit;
    __delay_cycles(440000); /* Delay of 2ms??? */
    //nrf_gpio_cfg_output(DW1000_RST);
    //nrf_gpio_pin_clear(DW1000_RST);
    //nrf_delay_ms(2);
    //nrf_gpio_pin_set(DW1000_RST);
    //nrf_delay_ms(50);
    //nrf_gpio_cfg_input(DW1000_RST, NRF_GPIO_PIN_NOPULL);
    //nrf_delay_ms(2);
  }

  /* Function name: _dwt_configlde
   * Input: An int (PRF index (0 or 1) - 0 corresponds to 16 and 1 to 64 PRF)
   * Output: None
   * Returns: None/Void
   * Purpose: This function configures LDE algorithm parameters
   */
  void UWBComponentImpl ::
      _dwt_configlde(
          int prfIndex
      )
  {
      dwt_write8bitoffsetreg(LDE_IF_ID, LDE_CFG1_OFFSET, LDE_PARAM1); // 8-bit configuration register

      if(prfIndex)
      {
          dwt_write16bitoffsetreg( LDE_IF_ID, LDE_CFG2_OFFSET, (uint16) LDE_PARAM3_64); // 16-bit LDE configuration tuning register
      }
      else
      {
          dwt_write16bitoffsetreg( LDE_IF_ID, LDE_CFG2_OFFSET, (uint16) LDE_PARAM3_16);
      }
  }

  /* Function name: dwt_configure
   * Input: A dwt_config_t pointer (a pointer to the configuration structure, which contains the device configuration data)
   * Output: None
   * Returns: None/Void
   * Purpose: This function provides the main API for the configuration of the
   * DW1000 and this low-level driver.  The input is a pointer to the data structure
   * of type dwt_config_t that holds all the configurable items.
   * The dwt_config_t structure shows which ones are supported
   */
  void UWBComponentImpl ::
     dwt_configure(
         dwt_config_t *config
     )
  {
    uint8 nsSfd_result  = 0;
    uint8 useDWnsSFD = 0;
    uint8 chan = config->chan ;
    uint32 regval ;
    uint16 reg16 = lde_replicaCoeff[config->rxCode];
    uint8 prfIndex = config->prf - DWT_PRF_16M;
    uint8 bw = ((chan == 4) || (chan == 7)) ? 1 : 0 ; // Select wide or narrow band

    // For 110 kbps we need a special setup
    if(DWT_BR_110K == config->dataRate)
    {
        pdw1000local->sysCFGreg |= SYS_CFG_RXM110K ;
        reg16 >>= 3; // lde_replicaCoeff must be divided by 8
    }
    else
    {
        pdw1000local->sysCFGreg &= (~SYS_CFG_RXM110K) ;
    }

    pdw1000local->longFrames = config->phrMode ;

    pdw1000local->sysCFGreg &= ~SYS_CFG_PHR_MODE_11;
    pdw1000local->sysCFGreg |= (SYS_CFG_PHR_MODE_11 & (config->phrMode << SYS_CFG_PHR_MODE_SHFT));

    dwt_write32bitreg(SYS_CFG_ID,pdw1000local->sysCFGreg) ;
    // Set the lde_replicaCoeff
    dwt_write16bitoffsetreg(LDE_IF_ID, LDE_REPC_OFFSET, reg16) ;

    _dwt_configlde(prfIndex);

    // Configure PLL2/RF PLL block CFG/TUNE (for a given channel)
    dwt_write32bitoffsetreg(FS_CTRL_ID, FS_PLLCFG_OFFSET, fs_pll_cfg[chan_idx[chan]]);
    dwt_write8bitoffsetreg(FS_CTRL_ID, FS_PLLTUNE_OFFSET, fs_pll_tune[chan_idx[chan]]);

    // Configure RF RX blocks (for specified channel/bandwidth)
    dwt_write8bitoffsetreg(RF_CONF_ID, RF_RXCTRLH_OFFSET, rx_config[bw]);

    // Configure RF TX blocks (for specified channel and PRF)
    // Configure RF TX control
    dwt_write32bitoffsetreg(RF_CONF_ID, RF_TXCTRL_OFFSET, tx_config[chan_idx[chan]]);

    // Configure the baseband parameters (for specified PRF, bit rate, PAC, and SFD settings)
    // DTUNE0
    dwt_write16bitoffsetreg(DRX_CONF_ID, DRX_TUNE0b_OFFSET, sftsh[config->dataRate][config->nsSFD]);

    // DTUNE1
    dwt_write16bitoffsetreg(DRX_CONF_ID, DRX_TUNE1a_OFFSET, dtune1[prfIndex]);

    if(config->dataRate == DWT_BR_110K)
    {
        dwt_write16bitoffsetreg(DRX_CONF_ID, DRX_TUNE1b_OFFSET, DRX_TUNE1b_110K);
    }
    else
    {
        if(config->txPreambLength == DWT_PLEN_64)
        {
            dwt_write16bitoffsetreg(DRX_CONF_ID, DRX_TUNE1b_OFFSET, DRX_TUNE1b_6M8_PRE64);
            dwt_write8bitoffsetreg(DRX_CONF_ID, DRX_TUNE4H_OFFSET, DRX_TUNE4H_PRE64);
        }
        else
        {
            dwt_write16bitoffsetreg(DRX_CONF_ID, DRX_TUNE1b_OFFSET, DRX_TUNE1b_850K_6M8);
            dwt_write8bitoffsetreg(DRX_CONF_ID, DRX_TUNE4H_OFFSET, DRX_TUNE4H_PRE128PLUS);
        }
    }

    // DTUNE2
    dwt_write32bitoffsetreg(DRX_CONF_ID, DRX_TUNE2_OFFSET, digital_bb_config[prfIndex][config->rxPAC]);

    // DTUNE3 (SFD timeout)
    // Don't allow 0 - SFD timeout will always be enabled
    if(config->sfdTO == 0)
    {
        config->sfdTO = DWT_SFDTOC_DEF;
    }
    dwt_write16bitoffsetreg(DRX_CONF_ID, DRX_SFDTOC_OFFSET, config->sfdTO);

    // Configure AGC parameters
    dwt_write32bitoffsetreg( AGC_CFG_STS_ID, 0xC, agc_config.lo32);
    dwt_write16bitoffsetreg( AGC_CFG_STS_ID, 0x4, agc_config.target[prfIndex]);

    // Set (non-standard) user SFD for improved performance,
    if(config->nsSFD)
    {
        // Write non standard (DW) SFD length
        dwt_write8bitoffsetreg(USR_SFD_ID, 0x00, dwnsSFDlen[config->dataRate]);
        nsSfd_result = 3 ;
        useDWnsSFD = 1 ;
    }
    regval =  (CHAN_CTRL_TX_CHAN_MASK & (chan << CHAN_CTRL_TX_CHAN_SHIFT)) | // Transmit Channel
              (CHAN_CTRL_RX_CHAN_MASK & (chan << CHAN_CTRL_RX_CHAN_SHIFT)) | // Receive Channel
              (CHAN_CTRL_RXFPRF_MASK & (config->prf << CHAN_CTRL_RXFPRF_SHIFT)) | // RX PRF
              ((CHAN_CTRL_TNSSFD|CHAN_CTRL_RNSSFD) & (nsSfd_result << CHAN_CTRL_TNSSFD_SHIFT)) | // nsSFD enable RX&TX
              (CHAN_CTRL_DWSFD & (useDWnsSFD << CHAN_CTRL_DWSFD_SHIFT)) | // Use DW nsSFD
              (CHAN_CTRL_TX_PCOD_MASK & (config->txCode << CHAN_CTRL_TX_PCOD_SHIFT)) | // TX Preamble Code
              (CHAN_CTRL_RX_PCOD_MASK & (config->rxCode << CHAN_CTRL_RX_PCOD_SHIFT)) ; // RX Preamble Code

    dwt_write32bitreg(CHAN_CTRL_ID,regval) ;

    // Set up TX Preamble Size, PRF and Data Rate
    pdw1000local->txFCTRL = ((config->txPreambLength | config->prf) << TX_FCTRL_TXPRF_SHFT) | (config->dataRate << TX_FCTRL_TXBR_SHFT);
    dwt_write32bitreg(TX_FCTRL_ID, pdw1000local->txFCTRL);

    // The SFD transmit pattern is initialised by the DW1000 upon a user TX request, but (due to an IC issue) it is not done for an auto-ACK TX. The
    // SYS_CTRL write below works around this issue, by simultaneously initiating and aborting a transmission, which correctly initialises the SFD
    // after its configuration or reconfiguration.
    // This issue is not documented at the time of writing this code. It should be in next release of DW1000 User Manual (v2.09, from July 2016).
    dwt_write8bitoffsetreg(SYS_CTRL_ID, SYS_CTRL_OFFSET, SYS_CTRL_TXSTRT | SYS_CTRL_TRXOFF); // Request TX start and TRX off at the same time
  } // end dwt_configure()

  /* Function name: dwt_setrxantennadelay
   * Input: A uint16 (rxDelay - this is the total (RX) antenna delay value, which
   *                             will be programmed into the RX register)
   * Output: None
   * Returns: None/void
   * Purpose: This function writes the antenna delay (in time units) to RX registers
   */
  void UWBComponentImpl ::
      dwt_setrxantennadelay(
          uint16 rxDelay
      )
  {
      // Set the RX antenna delay for auto TX timestamp adjustment
      dwt_write16bitoffsetreg(LDE_IF_ID, LDE_RXANTD_OFFSET, rxDelay);
  }

  /* Function name: dwt_settxantennadelay
   * Input: A uint16 (rxDelay - this is the total (TX) antenna delay value, which
   *                             will be programmed into the TX register)
   * Output: None
   * Returns: None/void
   * Purpose: This function writes the antenna delay (in time units) to TX registers
   */
  void UWBComponentImpl ::
      dwt_settxantennadelay(
          uint16 txDelay
      )
  {
      // Set the TX antenna delay for auto TX timestamp adjustment
      dwt_write16bitoffsetreg(TX_ANTD_ID, TX_ANTD_OFFSET, txDelay);
  }

  /*  
   *    Function Name: txrx_configure
   *    Input: None/void
   *    Output: None/void
   *    Returns: An integer (0 if initialization of DW1000 is successful, 1 if initialization of DW1000 is unsuccessful)
   *    Purpose: This function configures the DW1000
   */
  int UWBComponentImpl ::
    txrx_configure()
  {

    /* Implement LED Stuff */

    /* Setup DW1000 IRQ pin */
    // nrf_gpio_cfg_input(DW1000_IRQ, NRF_GPIO_PIN_NOPULL);      //irq

    /* Reset DW1000 */
    reset_DW1000();

    uint16 checkTxOK = dwt_read16bitoffsetreg(SYS_STATUS_ID, 3); // Read at offset 3 to get the upper 2 bytes out of 5
    printf("check value %x\n", checkTxOK);

    /* Set SPI clock to 2MHz */
    //port_set_dw1000_slowrate();

    /* Init the DW1000 */
    if (dwt_initialise(DWT_LOADUCODE) == DWT_ERROR)
    {
      //Init of DW1000 Failed
      return -1;
    }

    // Set SPI to 8MHz clock
    //port_set_dw1000_fastrate();

    /* Configure DW1000. */
    dwt_configure(&config);

    /* Set preamble timeout for expected frames. See NOTE 3 below. */
    //dwt_setpreambledetecttimeout(0); // PRE_TIMEOUT

    //-------------dw1000  ini------end---------------------------
    // IF WE GET HERE THEN THE LEDS WILL BLINK

    /* Apply default antenna delay value. See NOTE 2 below. */
    dwt_setrxantennadelay(RX_ANT_DLY);
    dwt_settxantennadelay(TX_ANT_DLY);


    return 0;
  }

  /* Function name: dwt_readsystime
   * Input: An uint8 pointer (5 byte buffer that will store read system time)
   * Output: None
   * Returns: None/void
   * Purpose: This function reads the system time and writes it to the buffer provided.
   */
  void UWBComponentImpl ::
    dwt_readsystime(
        uint8 * timestamp
    )
  {
    dwt_readfromdevice(SYS_TIME_ID, SYS_TIME_OFFSET, SYS_TIME_LEN, timestamp) ;
  }

  /* Function name: get_sys_timestamp_u64
   * Input: nothing/void
   * Output: None
   * Returns: uint64 containing the value of the sys time-stamp
   * Purpose: This function gets the sys time-stamp in a 64-bit variable, assuming the length of time-stamps
   *          is 40 bits.
   */
  uint64 UWBComponentImpl ::
    get_sys_timestamp_u64()
  {
    uint8 ts_tab[5];
    uint64 ts = 0;
    int i;
    dwt_readsystime(ts_tab);
    for (i = 4; i >= 0; i--) {
        ts <<= 8;
        ts |= ts_tab[i];
    }
    return ts;
  }

  /* Function name: dwt_setdelayedtrxtime
   * Input: A uint32 (TX/RX start time - the 32 bits should be the high 32 bits of the system time
   *                  at which to send the message, or at which to turn on the receiver)
   * Output: None
   * Returns: None/void
   * Purpose: This function configures the delayed transmit time or the delayed RX on time
   */
  void UWBComponentImpl ::
    dwt_setdelayedtrxtime(
        uint32 starttime
    )
  {
      dwt_write32bitoffsetreg(DX_TIME_ID, 1, starttime); // Write at offset 1 as the lower 9 bits of this register are ignored

  } // end dwt_setdelayedtrxtime()

  /* Function name: resp_msg_set_ts
   * Input: A uint8 pointer (first byte in the timestamp field) and a uint32 pointer (the timestamp value)
   * Output: None
   * Returns: None
   * Purpose: This function fills in a given timestamp field in the response message. In the timestamp fields of the response message,
   *           the least significant byte is at the lower address.
   */
  void UWBComponentImpl ::
    resp_msg_set_ts(
        uint8 *ts_field,
        const uint64 ts
    )
  {
    int i;
    for (i = 0; i < RESP_MSG_TS_LEN; i++)
    {
      ts_field[i] = (ts >> (i * 8)) & 0xFF;
    }
  }

  /* Function name: dwt_writetxdata
   * Input: A uint16 (total frame length, including the two byte CRC)
   *        Note: this is the length of TX message (including the 2 byte CRC) - max is 1023
   *                         standard PHR mode allows up to 127 bytes,
   *        a uint8 pointer (pointer to buffer containing data to send)
   *        and a uint16 (offset in the TX Buffer at which to start writing data)
   * Output: None
   * Returns: An int (DWT_SUCCESS for success, or DWT_ERROR for error)
   * Purpose: This function  writes the supplied TX data into the DW1000's
   *          TX buffer. The input parameters are the data length in bytes and a pointer
   */
  int UWBComponentImpl ::
    dwt_writetxdata(
        uint16 txFrameLength,
        uint8 *txFrameBytes,
        uint16 txBufferOffset
    )
  {
    if ((txBufferOffset + txFrameLength) <= 1024)
    {
        // Write the data to the IC TX buffer, (-2 bytes for auto generated CRC)
        dwt_writetodevice(TX_BUFFER_ID, txBufferOffset, txFrameLength-2, txFrameBytes);
        return DWT_SUCCESS;
    }
    else
    {
        return DWT_ERROR;
    }
  } // end dwt_writetxdata()

  /* Function name: dwt_writetxfctrl
   * Input: A uint16 (length of TX message - max is 1023)
   *        NOTE: standard PHR mode allows up to 127 bytes
   *              if > 127 is programmed, DWT_PHRMODE_EXT needs to be set in the phrMode configuration
   *              see dwt_configure function,
   *         a uint16 (offset in tx buffer to start writing data) and an int (ranging - 1 if this is a ranging frame, else 0)
   * Output: None
   * Returns: None/void
   * Purpose: This function configures the TX frame control register before the transmission of a frame
   */

  void UWBComponentImpl ::
    dwt_writetxfctrl(
        uint16 txFrameLength,
        uint16 txBufferOffset,
        int ranging
    )
  {
    // Write the frame length to the TX frame control register
    // pdw1000local->txFCTRL has kept configured bit rate information
    uint32 reg32 = pdw1000local->txFCTRL | txFrameLength | (txBufferOffset << TX_FCTRL_TXBOFFS_SHFT) | (ranging << TX_FCTRL_TR_SHFT);
    dwt_write32bitreg(TX_FCTRL_ID, reg32);
  } // end dwt_writetxfctrl()

  /* Function name: dwt_starttx
   * Input: An int (mode)
   *        Note -  if 0 immediate TX (no response expected)
   *                if 1 delayed TX (no response expected)
   *                if 2 immediate TX (response expected - so the receiver will be automatically turned on after TX is done)
   *                if 3 delayed TX (response expected - so the receiver will be automatically turned on after TX is done)
   * Output: None
   * Returns: An int (DWT_SUCCESS for success, or DWT_ERROR for error (e.g. a delayed transmission will fail if the delayed time has passed))
   * Purpose: This function initiates the transmission, input parameter indicates which TX mode is used
   */
  int UWBComponentImpl ::
    dwt_starttx(
        uint8 mode
    )
  {
    int retval = DWT_SUCCESS ;
    uint8 temp  = 0x00;
    uint16 checkTxOK = 0 ;

    if(mode & DWT_RESPONSE_EXPECTED)
    {
        temp = (uint8)SYS_CTRL_WAIT4RESP ; // Set wait4response bit
        dwt_write8bitoffsetreg(SYS_CTRL_ID, SYS_CTRL_OFFSET, temp);
        pdw1000local->wait4resp = 1;
    }

    if (mode & DWT_START_TX_DELAYED)
    {
        // Both SYS_CTRL_TXSTRT and SYS_CTRL_TXDLYS to correctly enable TX
        temp |= (uint8)(SYS_CTRL_TXDLYS | SYS_CTRL_TXSTRT) ;
        dwt_write8bitoffsetreg(SYS_CTRL_ID, SYS_CTRL_OFFSET, temp);
        checkTxOK = dwt_read16bitoffsetreg(SYS_STATUS_ID, 3); // Read at offset 3 to get the upper 2 bytes out of 5
        printf("check value %x\n", checkTxOK);
        printf("check mask %x\n", SYS_STATUS_TXERR);
        if ((checkTxOK & SYS_STATUS_TXERR) == 0) // Transmit Delayed Send set over Half a Period away or Power Up error (there is enough time to send but not to power up individual blocks).
        {
            retval = DWT_SUCCESS ; // All okay
        }
        else
        {
            // I am taking DSHP set to Indicate that the TXDLYS was set too late for the specified DX_TIME.
            // Remedial Action - (a) cancel delayed send
            temp = (uint8)SYS_CTRL_TRXOFF; // This assumes the bit is in the lowest byte
            dwt_write8bitoffsetreg(SYS_CTRL_ID, SYS_CTRL_OFFSET, temp);
            // Note event Delayed TX Time too Late
            // Could fall through to start a normal send (below) just sending late.....
            // ... instead return and assume return value of 1 will be used to detect and recover from the issue.
            pdw1000local->wait4resp = 0;
            retval = DWT_ERROR ; // Failed !
        }
    }
    else
    {
        temp |= (uint8)SYS_CTRL_TXSTRT ;
        dwt_write8bitoffsetreg(SYS_CTRL_ID, SYS_CTRL_OFFSET, temp);
    }

    return retval;

  } // end dwt_starttx()

  /* Function name: dwt_rxreset
   * Input: None/void
   * Output: None
   * Returns: None/void
   * Purpose: This function resets the receiver of the DW1000
   */
  void UWBComponentImpl ::
    dwt_rxreset()
  {
      // Set RX reset
      dwt_write8bitoffsetreg(PMSC_ID, PMSC_CTRL0_SOFTRESET_OFFSET, PMSC_CTRL0_RESET_RX);

      // Clear RX reset
      dwt_write8bitoffsetreg(PMSC_ID, PMSC_CTRL0_SOFTRESET_OFFSET, PMSC_CTRL0_RESET_CLEAR);
  }

  /* Function name: make_and_send_response
   * Input: A uint32 pointer (the send value), a uint32 pointer (the time stamp value), a uint8 pointer (the messaging function),
   *        a uint8* pointer (the message format) and a size_t (message format size)
   * Output: None
   * Returns: An integer (0 if message could be sent, -1 if message could not be sent)
   * Purpose: This function modifies the send value and creates and sends the message
   */
  int UWBComponentImpl ::
    make_and_send_response(
        uint64 rx_ts_u64,
        uint32 *tx_ts,
        uint8 *mess_function,
        uint8 *msg_arr,
        size_t arr_size
    )
  {
    uint32 resp_tx_time;
    uint64 resp_tx_ts;
    int ret;

    /* Compute final message transmission time. See NOTE 7 below. */
    resp_tx_time = ((rx_ts_u64 + (POLL_RX_TO_RESP_TX_DLY_UUS * UUS_TO_DWT_TIME)) >> 8);
    // the function called in the main loop is set_rxdelay
    dwt_setdelayedtrxtime(resp_tx_time);

    /* Response TX timestamp is the transmission time we programmed plus the antenna delay. */
    resp_tx_ts = (((uint64)(resp_tx_time & 0xFFFFFFFEUL)) << 8) + TX_ANT_DLY;
    *tx_ts = (uint32) resp_tx_ts;

    /* Write all timestamps in the final message. See NOTE 8 below. */
    resp_msg_set_ts(&msg_arr[RESP_MSG_POLL_RX_TS_IDX], rx_ts_u64);
    resp_msg_set_ts(&msg_arr[RESP_MSG_RESP_TX_TS_IDX], resp_tx_ts);

    /* Write and send the response message. See NOTE 9 below. */
    msg_arr[ALL_MSG_SN_IDX] = frame_seq_nb;
    msg_arr[ALL_MSG_FUNCTIONCODE_IDX] = *mess_function;
    dwt_writetxdata(arr_size, msg_arr, 0); /* Zero offset in TX buffer. See Note 5 below.*/
    dwt_writetxfctrl(arr_size, 0, 1); /* Zero offset in TX buffer, ranging. */

    ret = dwt_starttx(DWT_START_TX_DELAYED);
    // TODO find return type of dwt_starttx and consider returning ret;
    // TODO sometimes you need to expect a resposne, so you need to enable it here
    // or at some other point by calling the right function

    //ret = dwt_starttx(DWT_START_TX_IMMEDIATE);

    /* If dwt_starttx() returns an error, abandon this ranging exchange and proceed to the next one. */
    if (ret == DWT_SUCCESS) {
        /* Poll DW1000 until TX frame sent event set. See NOTE 5 below. */
        while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS))
        {};

        /* Clear TXFRS event. */
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);

        /* Increment frame sequence number after transmission of the poll message (modulo 256). */
        frame_seq_nb++;
    }
    else {
        /* Reset RX to properly reinitialise LDE operation. */
        printf("need to reset\r\n");
        dwt_rxreset();
        return -1;
    }
    return 0;
  }

  /* Function name: dwt_syncrxbufptrs
   * Input: None/void
   * Output: None
   * Returns: None/void
   * Purpose: This function synchronizes RX buffer pointers. Need to make sure that the host/IC
   *          buffer pointers are aligned before starting RX
   */
  void UWBComponentImpl ::
    dwt_syncrxbufptrs()
  {
    uint8  buff ;
    // Need to make sure that the host/IC buffer pointers are aligned before starting RX
    buff = dwt_read8bitoffsetreg(SYS_STATUS_ID, 3); // Read 1 byte at offset 3 to get the 4th byte out of 5

    if((buff & (SYS_STATUS_ICRBP >> 24)) !=     // IC side Receive Buffer Pointer
       ((buff & (SYS_STATUS_HSRBP>>24)) << 1) ) // Host Side Receive Buffer Pointer
    {
        dwt_write8bitoffsetreg(SYS_CTRL_ID, SYS_CTRL_HRBT_OFFSET , 0x01) ; // We need to swap RX buffer status reg (write one to toggle internally)
    }
  }

  /* Function name: decamutexon
   * Input: None/void
   * Output: None
   * Returns: An decaIrqStatus_t (returns 0)
   * Purpose: This function currently does nothing
   */
  decaIrqStatus_t UWBComponentImpl ::
    decamutexon()
  {
  //  u16 j = (u16)(NVIC->ISER[0] & (1 << 5));

      //  if(j)
  //  {
  //      NVIC->ISER[0] &= ~(1 << 5); //disable the external interrupt line
  //  }
  //  return j ;

    return 0;
  }

  /* Function name: decamutexon
   * Input: A decaIrqStatus_t (s)
   * Output: None
   * Returns: None/void
   * Purpose: This function currently does nothing
   */
  void UWBComponentImpl ::
    decamutexoff(
        decaIrqStatus_t s
    )
  {
  //  if(j)

      //  {
  //      NVIC->ISER[0] |= (1 << 5);;
  //  }
      ;
  }

  /* Function name: dwt_syncrxbufptrs
   * Input: None/void
   * Output: None
   * Returns: None/void
   * Purpose: This function turns off the transceiver
   */
  void UWBComponentImpl ::
    dwt_forcetrxoff()
  {
      decaIrqStatus_t stat ;
      uint32 mask;

      mask = dwt_read32bitreg(SYS_MASK_ID) ; // Read set interrupt mask

      // Need to beware of interrupts occurring in the middle of following read modify write cycle
      // We can disable the radio, but before the status is cleared an interrupt can be set (e.g. the
      // event has just happened before the radio was disabled)
      // thus we need to disable interrupt during this operation
      stat = decamutexon() ;

      dwt_write32bitreg(SYS_MASK_ID, 0) ; // Clear interrupt mask - so we don't get any unwanted events

      dwt_write8bitoffsetreg(SYS_CTRL_ID, SYS_CTRL_OFFSET, (uint8)SYS_CTRL_TRXOFF) ; // Disable the radio

      // Forcing Transceiver off - so we do not want to see any new events that may have happened
      dwt_write32bitreg(SYS_STATUS_ID, (SYS_STATUS_ALL_TX | SYS_STATUS_ALL_RX_ERR | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_GOOD));

      dwt_syncrxbufptrs();

      dwt_write32bitreg(SYS_MASK_ID, mask) ; // Set interrupt mask to what it was

      // Enable/restore interrupts again...
      decamutexoff(stat) ;
      pdw1000local->wait4resp = 0;

  } // end deviceforcetrxoff()

  /* Function name: dwt_rxenable
   * Input: An int (mode which can be one of the following allowed values)
   *                    DWT_START_RX_IMMEDIATE      0 used to enbale receiver immediately
   *                    DWT_START_RX_DELAYED        1 used to set up delayed RX, if "late" error triggers, then the RX will be enabled immediately
   *                    (DWT_START_RX_DELAYED | DWT_IDLE_ON_DLY_ERR) 3 used to disable re-enabling of receiver if delayed RX failed due to "late" error
   *                    (DWT_START_RX_IMMEDIATE | DWT_NO_SYNC_PTRS) 4 used to re-enable RX without trying to sync IC and host side buffer pointers, typically when
   *                                                                performing manual RX re-enabling in double buffering mode
   * Output: None
   * Returns: An int (DWT_SUCCESS for success, or DWT_ERROR for error (e.g. a delayed receive enable will be too far in the future if delayed time has passed))
   * Purpose: This function turns on the receiver, which can be immediate or delayed (depending on the mode parameter). In the case of a
   *          "late" error the receiver will only be turned on if the DWT_IDLE_ON_DLY_ERR is not set.
   *          The receiver will stay turned on, listening to any messages until
   *          it either receives a good frame, an error (CRC, PHY header, Reed Solomon) or  it times out (SFD, Preamble or Frame).
   */
  int UWBComponentImpl ::
    dwt_rxenable(
        int mode
    )
  {
    uint16 temp ;
    uint8 temp1 ;

    if ((mode & DWT_NO_SYNC_PTRS) == 0)
    {
        dwt_syncrxbufptrs();
    }

    temp = (uint16)SYS_CTRL_RXENAB ;

    if (mode & DWT_START_RX_DELAYED)
    {
        temp |= (uint16)SYS_CTRL_RXDLYE ;
    }

    dwt_write16bitoffsetreg(SYS_CTRL_ID, SYS_CTRL_OFFSET, temp);

    if (mode & DWT_START_RX_DELAYED) // check for errors
    {
        temp1 = dwt_read8bitoffsetreg(SYS_STATUS_ID, 3); // Read 1 byte at offset 3 to get the 4th byte out of 5
        if ((temp1 & (SYS_STATUS_HPDWARN >> 24)) != 0) // if delay has passed do immediate RX on unless DWT_IDLE_ON_DLY_ERR is true
        {
            dwt_forcetrxoff(); // turn the delayed receive off

            if((mode & DWT_IDLE_ON_DLY_ERR) == 0) // if DWT_IDLE_ON_DLY_ERR not set then re-enable receiver
            {
                dwt_write16bitoffsetreg(SYS_CTRL_ID, SYS_CTRL_OFFSET, SYS_CTRL_RXENAB);
            }
            return DWT_ERROR; // return warning indication
        }
    }

    return DWT_SUCCESS;
  } // end dwt_rxenable()

  /* Function name: dwt_readrxdata
   * Input: An uint8 pointer (buffer into which data is read), a uint16 (length of data to read in bytes) and
   *        an uint16 (offset in the rx buffer from which to read data)
   * Output: None
   * Returns: None/void
   * Purpose: This function reads data from the RX buffer, from an offset location give by offset parameter
   */
  void UWBComponentImpl ::
    dwt_readrxdata(
        uint8 *buffer,
        uint16 length,
        uint16 rxBufferOffset
    )
  {
      dwt_readfromdevice(RX_BUFFER_ID,rxBufferOffset,length,buffer) ;
  }

  /* Function name: resp_msg_get_ts
   * Input: A uint8 pointer (first byte in the timestamp field) and a uint32 pointer (the timestamp value)
   * Output: None
   * Returns: None
   * Purpose: This function reads a given timestamp value from the response message. In the timestamp fields of the response message,
   *           the least significant byte is at the lower address.
   */
  void UWBComponentImpl ::
    resp_msg_get_ts(
        uint8 *ts_field,
        uint32 *ts
    )
  {
    int i;
    *ts = 0;
    for (i = 0; i < RESP_MSG_TS_LEN; i++) {
        *ts += ts_field[i] << (i * 8);
    }
  }

  /* Function name: recv_and_parse
   * Input: A uint32 pointer (prev recv value), a unint32 pointer (curr sent value) and a uint8 pointer (the messaging function)
   * Output: None besides error messages
   * Returns: An integer (0 if message could be parsed, -1 if message could not be parsed)
   * Purpose: This function receives the message sent and checks the message
   */
  int UWBComponentImpl ::
    recv_and_parse(
        uint32 *their_rx_ts,
        uint32 *their_tx_ts,
        uint8 *mess_function
    )
  {
      /* Activate reception immediately. */
      dwt_rxenable(DWT_START_RX_IMMEDIATE);

      /* Poll for reception of a frame or error/timeout. See NOTE 5 below. */
      while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR)))
      {};

      #if 0     // Include to determine the type of timeout if required.
      int temp = 0;
      // (frame wait timeout and preamble detect timeout)
      if(status_reg & SYS_STATUS_RXRFTO )
      temp =1;
      else if(status_reg & SYS_STATUS_RXPTO )
      temp =2;
      #endif

      if (status_reg & SYS_STATUS_RXFCG) {
          uint32 frame_len;

          /* Clear good RX frame event in the DW1000 status register. */
          dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);

          /* A frame has been received, read it into the local buffer. */
          frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023;
          if (frame_len <= RX_BUFFER_LEN) {
              dwt_readrxdata(rx_buffer, frame_len, 0);
          }

          /* Check that the frame is a poll sent by "SS TWR initiator" example.
           * As the sequence number field of the frame is not relevant, it is cleared to simplify the validation of the frame.
           */
          rx_buffer[ALL_MSG_SN_IDX] = 0;
          expected_msg[ALL_MSG_FUNCTIONCODE_IDX] = *mess_function;
          if (memcmp(rx_buffer, expected_msg, ALL_MSG_COMMON_LEN) == 0) {
              /* Get timestamps embedded in response message. */
              resp_msg_get_ts(&rx_buffer[RESP_MSG_POLL_RX_TS_IDX], their_rx_ts);
              resp_msg_get_ts(&rx_buffer[RESP_MSG_RESP_TX_TS_IDX], their_tx_ts);
          }
          else {
              printf("memcmp failed\n");
              return -1;
          }
          return 0;
      }
      else {
          printf("Status Reg: %x \r\n", status_reg);
          printf("Reg Checker: %x \r\n", SYS_STATUS_RXFCG);
          printf("registererror\r\n");
          /* Clear RX error events in the DW1000 status register. */
          dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);

          /* Reset RX to properly reinitialise LDE operation. */
          dwt_rxreset();
          return -1;
      }
      return 0;
  }

  /* Function name: dwt_readrxtimestamp
   * Input: A uint8 pointer (buffer to store the read RX timestamp time)
   * Output: None
   * Returns: None/void
   * Purpose: This function reads the RX timestamp (adjusted time of arrival) and stores it to the buffer provided
   */
  void UWBComponentImpl ::
    dwt_readrxtimestamp(
        uint8 * timestamp
    )
  {
      dwt_readfromdevice(RX_TIME_ID, RX_TIME_RX_STAMP_OFFSET, RX_TIME_RX_STAMP_LEN, timestamp) ; // Get the adjusted time of arrival
  }

  /* Function name: get_rx_timestamp_u64
   * Input: nothing/void
   * Output: None
   * Returns: uint64 containing the value of the read time-stamp
   * Purpose: This function gets the RX time-stamp in a 64-bit variable, assuming the length of time-stamps
   *          is 40 bits, for both TX and RX.
   */
  uint64 UWBComponentImpl ::
    get_rx_timestamp_u64()
  {
    uint8 ts_tab[5];
    uint64 ts = 0;
    int i;
    dwt_readrxtimestamp(ts_tab);
    for (i = 4; i >= 0; i--) {
      ts <<= 8;
      ts |= ts_tab[i];
    }
    return ts;
  }

  /* Function name: ns_start_chain
   * Input: A uint32 pointer (the send value), a uint8 pointer (the messaging function),
   *        a uint8* pointer (the message format) and a size_t (message format size)
   * Output: None besides error messages
   * Returns: An integer (0 if message could be sent, -1 if message could not be sent)
   * Purpose: This function modifies the send value passed in and creates a message to send to the responder
   */
  int UWBComponentImpl ::
    ns_start_chain(
        uint32 *tx_ts,
        uint8 *mess_function,
        uint8 *msg_arr,
        size_t arr_size
    )
  {
    uint64 rx_ts_u64 = get_sys_timestamp_u64();

    if (make_and_send_response(rx_ts_u64, tx_ts, mess_function, msg_arr, arr_size) < 0) {
        printf("make and send response failed start\r\n");
        return -1;
    }
    return 0;
  }

  /* Function name: ns_continue chain
   * Input: A uint32 pointer (prev recv value), a unint32 pointer (curr sent value), a uint32 pointer (curr recv value),
   *        a uint32 pointer (next sent value), a uint8 pointer (the messaging function), an uint8* pointer (the message format)
   *        and a size_t (message format size)
   * Output: None besides error messages
   * Returns: An integer (0 if messages could be received and sent, -1 if messages could not be received or sent)
   * Purpose: This function modifies the send and recv values passed in, parses through the previous message, and
   *          creates the next message to be sent
   */
  int UWBComponentImpl ::
    ns_continue_chain(
        uint32 *their_rx_ts,
        uint32 *their_tx_ts,
        uint32 *rx_ts,
        uint32 *tx_ts,
        uint8 *mess_function,
        uint8 *msg_arr,
        size_t arr_size
    )
  {
      if (recv_and_parse(their_rx_ts, their_tx_ts, mess_function) < 0) {
        printf("recv and parse failed continue\r\n");
        return -1;
      }
      /* Retrieve poll reception timestamp. */
      /* this function simply calls dwt_readtxtimestamp() to get the full 40 bits
       * this must be the full 40 bits because we are calculating a future send time
       * based off rx_ts
       */
      uint64 rx_ts_u64 = get_rx_timestamp_u64();
      /* this value can be only 32 bits, because the high order bytes don't matter,
       * because we will only use this value in a difference calculation
       */
      *rx_ts = (uint32) rx_ts_u64;

      if (make_and_send_response(rx_ts_u64, tx_ts, mess_function, msg_arr, arr_size) < 0) {
          printf("make and send failed continue\r\n");
          return -1;
      }
      return 0;
  }

  /* Function name: ns_end_chain
   * Input: A uint32 pointer (prev recv value), a unint32 pointer (curr sent value), a uint32 pointer (curr recv value),
   *        and a uint8 pointer (the messaging function)
   * Output: None besides error messages
   * Returns: An integer (0 if messages could be received, -1 if messages could not be received)
   * Purpose: This function modifies the send and recv values passed in, parses through the previous message, and modifies the
   *          final received message
   */
  int UWBComponentImpl ::
    ns_end_chain(
        uint32 *their_rx_ts,
        uint32 *their_tx_ts,
        uint32 *rx_ts,
        uint8 *mess_function
    )
  {
    if (recv_and_parse(their_rx_ts, their_tx_ts, mess_function) < 0) {
      printf("recv and parse fail end\r\n");
      return -1;
    }

    *rx_ts = (uint32) get_rx_timestamp_u64();
    return 0;
  }

  /* Function name: calc_distance
   * Input: A uint32 (leg1_send_ts), a uint32 (leg1_recv_ts), a uint32 (leg2_send_ts), a uint32 (leg2_recv_ts),
            a uint32 (leg3_send_ts), a uint32 (leg3_recv_ts)
   * Output: tof and distance caculated
   * Returns: An integer (0 if distance and tof could be calculated, -1 if distance and tof could not be calculated
   * Purpose: This function calculates and prints the tof and distance based on the timestamp values for
              the first three legs
   */
  int UWBComponentImpl ::
    calc_distance(
        uint32 leg1_send_ts,
        uint32 leg1_recv_ts,
        uint32 leg2_send_ts,
        uint32 leg2_recv_ts,
        uint32 leg3_send_ts,
        uint32 leg3_recv_ts
    )
  {
    /* See the patent for
     * "Asymmetric double-sided two-way ranging in an ultrawideband
     * communication system" by Michael Mclaughlin and Billy Verso */
    double roundA = (double)(leg2_recv_ts - leg1_send_ts);
    double replyA = (double)(leg2_send_ts - leg1_recv_ts);
    double roundB = (double)(leg3_recv_ts - leg2_send_ts);
    double replyB = (double)(leg3_send_ts - leg2_recv_ts);

    double inter = roundA * roundB - replyA * replyB;
    if (inter <= 0) { return -1; }
    double tof_dtu = (uint64) (inter / (roundA + roundB + replyA + replyB));

    double tof = tof_dtu * DWT_TIME_UNITS;
    printf("tof: %f\r\n", tof);
    double distance = tof * SPEED_OF_LIGHT;
    printf("Distance : %f\r\n", distance);
    return 0;
  }

  /* Function name: write_leg_values
    * Input: A char pointer (char buffer to store timestamp data), a uint32 (sent timestamp)
    *        and a unint32 (recv timestamp)
    * Output: None
    * Returns: None/void
    * Purpose: This function copes two uint32 values into a char buffer.
    */
  void UWBComponentImpl ::
    write_leg_values(
        char *char_buffer,
        uint32 leg_send_ts,
        uint32 leg_recv_ts
    )
  {
    char_buffer[0] = (char)(uint8)(leg_send_ts >> 24);
    char_buffer[1] = (char)(uint8)(leg_send_ts >> 16);
    char_buffer[2] = (char)(uint8)(leg_send_ts >> 8);
    char_buffer[3] = (char)(uint8)(leg_send_ts);

    char_buffer[4] = (char)(uint8)(leg_recv_ts >> 24);
    char_buffer[5] = (char)(uint8)(leg_recv_ts >> 16);
    char_buffer[6] = (char)(uint8)(leg_recv_ts >> 8);
    char_buffer[7] = (char)(uint8)(leg_recv_ts);
  }

  /* Function name: start_ds_twr
   * Input: A char pointer (char buffer to store timestamp data)
   * Output: Leg values (should also add distance output between the values?)
   * Returns: An integer (0 if leg values could be retrieved, -1 if values could not be retrieved)
   * Purpose: This function retrieves and stores the leg values for the initiator into a buffer
   */
  int UWBComponentImpl ::
    start_ds_twr(
        char *char_buffer
    )
  {
    uint32 leg1_send_ts;
    uint32 leg1_recv_ts;

    uint32 leg2_send_ts;
    uint32 leg2_recv_ts;

    uint32 leg3_send_ts;
    uint32 leg3_recv_ts;

    uint32 leg4_send_ts;
    uint32 leg4_recv_ts;

    uint8 mess_function =  0xE0;

    if (ns_start_chain(&leg1_send_ts, &mess_function, outgoing_msg, sizeof(outgoing_msg)) < 0) { return -1; }

    if (ns_continue_chain(&leg1_recv_ts, &leg2_send_ts, &leg2_recv_ts, &leg3_send_ts, &mess_function,
                          outgoing_msg, sizeof(outgoing_msg)) < 0) { return -1; }

    if (ns_end_chain(&leg3_recv_ts, &leg4_send_ts, &leg4_recv_ts, &mess_function) < 0) { return -1; }

    calc_distance(leg1_send_ts, leg1_recv_ts, leg2_send_ts, leg2_recv_ts, leg3_send_ts, leg3_recv_ts);

    write_leg_values(char_buffer, leg1_send_ts, leg1_recv_ts);
    write_leg_values((char_buffer + 8), leg2_send_ts, leg2_recv_ts);
    write_leg_values((char_buffer + 16), leg3_send_ts, leg3_recv_ts);
    write_leg_values((char_buffer + 24), leg4_send_ts, leg4_recv_ts);

    return 0;
  }

  /* Function name: dwt_readaccdata
   * Input: A uint8 pointer (buffer into which data will be read), a uint16 (length of data to read)
   *        and a unint 16 (offset in acc buffer from which to read data)
   * Output: None
   * Returns: None/Void
   * Purpose: This function is used to read the data from the Accumulator buffer, from an offset location give by offset parameter
   *   gioSetBit(spiPORT5, UWB_bit, 0);       NOTE: Because of an internal memory access delay when reading the accumulator the first octet output is a dummy octet
   *                that should be discarded. This is true no matter what sub-index the read begins at.
   */
  void UWBComponentImpl ::
    dwt_readaccdata(
        uint8 *buffer,
        uint16 len,
        uint16 accOffset
    )
  {
    // Force on the ACC clocks if we are sequenced
    _dwt_enableclocks(READ_ACC_ON);

    dwt_readfromdevice(ACC_MEM_ID,accOffset,len,buffer) ;

    _dwt_enableclocks(READ_ACC_OFF); // Revert clocks back
  }

  /* Function name: Get_CIRdata
   * Input: A char pointer (char buffer to store CIR data)
   * Output: None
   * Returns: None/Void
   * Purpose: This function reads CIR data from DW1000 and stores them to a buffer
   */
  void UWBComponentImpl ::
    Get_CIRdata(
        char *char_buffer
    )
  {
    size_t chunk_size = 100;
    size_t count = ACC_MEM_LEN / chunk_size;
    size_t leftover = ACC_MEM_LEN % chunk_size;
    // printf("ACC_MEM_LEN: %u, count: %u, leftover: %u", ACC_MEM_LEN, count, leftover);
    // printf("Buffer Values: \r\n");

    uint8 *buffer;
    buffer = new uint8 [chunk_size];

    // uint8 *buffer = malloc(chunk_size * sizeof(uint8));

    for (int i = 0; i < count; i++){
        // printf("Chunk %d: ", i);
        dwt_readaccdata(buffer, chunk_size, i*chunk_size);
        for (int j = 0; j < chunk_size; j++) {
            // printf("%02x", buffer[j]);
            char_buffer[i * chunk_size + j] = (char)buffer[j];
        }
            // printf("\r\n");
    }

        dwt_readaccdata(buffer, leftover, count*chunk_size);
        for (int k = 0; k < leftover; k++) {
             // printf("%hhu", buffer[k]);
            char_buffer[count * chunk_size + k] = (char)buffer[k];
        }
    delete[] buffer;
  }

  void UWBComponentImpl ::
    Get_Time_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // char times[4*4]; /*4, 4 byte sized U32's for time stamps*/
    char times[8*4]; /*8, 4 byte sized U32's for time stamps*/
    Fw::Buffer fwBuffer(0, 0, reinterpret_cast<U64>(&times), sizeof(times));
    // gioSetBit(spiPORT5, UWB_bit, 0);
    // Transmit function
    // One Byte: uint32 spiTransmitOneByte(spi, dataConfig, uint16* srcbuff)
    // Multibyte: uint32 spiTransmitData(spi, dataConfig, uint32 blocksize, uint16 * srcbuff);

    // Receive function
    // One Byte: uint32 spiReceiveOneByte(spi, dataConfig, uint16 *destbuff)
    // Multibyte: uint32 spiReceiveData(spi, dataConfig, sizeof(times), static_cast<uint16_t*>(&times));

    txrx_configure();

    gioSetBit(spiPORT5, UWB_bit, 1);

    printf("set to zero\n");

    gioSetBit(spiPORT5, UWB_bit, 0);

    __delay_cycles(12000000);

    gioSetBit(spiPORT5, UWB_bit, 1);

    printf("set to one\n");

    start_ds_twr(times);

    // gioSetBit(spiPORT5, UWB_bit, 1);
    uint32_t createTime = static_cast<uint32_t>(getTime().get_time_ms());
    UWBSend_out(0, m_callbackId, createTime, fwBuffer);
    m_bytesSent += static_cast<U32>(sizeof(times));
    tlmWrite_UWBBytesSent(m_bytesSent);
    m_callbackId++;
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void UWBComponentImpl ::
    Get_Data_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // char times[4000+4*4]; /*4, 4 byte sized U32's for time stamps and 4k bytes of data*/
    char times[ACC_MEM_LEN+8*4]; /*8, 4 byte sized U32's for time stamps and 4064 bytes of data*/
    Fw::Buffer fwBuffer(0, 0, reinterpret_cast<U64>(&times), sizeof(times));
    // gioSetBit(spiPORT5, UWB_bit, 0);
    // Transmit function
    // One Byte: uint32 spiTransmitOneByte(spi, dataConfig, uint16* srcbuff)
    // Multibyte: uint32 spiTransmitData(spi, dataConfig, uint32 blocksize, uint16 * srcbuff);

    // Receive function
    // One Byte: uint32 spiReceiveOneByte(spi, dataConfig, uint16 *destbuff)
    // Multibyte: uint32 spiReceiveData(spi, dataConfig, sizeof(times), static_cast<uint16_t*>(&times));

    start_ds_twr(times);
    Get_CIRdata(times + 32);

    // gioSetBit(spiPORT5, UWB_bit, 1);
    uint32_t createTime = static_cast<uint32_t>(getTime().get_time_ms());
    UWBSend_out(0, m_callbackId, createTime, fwBuffer);
    m_bytesSent += static_cast<U32>(sizeof(times));
    tlmWrite_UWBBytesSent(m_bytesSent);
    m_callbackId++;
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

} // end namespace CubeRover
