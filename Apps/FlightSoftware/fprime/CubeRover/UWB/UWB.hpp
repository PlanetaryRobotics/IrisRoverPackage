// ======================================================================
// \title  UWBComponentImpl.hpp
// \author alec
// \brief  hpp file for UWB component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef UWB_HPP
#define UWB_HPP

#include "CubeRover/UWB/UWBComponentAc.hpp"
#include <stdint.h>
#include <hal_stdtypes.h>

namespace CubeRover {

  class UWBComponentImpl :
    public UWBComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object UWB
      //!
      UWBComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object UWB
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object UWB
      //!
      ~UWBComponentImpl(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for PingIn
      //!
      void PingIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Definitions
      // ----------------------------------------------------------------------

    /* Rover Config */

    /* Length of the common part of the message (up to and including the function code, see NOTE 1 below). */
    #define ALL_MSG_COMMON_LEN 10

    /* Indexes to access some of the fields in the frames defined above. */
    #define ALL_MSG_SN_IDX 2
    #define ALL_MSG_DEST_ADDR_IDX 5
    #define ALL_MSG_SRC_ADDR_IDX 7
    #define ALL_MSG_ADDR_LEN 7
    #define RESP_MSG_POLL_RX_TS_IDX 10
    #define RESP_MSG_RESP_TX_TS_IDX 14
    #define RESP_MSG_TS_LEN 4
    #define ALL_MSG_FUNCTIONCODE_IDX 9

    /*Should be accurately calculated during calibration*/
    #define TX_ANT_DLY 16300
    #define RX_ANT_DLY 16456

    // Not enough time to write the data so TX timeout extended for nRF operation.
    // Might be able to get away with 800 uSec but would have to test
    // See note 6 at the end of this file
    #define POLL_RX_TO_RESP_TX_DLY_UUS  2100

    /* UWB microsecond (uus) to device time unit (dtu, around 15.65 ps) conversion factor.
    * 1 uus = 512 / 499.2 s and 1 s = 499.2 * 128 dtu. */
    #define UUS_TO_DWT_TIME 65536

    #define MY_ID 0

    /* End Rover Config */

    // OTP addresses definitions
    #define LDOTUNE_ADDRESS (0x04)
    #define PARTID_ADDRESS (0x06)
    #define LOTID_ADDRESS  (0x07)
    #define VBAT_ADDRESS   (0x08)
    #define VTEMP_ADDRESS  (0x09)
    #define XTRIM_ADDRESS  (0x1E)

    #ifndef DWT_NUM_DW_DEV
    #define DWT_NUM_DW_DEV (1)
    #endif

    #define DWT_SUCCESS (0)
    #define DWT_ERROR   (-1)

    #define DWT_DEVICE_ID   (0xDECA0130)        //!< DW1000 MP device ID

    //DW1000 INIT configuration parameters
    #define DWT_LOADUCODE     0x1
    #define DWT_LOADNONE      0x0

    //! constants for selecting the bit rate for data TX (and RX)
    //! These are defined for write (with just a shift) the TX_FCTRL register
    #define DWT_BR_110K     0   //!< UWB bit rate 110 kbits/s
    #define DWT_BR_850K     1   //!< UWB bit rate 850 kbits/s
    #define DWT_BR_6M8      2   //!< UWB bit rate 6.8 Mbits/s

    //! constants for specifying the (Nominal) mean Pulse Repetition Frequency
    //! These are defined for direct write (with a shift if necessary) to CHAN_CTRL and TX_FCTRL regs
    #define DWT_PRF_16M     1   //!< UWB PRF 16 MHz
    #define DWT_PRF_64M     2   //!< UWB PRF 64 MHz

    //! constants for specifying Preamble Acquisition Chunk (PAC) Size in symbols
    #define DWT_PAC8        0   //!< PAC  8 (recommended for RX of preamble length  128 and below
    #define DWT_PAC16       1   //!< PAC 16 (recommended for RX of preamble length  256
    #define DWT_PAC32       2   //!< PAC 32 (recommended for RX of preamble length  512
    #define DWT_PAC64       3   //!< PAC 64 (recommended for RX of preamble length 1024 and up

    //! constants for specifying TX Preamble length in symbols
    //! These are defined to allow them be directly written into byte 2 of the TX_FCTRL register
    //! (i.e. a four bit value destined for bits 20..18 but shifted left by 2 for byte alignment)
    #define DWT_PLEN_4096   0x0C    //! Standard preamble length 4096 symbols
    #define DWT_PLEN_2048   0x28    //! Non-standard preamble length 2048 symbols
    #define DWT_PLEN_1536   0x18    //! Non-standard preamble length 1536 symbols
    #define DWT_PLEN_1024   0x08    //! Standard preamble length 1024 symbols
    #define DWT_PLEN_512    0x34    //! Non-standard preamble length 512 symbols
    #define DWT_PLEN_256    0x24    //! Non-standard preamble length 256 symbols
    #define DWT_PLEN_128    0x14    //! Non-standard preamble length 128 symbols
    #define DWT_PLEN_64     0x04    //! Standard preamble length 64 symbols

    #define DWT_SFDTOC_DEF              0x1041  // default SFD timeout value

    #define DWT_PHRMODE_STD             0x0     // standard PHR mode
    #define DWT_PHRMODE_EXT             0x3     // DW proprietary extended frames PHR mode

    #define DWT_TIME_UNITS          (1.0/499.2e6/128.0) //!< = 15.65e-12 s

    #define SPEED_OF_LIGHT 299702547

    // Defined constants for "mode" bitmask parameter passed into dwt_starttx() function.
    #define DWT_START_TX_IMMEDIATE      0
    #define DWT_START_TX_DELAYED        1
    #define DWT_RESPONSE_EXPECTED       2

    #define DWT_START_RX_IMMEDIATE  0
    #define DWT_START_RX_DELAYED    1    // Set up delayed RX, if "late" error triggers, then the RX will be enabled immediately
    #define DWT_IDLE_ON_DLY_ERR     2    // If delayed RX failed due to "late" error then if this
                                         // flag is set the RX will not be re-enabled immediately, and device will be in IDLE when function exits
    #define DWT_NO_SYNC_PTRS        4    // Do not try to sync IC side and Host side buffer pointers when enabling RX. This is used to perform manual RX
                                         // re-enabling when receiving a frame in double buffer mode.

    /****************************************************************************//**
     * @brief Bit definitions for register DEV_ID
    **/
    #define DEV_ID_ID               0x00            /* Device ID register, includes revision info (0xDECA0130) */
    #define DEV_ID_LEN              (4)
    /* mask and shift */
    #define DEV_ID_REV_MASK         0x0000000FUL    /* Revision */
    #define DEV_ID_VER_MASK         0x000000F0UL    /* Version */
    #define DEV_ID_MODEL_MASK       0x0000FF00UL    /* The MODEL identifies the device. The DW1000 is device type 0x01 */
    #define DEV_ID_RIDTAG_MASK      0xFFFF0000UL    /* Register Identification Tag 0XDECA */

    /****************************************************************************//**
     * @brief Bit definitions for register SYS_TIME
    **/
    #define SYS_TIME_ID             0x06            /* System Time Counter (40-bit) */
    #define SYS_TIME_OFFSET         0x00
    #define SYS_TIME_LEN            (5)             /* Note 40 bit register */

    /****************************************************************************//**
     * @brief Bit definitions for register ACC_MEM
    **/
    // CIR DATA - ACCUMULATOR MEMORY
    #define ACC_MEM_ID              0x25            /* Read access to accumulator data */
    #define ACC_MEM_LEN             (4064)

    /****************************************************************************//**
     * @brief Bit definitions for register USR_SFD
     *        Please read User Manual : User defined SFD sequence
    **/
    #define USR_SFD_ID              0x21            /* User-specified short/long TX/RX SFD sequences */
    #define USR_SFD_LEN             (41)
    #define DW_NS_SFD_LEN_110K      64              /* Decawave non-standard SFD length for 110 kbps */
    #define DW_NS_SFD_LEN_850K      16              /* Decawave non-standard SFD length for 850 kbps */
    #define DW_NS_SFD_LEN_6M8       8               /* Decawave non-standard SFD length for 6.8 Mbps */

    /****************************************************************************//**
     * @brief Bit definitions for register TX_ANTD
    **/
    #define TX_ANTD_ID              0x18            /* 16-bit Delay from Transmit to Antenna */
    #define TX_ANTD_OFFSET          0x00
    #define TX_ANTD_LEN             (2)

    /****************************************************************************//**
     * @brief Bit definitions for register TX_BUFFER
    **/
    #define TX_BUFFER_ID            0x09            /* Transmit Data Buffer */
    #define TX_BUFFER_LEN           (1024)

    /****************************************************************************//**
     * @brief Bit definitions for register RX_BUFFER
    **/
    #define RX_BUFFER_ID            0x11            /* Receive Data Buffer (in double buffer set) */
    #define RX_BUFFER_LEN           (1024)

    /****************************************************************************//**
     * @brief Bit definitions for register  DX_TIME
    **/
    #define DX_TIME_ID              0x0A            /* Delayed Send or Receive Time (40-bit) */
    #define DX_TIME_LEN             (5)

    /****************************************************************************//**
     * @brief Bit definitions for register RX_TIME
    **/
    #define RX_TIME_ID              0x15            /* Receive Message Time of Arrival (in double buffer set) */
    #define RX_TIME_LLEN            (14)
    #define RX_TIME_RX_STAMP_LEN    (5)             /* read only 5 bytes (the adjusted timestamp (40:0)) */
    #define RX_STAMP_LEN            RX_TIME_RX_STAMP_LEN
    /*mask and shift */
    #define RX_TIME_RX_STAMP_OFFSET  (0) /* byte 0..4 40 bit Reports the fully adjusted time of reception. */
    #define RX_TIME_FP_INDEX_OFFSET  (5)    /* byte 5..6 16 bit First path index. */
    #define RX_TIME_FP_AMPL1_OFFSET  (7)    /* byte 7..8 16 bit First Path Amplitude - magnitude of 3rd point after Ceiling(FP_Index) */
    #define RX_TIME_FP_RAWST_OFFSET  (9)    /* byte 9..13 40 bit Raw Timestamp for the frame */

    /****************************************************************************//**
     * @brief Bit definitions for register AGC_CTRL
     * Please take care to write to this register as doing so may cause the DW1000 to malfunction
    **/
    #define AGC_CTRL_ID             0x23            /* Automatic Gain Control configuration */
    #define AGC_CTRL_LEN            (32)
    #define AGC_CFG_STS_ID          AGC_CTRL_ID
    /* offset from AGC_CTRL_ID in bytes */
    #define AGC_CTRL1_OFFSET        (0x02)
    #define AGC_CTRL1_LEN           (2)
    #define AGC_CTRL1_MASK          0x0001          /* access mask to AGC configuration and control register */
    #define AGC_CTRL1_DIS_AM        0x0001          /* Disable AGC Measurement. The DIS_AM bit is set by default. */
    /* offset from AGC_CTRL_ID in bytes */
    /* Please take care not to write other values to this register as doing so may cause the DW1000 to malfunction */
    #define AGC_TUNE1_OFFSET        (0x04)
    #define AGC_TUNE1_LEN           (2)
    #define AGC_TUNE1_MASK          0xFFFF          /* It is a 16-bit tuning register for the AGC. */
    #define AGC_TUNE1_16M           0x8870
    #define AGC_TUNE1_64M           0x889B
    /* offset from AGC_CTRL_ID in bytes */
    /* Please take care not to write other values to this register as doing so may cause the DW1000 to malfunction */
    #define AGC_TUNE2_OFFSET        (0x0C)
    #define AGC_TUNE2_LEN           (4)
    #define AGC_TUNE2_MASK          0xFFFFFFFFUL
    #define AGC_TUNE2_VAL           0X2502A907UL
    /* offset from AGC_CTRL_ID in bytes */
    /* Please take care not to write other values to this register as doing so may cause the DW1000 to malfunction */
    #define AGC_TUNE3_OFFSET        (0x12)
    #define AGC_TUNE3_LEN           (2)
    #define AGC_TUNE3_MASK          0xFFFF
    #define AGC_TUNE3_VAL           0X0035
    /* offset from AGC_CTRL_ID in bytes */
    #define AGC_STAT1_OFFSET        (0x1E)
    #define AGC_STAT1_LEN           (3)
    #define AGC_STAT1_MASK          0x0FFFFF
    #define AGC_STAT1_EDG1_MASK     0x0007C0        /* This 5-bit gain value relates to input noise power measurement. */
    #define AGC_STAT1_EDG2_MASK     0x0FF800        /* This 9-bit value relates to the input noise power measurement. */

    /****************************************************************************//**
     * @brief Bit definitions for register PMSC
    **/
    #define PMSC_ID                 0x36            /* Power Management System Control Block */
    #define PMSC_LEN                (48)
    /* offset from PMSC_ID in bytes */
    #define PMSC_CTRL0_OFFSET       0x00
    #define PMSC_CTRL0_LEN          (4)
    #define PMSC_CTRL0_MASK         0xF18F847FUL    /* access mask to register PMSC_CTRL0 */
    #define PMSC_CTRL0_SYSCLKS_AUTO 0x00000000UL    /* The system clock will run off the 19.2 MHz XTI clock until the PLL is calibrated and locked, then it will switch over the 125 MHz PLL clock */
    #define PMSC_CTRL0_SYSCLKS_19M  0x00000001UL    /* Force system clock to be the 19.2 MHz XTI clock. */
    #define PMSC_CTRL0_SYSCLKS_125M 0x00000002UL    /* Force system clock to the 125 MHz PLL clock. */
    #define PMSC_CTRL0_RXCLKS_AUTO  0x00000000UL    /* The RX clock will be disabled until it is required for an RX operation */
    #define PMSC_CTRL0_RXCLKS_19M   0x00000004UL    /* Force RX clock enable and sourced clock from the 19.2 MHz XTI clock */
    #define PMSC_CTRL0_RXCLKS_125M  0x00000008UL    /* Force RX clock enable and sourced from the 125 MHz PLL clock */
    #define PMSC_CTRL0_RXCLKS_OFF   0x0000000CUL    /* Force RX clock off. */
    #define PMSC_CTRL0_TXCLKS_AUTO  0x00000000UL    /* The TX clock will be disabled until it is required for a TX operation */
    #define PMSC_CTRL0_TXCLKS_19M   0x00000010UL    /* Force TX clock enable and sourced clock from the 19.2 MHz XTI clock */
    #define PMSC_CTRL0_TXCLKS_125M  0x00000020UL    /* Force TX clock enable and sourced from the 125 MHz PLL clock */
    #define PMSC_CTRL0_TXCLKS_OFF   0x00000030UL    /* Force TX clock off */
    #define PMSC_CTRL0_FACE         0x00000040UL    /* Force Accumulator Clock Enable */
    #define PMSC_CTRL0_GPDCE        0x00040000UL    /* GPIO De-bounce Clock Enable */
    #define PMSC_CTRL0_KHZCLEN      0x00800000UL    /* Kilohertz Clock Enable */
    #define PMSC_CTRL0_PLL2_SEQ_EN  0x01000000UL    /* Enable PLL2 on/off sequencing by SNIFF mode */
    #define PMSC_CTRL0_SOFTRESET_OFFSET 3           /* In bytes */
    #define PMSC_CTRL0_RESET_ALL    0x00            /* Assuming only 4th byte of the register is read */
    #define PMSC_CTRL0_RESET_RX     0xE0            /* Assuming only 4th byte of the register is read */
    #define PMSC_CTRL0_RESET_CLEAR  0xF0            /* Assuming only 4th byte of the register is read */
    /* offset from PMSC_ID in bytes */
    #define PMSC_CTRL1_OFFSET       0x04
    #define PMSC_CTRL1_LEN          (4)
    #define PMSC_CTRL1_MASK         0xFC02F802UL    /* access mask to register PMSC_CTRL1 */
    #define PMSC_CTRL1_ARX2INIT     0x00000002UL    /* Automatic transition from receive mode into the INIT state */
    #define PMSC_CTRL1_ATXSLP       0x00000800UL    /* If this bit is set then the DW1000 will automatically transition into SLEEP or DEEPSLEEP mode after transmission of a frame */
    #define PMSC_CTRL1_ARXSLP       0x00001000UL    /* this bit is set then the DW1000 will automatically transition into SLEEP mode after a receive attempt */
    #define PMSC_CTRL1_SNOZE        0x00002000UL    /* Snooze Enable */
    #define PMSC_CTRL1_SNOZR        0x00004000UL    /* The SNOZR bit is set to allow the snooze timer to repeat twice */
    #define PMSC_CTRL1_PLLSYN       0x00008000UL    /* This enables a special 1 GHz clock used for some external SYNC modes */
    #define PMSC_CTRL1_LDERUNE      0x00020000UL    /* This bit enables the running of the LDE algorithm */
    #define PMSC_CTRL1_KHZCLKDIV_MASK   0xFC000000UL    /* Kilohertz clock divisor */
    #define PMSC_CTRL1_PKTSEQ_DISABLE   0x00        /* writing this to PMSC CONTROL 1 register (bits 10-3) disables PMSC control of analog RF subsystems */
    #define PMSC_CTRL1_PKTSEQ_ENABLE    0xE7        /* writing this to PMSC CONTROL 1 register (bits 10-3) enables PMSC control of analog RF subsystems */
    /* offset from PMSC_ID in bytes */
    #define PMSC_RES1_OFFSET        0x08
    /* offset from PMSC_ID in bytes */
    #define PMSC_SNOZT_OFFSET       0x0C            /* PMSC Snooze Time Register */
    #define PMSC_SNOZT_LEN          (1)
    /* offset from PMSC_ID in bytes */
    #define PMSC_RES2_OFFSET        0x10
    /* offset from PMSC_ID in bytes */
    #define PMSC_RES3_OFFSET        0x24
    /* offset from PMSC_ID in bytes */
    #define PMSC_TXFINESEQ_OFFSET   0x26
    #define PMSC_TXFINESEQ_DISABLE  0x0             /* Writing this disables fine grain sequencing in the transmitter */
    #define PMSC_TXFINESEQ_ENABLE   0x0B74          /* Writing this enables fine grain sequencing in the transmitter */
    /* offset from PMSC_ID in bytes */
    #define PMSC_LEDC_OFFSET        0x28
    #define PMSC_LEDC_LEN           (4)
    #define PMSC_LEDC_MASK          0x000001FFUL    /* 32-bit LED control register. */
    #define PMSC_LEDC_BLINK_TIM_MASK 0x000000FFUL   /* This field determines how long the LEDs remain lit after an event that causes them to be set on. */
    #define PMSC_LEDC_BLNKEN        0x00000100UL    /* Blink Enable. When this bit is set to 1 the LED blink feature is enabled. */
    /* Default blink time. Blink time is expressed in multiples of 14 ms. The value defined here is ~225 ms. */
    #define PMSC_LEDC_BLINK_TIME_DEF 0x10
    /* Command a blink of all LEDs */
    #define PMSC_LEDC_BLINK_NOW_ALL 0x000F0000UL

    /****************************************************************************//**
     * @brief Bit definitions for register SYS_CFG
    **/
    #define SYS_CFG_ID              0x04            /* System Configuration (31:0) */
    #define SYS_CFG_LEN             (4)
    /*mask and shift */
    #define SYS_CFG_MASK            0xF047FFFFUL    /* access mask to SYS_CFG_ID */
    #define SYS_CFG_FF_ALL_EN       0x000001FEUL    /* Frame filtering options all frames allowed */
    /*offset 0 */
    #define SYS_CFG_FFE             0x00000001UL    /* Frame Filtering Enable. This bit enables the frame filtering functionality */
    #define SYS_CFG_FFBC            0x00000002UL    /* Frame Filtering Behave as a Co-ordinator */
    #define SYS_CFG_FFAB            0x00000004UL    /* Frame Filtering Allow Beacon frame reception */
    #define SYS_CFG_FFAD            0x00000008UL    /* Frame Filtering Allow Data frame reception */
    #define SYS_CFG_FFAA            0x00000010UL    /* Frame Filtering Allow Acknowledgment frame reception */
    #define SYS_CFG_FFAM            0x00000020UL    /* Frame Filtering Allow MAC command frame reception */
    #define SYS_CFG_FFAR            0x00000040UL    /* Frame Filtering Allow Reserved frame types */
    #define SYS_CFG_FFA4            0x00000080UL    /* Frame Filtering Allow frames with frame type field of 4, (binary 100) */
    /*offset 8 */
    #define SYS_CFG_FFA5            0x00000100UL    /* Frame Filtering Allow frames with frame type field of 5, (binary 101) */
    #define SYS_CFG_HIRQ_POL        0x00000200UL    /* Host interrupt polarity */
    #define SYS_CFG_SPI_EDGE        0x00000400UL    /* SPI data launch edge */
    #define SYS_CFG_DIS_FCE         0x00000800UL    /* Disable frame check error handling */
    #define SYS_CFG_DIS_DRXB        0x00001000UL    /* Disable Double RX Buffer */
    #define SYS_CFG_DIS_PHE         0x00002000UL    /* Disable receiver abort on PHR error */
    #define SYS_CFG_DIS_RSDE        0x00004000UL    /* Disable Receiver Abort on RSD error */
    #define SYS_CFG_FCS_INIT2F      0x00008000UL    /* initial seed value for the FCS generation and checking function */
    /*offset 16 */
    #define SYS_CFG_PHR_MODE_SHFT   16
    #define SYS_CFG_PHR_MODE_00     0x00000000UL    /* Standard Frame mode */
    #define SYS_CFG_PHR_MODE_11     0x00030000UL    /* Long Frames mode */
    #define SYS_CFG_DIS_STXP        0x00040000UL    /* Disable Smart TX Power control */
    #define SYS_CFG_RXM110K         0x00400000UL    /* Receiver Mode 110 kbps data rate */
    /*offset 24 */
    #define SYS_CFG_RXWTOE          0x10000000UL    /* Receive Wait Timeout Enable. */
    #define SYS_CFG_RXAUTR          0x20000000UL    /* Receiver Auto-Re-enable. This bit is used to cause the receiver to re-enable automatically */
    #define SYS_CFG_AUTOACK         0x40000000UL    /* Automatic Acknowledgement Enable */
    #define SYS_CFG_AACKPEND        0x80000000UL    /* Automatic Acknowledgement Pending bit control */

    /****************************************************************************//**
     * @brief Bit definitions for register  DRX_CONF
     * Digital Receiver configuration block
    **/
    #define DRX_CONF_ID             0x27            /* Digital Receiver configuration */
    #define DRX_CONF_LEN            (44)
    /* offset from DRX_CONF_ID in bytes */
    #define DRX_TUNE0b_OFFSET       (0x02)  /* sub-register 0x02 is a 16-bit tuning register. */
    #define DRX_TUNE0b_LEN          (2)
    #define DRX_TUNE0b_MASK         0xFFFF  /* 7.2.40.2 Sub-Register 0x27:02 – DRX_TUNE0b */
    #define DRX_TUNE0b_110K_STD     0x000A
    #define DRX_TUNE0b_110K_NSTD    0x0016
    #define DRX_TUNE0b_850K_STD     0x0001
    #define DRX_TUNE0b_850K_NSTD    0x0006
    #define DRX_TUNE0b_6M8_STD      0x0001
    #define DRX_TUNE0b_6M8_NSTD     0x0002

    /* offset from DRX_CONF_ID in bytes */
    #define DRX_TUNE1a_OFFSET       0x04    /* 7.2.40.3 Sub-Register 0x27:04 – DRX_TUNE1a */
    #define DRX_TUNE1a_LEN          (2)
    #define DRX_TUNE1a_MASK         0xFFFF
    #define DRX_TUNE1a_PRF16        0x0087
    #define DRX_TUNE1a_PRF64        0x008D

    /* offset from DRX_CONF_ID in bytes */
    #define DRX_TUNE1b_OFFSET       0x06    /* 7.2.40.4 Sub-Register 0x27:06 – DRX_TUNE1b */
    #define DRX_TUNE1b_LEN          (2)
    #define DRX_TUNE1b_MASK         0xFFFF
    #define DRX_TUNE1b_110K         0x0064
    #define DRX_TUNE1b_850K_6M8     0x0020
    #define DRX_TUNE1b_6M8_PRE64    0x0010

    /* offset from DRX_CONF_ID in bytes */
    #define DRX_TUNE2_OFFSET        0x08    /* 7.2.40.5 Sub-Register 0x27:08 – DRX_TUNE2 */
    #define DRX_TUNE2_LEN           (4)
    #define DRX_TUNE2_MASK          0xFFFFFFFFUL
    #define DRX_TUNE2_PRF16_PAC8    0x311A002DUL
    #define DRX_TUNE2_PRF16_PAC16   0x331A0052UL
    #define DRX_TUNE2_PRF16_PAC32   0x351A009AUL
    #define DRX_TUNE2_PRF16_PAC64   0x371A011DUL
    #define DRX_TUNE2_PRF64_PAC8    0x313B006BUL
    #define DRX_TUNE2_PRF64_PAC16   0x333B00BEUL
    #define DRX_TUNE2_PRF64_PAC32   0x353B015EUL
    #define DRX_TUNE2_PRF64_PAC64   0x373B0296UL

    /* offset from DRX_CONF_ID in bytes */
    /* WARNING: Please do NOT set DRX_SFDTOC to zero (disabling SFD detection timeout)
     * since this risks IC malfunction due to prolonged receiver activity in the event of false preamble detection.
     */
    #define DRX_SFDTOC_OFFSET       0x20    /* 7.2.40.7 Sub-Register 0x27:20 – DRX_SFDTOC */
    #define DRX_SFDTOC_LEN          (2)
    #define DRX_SFDTOC_MASK         0xFFFF

    /* offset from DRX_CONF_ID in bytes */
    #define DRX_PRETOC_OFFSET       0x24    /* 7.2.40.9 Sub-Register 0x27:24 – DRX_PRETOC */
    #define DRX_PRETOC_LEN          (2)
    #define DRX_PRETOC_MASK         0xFFFF

    /* offset from DRX_CONF_ID in bytes */
    #define DRX_TUNE4H_OFFSET       0x26    /* 7.2.40.10 Sub-Register 0x27:26 – DRX_TUNE4H */
    #define DRX_TUNE4H_LEN          (2)
    #define DRX_TUNE4H_MASK         0xFFFF
    #define DRX_TUNE4H_PRE64        0x0010
    #define DRX_TUNE4H_PRE128PLUS   0x0028

    /* offset from DRX_CONF_ID in bytes to 21-bit signed RX carrier integrator value */
    #define DRX_CARRIER_INT_OFFSET  0x28
    #define DRX_CARRIER_INT_LEN     (3)
    #define DRX_CARRIER_INT_MASK    0x001FFFFF

    /****************************************************************************//**
     * @brief Bit definitions for register CHAN_CTRL
    **/
    #define CHAN_CTRL_ID            0x1F            /* Channel Control */
    #define CHAN_CTRL_LEN           (4)
    /*mask and shift */
    #define CHAN_CTRL_MASK          0xFFFF00FFUL    /* Channel Control Register access mask */
    #define CHAN_CTRL_TX_CHAN_MASK  0x0000000FUL    /* Supported channels are 1, 2, 3, 4, 5, and 7.*/
    #define CHAN_CTRL_TX_CHAN_SHIFT (0)             /* Bits 0..3        TX channel number 0-15 selection */

    #define CHAN_CTRL_RX_CHAN_MASK  0x000000F0UL
    #define CHAN_CTRL_RX_CHAN_SHIFT (4)             /* Bits 4..7        RX channel number 0-15 selection */

    #define CHAN_CTRL_RXFPRF_MASK   0x000C0000UL    /* Bits 18..19      Specify (Force) RX Pulse Repetition Rate: 00 = 4 MHz, 01 = 16 MHz, 10 = 64MHz. */
    #define CHAN_CTRL_RXFPRF_SHIFT  (18)
    /* Specific RXFPRF configuration */
    #define CHAN_CTRL_RXFPRF_4      0x00000000UL    /* Specify (Force) RX Pulse Repetition Rate: 00 = 4 MHz, 01 = 16 MHz, 10 = 64MHz. */
    #define CHAN_CTRL_RXFPRF_16     0x00040000UL    /* Specify (Force) RX Pulse Repetition Rate: 00 = 4 MHz, 01 = 16 MHz, 10 = 64MHz. */
    #define CHAN_CTRL_RXFPRF_64     0x00080000UL    /* Specify (Force) RX Pulse Repetition Rate: 00 = 4 MHz, 01 = 16 MHz, 10 = 64MHz. */
    #define CHAN_CTRL_TX_PCOD_MASK  0x07C00000UL    /* Bits 22..26      TX Preamble Code selection, 1 to 24. */
    #define CHAN_CTRL_TX_PCOD_SHIFT (22)
    #define CHAN_CTRL_RX_PCOD_MASK  0xF8000000UL    /* Bits 27..31      RX Preamble Code selection, 1 to 24. */
    #define CHAN_CTRL_RX_PCOD_SHIFT (27)
    /*offset 16 */
    #define CHAN_CTRL_DWSFD         0x00020000UL    /* Bit 17 This bit enables a non-standard DecaWave proprietary SFD sequence. */
    #define CHAN_CTRL_DWSFD_SHIFT   (17)
    #define CHAN_CTRL_TNSSFD        0x00100000UL    /* Bit 20 Non-standard SFD in the transmitter */
    #define CHAN_CTRL_TNSSFD_SHIFT  (20)
    #define CHAN_CTRL_RNSSFD        0x00200000UL    /* Bit 21 Non-standard SFD in the receiver */
    #define CHAN_CTRL_RNSSFD_SHIFT  (21)

    /****************************************************************************//**
     * @brief Bit definitions for register
    **/
    #define AON_ID                  0x2C            /* Always-On register set */
    #define AON_LEN                 (12)
    /* offset from AON_ID in bytes */
    #define AON_WCFG_OFFSET         0x00    /* used to control what the DW1000 IC does as it wakes up from low-power SLEEP or DEEPSLEEPstates. */
    #define AON_WCFG_LEN            (2)
    #define AON_WCFG_MASK           0x09CB  /* access mask to AON_WCFG register*/
    #define AON_WCFG_ONW_RADC       0x0001  /* On Wake-up Run the (temperature and voltage) Analog-to-Digital Convertors */
    #define AON_WCFG_ONW_RX         0x0002  /* On Wake-up turn on the Receiver */
    #define AON_WCFG_ONW_LEUI       0x0008  /* On Wake-up load the EUI from OTP memory into Register file: 0x01 – Extended Unique Identifier. */
    #define AON_WCFG_ONW_LDC        0x0040  /* On Wake-up load configurations from the AON memory into the host interface register set */
    #define AON_WCFG_ONW_L64P       0x0080  /* On Wake-up load the Length64 receiver operating parameter set */
    #define AON_WCFG_PRES_SLEEP     0x0100  /* Preserve Sleep. This bit determines what the DW1000 does with respect to the ARXSLP and ATXSLP sleep controls */
    #define AON_WCFG_ONW_LLDE       0x0800  /* On Wake-up load the LDE microcode. */
    #define AON_WCFG_ONW_LLDO       0x1000  /* On Wake-up load the LDO tune value. */
    /* offset from AON_ID in bytes */
    #define AON_CTRL_OFFSET         0x02    /* The bits in this register in general cause direct activity within the AON block with respect to the stored AON memory */
    #define AON_CTRL_LEN            (1)
    #define AON_CTRL_MASK           0x8F    /* access mask to AON_CTRL register */
    #define AON_CTRL_RESTORE        0x01    /* When this bit is set the DW1000 will copy the user configurations from the AON memory to the host interface register set. */
    #define AON_CTRL_SAVE           0x02    /* When this bit is set the DW1000 will copy the user configurations from the host interface register set into the AON memory */
    #define AON_CTRL_UPL_CFG        0x04    /* Upload the AON block configurations to the AON  */
    #define AON_CTRL_DCA_READ       0x08    /* Direct AON memory access read */
    #define AON_CTRL_DCA_ENAB       0x80    /* Direct AON memory access enable bit */
    /* offset from AON_ID in bytes */
    #define AON_RDAT_OFFSET         0x03    /* AON Direct Access Read Data Result */
    #define AON_RDAT_LEN            (1)
    /* offset from AON_ID in bytes */
    #define AON_ADDR_OFFSET         0x04    /* AON Direct Access Address */
    #define AON_ADDR_LEN            (1)
    #define AON_ADDR_LPOSC_CAL_0    117     /* Address of low-power oscillator calibration value (lower byte) */
    #define AON_ADDR_LPOSC_CAL_1    118     /* Address of low-power oscillator calibration value (lower byte) */

    /* offset from AON_ID in bytes */
    #define AON_CFG0_OFFSET             0x06    /* 32-bit configuration register for the always on block. */
    #define AON_CFG0_LEN                (4)
    #define AON_CFG0_SLEEP_EN           0x00000001UL    /* This is the sleep enable configuration bit */
    #define AON_CFG0_WAKE_PIN           0x00000002UL    /* Wake using WAKEUP pin */
    #define AON_CFG0_WAKE_SPI           0x00000004UL    /* Wake using SPI access SPICSn */
    #define AON_CFG0_WAKE_CNT           0x00000008UL    /* Wake when sleep counter elapses */
    #define AON_CFG0_LPDIV_EN           0x00000010UL    /* Low power divider enable configuration */
    #define AON_CFG0_LPCLKDIVA_MASK     0x0000FFE0UL    /* divider count for dividing the raw DW1000 XTAL oscillator frequency to set an LP clock frequency */
    #define AON_CFG0_LPCLKDIVA_SHIFT    (5)
    #define AON_CFG0_SLEEP_TIM          0xFFFF0000UL    /* Sleep time. This field configures the sleep time count elapse value */
    #define AON_CFG0_SLEEP_SHIFT        (16)
    #define AON_CFG0_SLEEP_TIM_OFFSET   2               /* In bytes */
    /* offset from AON_ID in bytes */
    #define AON_CFG1_OFFSET         0x0A
    #define AON_CFG1_LEN            (2)
    #define AON_CFG1_MASK           0x0007  /* aceess mask to AON_CFG1 */
    #define AON_CFG1_SLEEP_CEN      0x0001  /* This bit enables the sleep counter */
    #define AON_CFG1_SMXX           0x0002  /* This bit needs to be set to 0 for correct operation in the SLEEP state within the DW1000 */
    #define AON_CFG1_LPOSC_CAL      0x0004  /* This bit enables the calibration function that measures the period of the IC’s internal low powered oscillator */

    /****************************************************************************//**
     * @brief Bit definitions for register OTP_IF
     * Refer to section 7.2.46 Register file: 0x2D – OTP Memory Interface
    **/
    #define OTP_IF_ID               0x2D            /* One Time Programmable Memory Interface */
    #define OTP_IF_LEN              (18)
    /* offset from OTP_IF_ID in bytes */
    #define OTP_WDAT                0x00            /* 32-bit register. The data value to be programmed into an OTP location  */
    #define OTP_WDAT_LEN            (4)
    /* offset from OTP_IF_ID in bytes */
    #define OTP_ADDR                0x04            /* 16-bit register used to select the address within the OTP memory block */
    #define OTP_ADDR_LEN            (2)
    #define OTP_ADDR_MASK           0x07FF          /* This 11-bit field specifies the address within OTP memory that will be accessed read or written. */
    /* offset from OTP_IF_ID in bytes */
    #define OTP_CTRL                0x06            /* used to control the operation of the OTP memory */
    #define OTP_CTRL_LEN            (2)
    #define OTP_CTRL_MASK           0x8002
    #define OTP_CTRL_OTPRDEN        0x0001          /* This bit forces the OTP into manual read mode */
    #define OTP_CTRL_OTPREAD        0x0002          /* This bit commands a read operation from the address specified in the OTP_ADDR register */
    #define OTP_CTRL_LDELOAD        0x8000          /* This bit forces a load of LDE microcode */
    #define OTP_CTRL_OTPPROG        0x0040          /* Setting this bit will cause the contents of OTP_WDAT to be written to OTP_ADDR. */
    /* offset from OTP_IF_ID in bytes */
    #define OTP_STAT                0x08
    #define OTP_STAT_LEN            (2)
    #define OTP_STAT_MASK           0x0003
    #define OTP_STAT_OTPPRGD        0x0001          /* OTP Programming Done */
    #define OTP_STAT_OTPVPOK        0x0002          /* OTP Programming Voltage OK */
    /* offset from OTP_IF_ID in bytes */
    #define OTP_RDAT                0x0A            /* 32-bit register. The data value read from an OTP location will appear here */
    #define OTP_RDAT_LEN            (4)
    /* offset from OTP_IF_ID in bytes */
    #define OTP_SRDAT               0x0E            /* 32-bit register. The data value stored in the OTP SR (0x400) location will appear here after power up */
    #define OTP_SRDAT_LEN           (4)
    /* offset from OTP_IF_ID in bytes */
    #define OTP_SF                  0x12            /*8-bit special function register used to select and load special receiver operational parameter */
    #define OTP_SF_LEN              (1)
    #define OTP_SF_MASK             0x63
    #define OTP_SF_OPS_KICK         0x01            /* This bit when set initiates a load of the operating parameter set selected by the OPS_SEL */
    #define OTP_SF_LDO_KICK         0x02            /* This bit when set initiates a load of the LDO tune code */
    #define OTP_SF_OPS_SEL_SHFT     5
    #define OTP_SF_OPS_SEL_MASK     0x60
    #define OTP_SF_OPS_SEL_L64      0x00            /* Operating parameter set selection: Length64 */
    #define OTP_SF_OPS_SEL_TIGHT    0x20            /* Operating parameter set selection: Tight */

    /****************************************************************************//**
     * @brief Bit definitions for register
     * Refer to section 7.2.44 Register file: 0x2B – Frequency synthesiser control block
    **/
    #define FS_CTRL_ID              0x2B            /* Frequency synthesiser control block */
    #define FS_CTRL_LEN             (21)
    /* offset from FS_CTRL_ID in bytes */
    #define FS_RES1_OFFSET          0x00            /* reserved area. Please take care not to write to this area as doing so may cause the DW1000 to malfunction. */
    #define FS_RES1_LEN             (7)
    /* offset from FS_CTRL_ID in bytes */
    #define FS_PLLCFG_OFFSET        0x07            /* Frequency synthesiser – PLL configuration */
    #define FS_PLLCFG_LEN           (5)
    #define FS_PLLCFG_CH1           0x09000407UL    /* Operating Channel 1 */
    #define FS_PLLCFG_CH2           0x08400508UL    /* Operating Channel 2 */
    #define FS_PLLCFG_CH3           0x08401009UL    /* Operating Channel 3 */
    #define FS_PLLCFG_CH4           FS_PLLCFG_CH2   /* Operating Channel 4 (same as 2) */
    #define FS_PLLCFG_CH5           0x0800041DUL    /* Operating Channel 5 */
    #define FS_PLLCFG_CH7           FS_PLLCFG_CH5   /* Operating Channel 7 (same as 5) */
    /* offset from FS_CTRL_ID in bytes */
    #define FS_PLLTUNE_OFFSET       0x0B            /* Frequency synthesiser – PLL Tuning */
    #define FS_PLLTUNE_LEN          (1)
    #define FS_PLLTUNE_CH1          0x1E            /* Operating Channel 1 */
    #define FS_PLLTUNE_CH2          0x26            /* Operating Channel 2 */
    #define FS_PLLTUNE_CH3          0x56            /* Operating Channel 3 */
    #define FS_PLLTUNE_CH4          FS_PLLTUNE_CH2  /* Operating Channel 4 (same as 2) */
    #define FS_PLLTUNE_CH5          0xBE            /* Operating Channel 5 */
    #define FS_PLLTUNE_CH7          FS_PLLTUNE_CH5  /* Operating Channel 7 (same as 5) */
    /* offset from FS_CTRL_ID in bytes */
    #define FS_RES2_OFFSET          0x0C    /* reserved area. Please take care not to write to this area as doing so may cause the DW1000 to malfunction. */
    #define FS_RES2_LEN             (2)
    /* offset from FS_CTRL_ID in bytes */
    #define FS_XTALT_OFFSET         0x0E    /* Frequency synthesiser – Crystal trim */
    #define FS_XTALT_LEN            (1)
    #define FS_XTALT_MASK           0x1F    /* Crystal Trim. Crystals may be trimmed using this register setting to tune out errors, see 8.1 – IC Calibration – Crystal Oscillator Trim. */
    #define FS_XTALT_MIDRANGE       0x10
    /* offset from FS_CTRL_ID in bytes */
    #define FS_RES3_OFFSET          0x0F    /* reserved area. Please take care not to write to this area as doing so may cause the DW1000 to malfunction. */
    #define FS_RES3_LEN             (6)

    /****************************************************************************//**
     * @brief Bit definitions for register EXT_SYNC
    **/
    #define EXT_SYNC_ID             0x24            /* External synchronisation control */
    #define EXT_SYNC_LEN            (12)
    /* offset from EXT_SYNC_ID in bytes */
    #define EC_CTRL_OFFSET          (0x00)
    #define EC_CTRL_LEN             (4)
    #define EC_CTRL_MASK            0x00000FFBUL    /* sub-register 0x00 is the External clock synchronisation counter configuration register */
    #define EC_CTRL_OSTSM           0x00000001UL    /* External transmit synchronisation mode enable */
    #define EC_CTRL_OSRSM           0x00000002UL    /* External receive synchronisation mode enable */
    #define EC_CTRL_PLLLCK          0x04            /* PLL lock detect enable */
    #define EC_CTRL_OSTRM           0x00000800UL    /* External timebase reset mode enable */
    #define EC_CTRL_WAIT_MASK       0x000007F8UL    /* Wait counter used for external transmit synchronisation and external timebase reset */
    /* offset from EXT_SYNC_ID in bytes */
    #define EC_RXTC_OFFSET          (0x04)
    #define EC_RXTC_LEN             (4)
    #define EC_RXTC_MASK            0xFFFFFFFFUL    /* External clock synchronisation counter captured on RMARKER */
    /* offset from EXT_SYNC_ID in bytes */
    #define EC_GOLP                 (0x08)
    #define EC_GOLP_LEN             (4)
    #define EC_GOLP_MASK            0x0000003FUL    /* sub-register 0x08 is the External clock offset to first path 1 GHz counter, EC_GOLP */
    #define EC_GOLP_OFFSET_EXT_MASK 0x0000003FUL    /* This register contains the 1 GHz count from the arrival of the RMARKER and the next edge of the external clock. */


    /****************************************************************************//**
     * @brief Bit definitions for register LDE_IF
     * Refer to section 7.2.47 Register file: 0x2E – Leading Edge Detection Interface
     * PLEASE NOTE: Other areas within the address space of Register file: 0x2E – Leading Edge Detection Interface
     * are reserved. To ensure proper operation of the LDE algorithm (i.e. to avoid loss of performance or a malfunction),
     * care must be taken not to write to any byte locations other than those defined in the sub-sections below.
    **/
    #define LDE_IF_ID               0x2E            /* Leading edge detection control block */
    #define LDE_IF_LEN              (0)
    /* offset from LDE_IF_ID in bytes */
    #define LDE_THRESH_OFFSET       0x0000  /* 16-bit status register reporting the threshold that was used to find the first path */
    #define LDE_THRESH_LEN          (2)
    /* offset from LDE_IF_ID in bytes */
    #define LDE_CFG1_OFFSET         0x0806  /*8-bit configuration register*/
    #define LDE_CFG1_LEN            (1)
    #define LDE_CFG1_NSTDEV_MASK    0x1F    /* Number of Standard Deviations mask. */
    #define LDE_CFG1_PMULT_MASK     0xE0    /* Peak Multiplier mask. */
    /* offset from LDE_IF_ID in bytes */
    #define LDE_PPINDX_OFFSET       0x1000  /* reporting the position within the accumulator that the LDE algorithm has determined to contain the maximum */
    #define LDE_PPINDX_LEN          (2)
    /* offset from LDE_IF_ID in bytes */
    #define LDE_PPAMPL_OFFSET       0x1002  /* reporting the magnitude of the peak signal seen in the accumulator data memory */
    #define LDE_PPAMPL_LEN          (2)
    /* offset from LDE_IF_ID in bytes */
    #define LDE_RXANTD_OFFSET       0x1804  /* 16-bit configuration register for setting the receive antenna delay */
    #define LDE_RXANTD_LEN          (2)
    /* offset from LDE_IF_ID in bytes */
    #define LDE_CFG2_OFFSET         0x1806  /* 16-bit LDE configuration tuning register */
    #define LDE_CFG2_LEN            (2)
    /* offset from LDE_IF_ID in bytes */
    #define LDE_REPC_OFFSET         0x2804  /* 16-bit configuration register for setting the replica avoidance coefficient */
    #define LDE_REPC_LEN            (2)
    #define LDE_REPC_PCODE_1        0x5998
    #define LDE_REPC_PCODE_2        0x5998
    #define LDE_REPC_PCODE_3        0x51EA
    #define LDE_REPC_PCODE_4        0x428E
    #define LDE_REPC_PCODE_5        0x451E
    #define LDE_REPC_PCODE_6        0x2E14
    #define LDE_REPC_PCODE_7        0x8000
    #define LDE_REPC_PCODE_8        0x51EA
    #define LDE_REPC_PCODE_9        0x28F4
    #define LDE_REPC_PCODE_10       0x3332
    #define LDE_REPC_PCODE_11       0x3AE0
    #define LDE_REPC_PCODE_12       0x3D70
    #define LDE_REPC_PCODE_13       0x3AE0
    #define LDE_REPC_PCODE_14       0x35C2
    #define LDE_REPC_PCODE_15       0x2B84
    #define LDE_REPC_PCODE_16       0x35C2
    #define LDE_REPC_PCODE_17       0x3332
    #define LDE_REPC_PCODE_18       0x35C2
    #define LDE_REPC_PCODE_19       0x35C2
    #define LDE_REPC_PCODE_20       0x47AE
    #define LDE_REPC_PCODE_21       0x3AE0
    #define LDE_REPC_PCODE_22       0x3850
    #define LDE_REPC_PCODE_23       0x30A2
    #define LDE_REPC_PCODE_24       0x3850

    /****************************************************************************//**
     * @brief Bit definitions for register  RF_CONF
     * Analog RF Configuration block
     * Refer to section 7.2.41 Register file: 0x28 – Analog RF configuration block
    **/
    #define RF_CONF_ID              0x28            /* Analog RF Configuration */
    #define RF_CONF_LEN             (58)
    #define RF_CONF_TXEN_MASK       0x00400000UL   /* TX enable */
    #define RF_CONF_RXEN_MASK       0x00200000UL   /* RX enable */
    #define RF_CONF_TXPOW_MASK      0x001F0000UL   /* turn on power all LDOs */
    #define RF_CONF_PLLEN_MASK      0x0000E000UL   /* enable PLLs */
    #define RF_CONF_PGMIXBIASEN_MASK    0x0000A700UL    /* Enable TX mixer bias and pulse gen */
    #define RF_CONF_TXBLOCKSEN_MASK 0x00001F00UL   /* enable TX blocks */
    #define RF_CONF_TXPLLPOWEN_MASK (RF_CONF_PLLEN_MASK | RF_CONF_TXPOW_MASK)
    #define RF_CONF_TXALLEN_MASK    (RF_CONF_TXEN_MASK | RF_CONF_TXPOW_MASK | RF_CONF_PLLEN_MASK | RF_CONF_TXBLOCKSEN_MASK)
    /* offset from TX_CAL_ID in bytes */
    #define RF_RXCTRLH_OFFSET       0x0B            /* Analog RX Control Register */
    #define RF_RXCTRLH_LEN          (1)
    #define RF_RXCTRLH_NBW          0xD8            /* RXCTRLH value for narrow bandwidth channels */
    #define RF_RXCTRLH_WBW          0xBC            /* RXCTRLH value for wide bandwidth channels */
    /* offset from TX_CAL_ID in bytes */
    #define RF_TXCTRL_OFFSET        0x0C            /* Analog TX Control Register */
    #define RF_TXCTRL_LEN           (4)
    #define RF_TXCTRL_TXMTUNE_MASK  0x000001E0UL    /* Transmit mixer tuning register */
    #define RF_TXCTRL_TXTXMQ_MASK   0x00000E00UL    /* Transmit mixer Q-factor tuning register */
    #define RF_TXCTRL_CH1           0x00005C40UL    /* 32-bit value to program to Sub-Register 0x28:0C – RF_TXCTRL */
    #define RF_TXCTRL_CH2           0x00045CA0UL    /* 32-bit value to program to Sub-Register 0x28:0C – RF_TXCTRL */
    #define RF_TXCTRL_CH3           0x00086CC0UL    /* 32-bit value to program to Sub-Register 0x28:0C – RF_TXCTRL */
    #define RF_TXCTRL_CH4           0x00045C80UL    /* 32-bit value to program to Sub-Register 0x28:0C – RF_TXCTRL */
    #define RF_TXCTRL_CH5           0x001E3FE0UL    /* 32-bit value to program to Sub-Register 0x28:0C – RF_TXCTRL */
    #define RF_TXCTRL_CH7           0x001E7DE0UL    /* 32-bit value to program to Sub-Register 0x28:0C – RF_TXCTRL */

    /* offset from TX_CAL_ID in bytes */
    #define RF_STATUS_OFFSET        0x2C

    /****************************************************************************//**
     * @brief Bit definitions for register TX_FCTRL
    **/
    #define TX_FCTRL_ID             0x08            /* Transmit Frame Control */
    #define TX_FCTRL_LEN            (5)             /* Note 40 bit register */
    /*masks (low 32 bit) */
    #define TX_FCTRL_TFLEN_MASK     0x0000007FUL    /* bit mask to access Transmit Frame Length */
    #define TX_FCTRL_TFLE_MASK      0x00000380UL    /* bit mask to access Transmit Frame Length Extension */
    #define TX_FCTRL_FLE_MASK       0x000003FFUL    /* bit mask to access Frame Length field */
    #define TX_FCTRL_TXBR_MASK      0x00006000UL    /* bit mask to access Transmit Bit Rate */
    #define TX_FCTRL_TXPRF_MASK     0x00030000UL    /* bit mask to access Transmit Pulse Repetition Frequency */
    #define TX_FCTRL_TXPSR_MASK     0x000C0000UL    /* bit mask to access Transmit Preamble Symbol Repetitions (PSR). */
    #define TX_FCTRL_PE_MASK        0x00300000UL    /* bit mask to access Preamble Extension */
    #define TX_FCTRL_TXPSR_PE_MASK  0x003C0000UL    /* bit mask to access Transmit Preamble Symbol Repetitions (PSR). */
    #define TX_FCTRL_SAFE_MASK_32   0xFFFFE3FFUL    /* FSCTRL has fields which should always be writen zero */
    /*offset 0 */
    /*offset 8 */
    #define TX_FCTRL_TXBR_110k      0x00000000UL    /* Transmit Bit Rate = 110k */
    #define TX_FCTRL_TXBR_850k      0x00002000UL    /* Transmit Bit Rate = 850k */
    #define TX_FCTRL_TXBR_6M        0x00004000UL    /* Transmit Bit Rate = 6.8M */
    #define TX_FCTRL_TXBR_SHFT      (13)            /* shift to access Data Rate field */
    #define TX_FCTRL_TR             0x00008000UL    /* Transmit Ranging enable */
    #define TX_FCTRL_TR_SHFT        (15)            /* shift to access Ranging bit */
    /*offset 16 */
    #define TX_FCTRL_TXPRF_SHFT     (16)            /* shift to access Pulse Repetition Frequency field */
    #define TX_FCTRL_TXPRF_4M       0x00000000UL    /* Transmit Pulse Repetition Frequency = 4 Mhz */
    #define TX_FCTRL_TXPRF_16M      0x00010000UL    /* Transmit Pulse Repetition Frequency = 16 Mhz */
    #define TX_FCTRL_TXPRF_64M      0x00020000UL    /* Transmit Pulse Repetition Frequency = 64 Mhz */
    #define TX_FCTRL_TXPSR_SHFT     (18)            /* shift to access Preamble Symbol Repetitions field */
    #define TX_FCTRL_PE_SHFT        (20)            /* shift to access Preamble length Extension to allow specification of non-standard values */
    #define TX_FCTRL_TXPSR_PE_16    0x00000000UL    /* bit mask to access Preamble Extension = 16 */
    #define TX_FCTRL_TXPSR_PE_64    0x00040000UL    /* bit mask to access Preamble Extension = 64 */
    #define TX_FCTRL_TXPSR_PE_128   0x00140000UL    /* bit mask to access Preamble Extension = 128 */
    #define TX_FCTRL_TXPSR_PE_256   0x00240000UL    /* bit mask to access Preamble Extension = 256 */
    #define TX_FCTRL_TXPSR_PE_512   0x00340000UL    /* bit mask to access Preamble Extension = 512 */
    #define TX_FCTRL_TXPSR_PE_1024  0x00080000UL    /* bit mask to access Preamble Extension = 1024 */
    #define TX_FCTRL_TXPSR_PE_1536  0x00180000UL    /* bit mask to access Preamble Extension = 1536 */
    #define TX_FCTRL_TXPSR_PE_2048  0x00280000UL    /* bit mask to access Preamble Extension = 2048 */
    #define TX_FCTRL_TXPSR_PE_4096  0x000C0000UL    /* bit mask to access Preamble Extension = 4096 */
    /*offset 22 */
    #define TX_FCTRL_TXBOFFS_SHFT   (22)            /* Shift to access transmit buffer index offset */
    #define TX_FCTRL_TXBOFFS_MASK   0xFFC00000UL    /* bit mask to access Transmit buffer index offset 10-bit field */
    /*offset 32 */
    #define TX_FCTRL_IFSDELAY_MASK  0xFF00000000ULL /* bit mask to access Inter-Frame Spacing field */

    /****************************************************************************//**
     * @brief Bit definitions for register RX_FINFO
    **/
    #define RX_FINFO_ID             0x10            /* RX Frame Information (in double buffer set) */
    #define RX_FINFO_OFFSET         0x00
    #define RX_FINFO_LEN            (4)
    /*mask and shift */
    #define RX_FINFO_MASK_32        0xFFFFFBFFUL    /* System event Status Register access mask (all unused fields should always be writen as zero) */
    #define RX_FINFO_RXFLEN_MASK    0x0000007FUL    /* Receive Frame Length (0 to 127) */
    #define RX_FINFO_RXFLE_MASK     0x00000380UL    /* Receive Frame Length Extension (0 to 7)<<7 */
    #define RX_FINFO_RXFL_MASK_1023 0x000003FFUL    /* Receive Frame Length Extension (0 to 1023) */

    #define RX_FINFO_RXNSPL_MASK    0x00001800UL    /* Receive Non-Standard Preamble Length */
    #define RX_FINFO_RXPSR_MASK     0x000C0000UL    /* RX Preamble Repetition. 00 = 16 symbols, 01 = 64 symbols, 10 = 1024 symbols, 11 = 4096 symbols */

    #define RX_FINFO_RXPEL_MASK     0x000C1800UL    /* Receive Preamble Length = RXPSR+RXNSPL */
    #define RX_FINFO_RXPEL_64       0x00040000UL    /* Receive Preamble length = 64 */
    #define RX_FINFO_RXPEL_128      0x00040800UL    /* Receive Preamble length = 128 */
    #define RX_FINFO_RXPEL_256      0x00041000UL    /* Receive Preamble length = 256 */
    #define RX_FINFO_RXPEL_512      0x00041800UL    /* Receive Preamble length = 512 */
    #define RX_FINFO_RXPEL_1024     0x00080000UL    /* Receive Preamble length = 1024 */
    #define RX_FINFO_RXPEL_1536     0x00080800UL    /* Receive Preamble length = 1536 */
    #define RX_FINFO_RXPEL_2048     0x00081000UL    /* Receive Preamble length = 2048 */
    #define RX_FINFO_RXPEL_4096     0x000C0000UL    /* Receive Preamble length = 4096 */

    #define RX_FINFO_RXBR_MASK      0x00006000UL    /* Receive Bit Rate report. This field reports the received bit rate */
    #define RX_FINFO_RXBR_110k      0x00000000UL    /* Received bit rate = 110 kbps */
    #define RX_FINFO_RXBR_850k      0x00002000UL    /* Received bit rate = 850 kbps */
    #define RX_FINFO_RXBR_6M        0x00004000UL    /* Received bit rate = 6.8 Mbps */
    #define RX_FINFO_RXBR_SHIFT     (13)

    #define RX_FINFO_RNG            0x00008000UL    /* Receiver Ranging. Ranging bit in the received PHY header identifying the frame as a ranging packet. */
    #define RX_FINFO_RNG_SHIFT      (15)

    #define RX_FINFO_RXPRF_MASK     0x00030000UL    /* RX Pulse Repetition Rate report */
    #define RX_FINFO_RXPRF_16M      0x00010000UL    /* PRF being employed in the receiver = 16M */
    #define RX_FINFO_RXPRF_64M      0x00020000UL    /* PRF being employed in the receiver = 64M */
    #define RX_FINFO_RXPRF_SHIFT    (16)

    #define RX_FINFO_RXPACC_MASK    0xFFF00000UL    /* Preamble Accumulation Count */
    #define RX_FINFO_RXPACC_SHIFT   (20)

    /****************************************************************************//**
     * @brief Bit definitions for register SYS_CTRL
    **/
    #define SYS_CTRL_ID             0x0D            /* System Control Register */
    #define SYS_CTRL_OFFSET         0x00
    #define SYS_CTRL_LEN            (4)
    /*masks */
    #define SYS_CTRL_MASK_32        0x010003CFUL    /* System Control Register access mask (all unused fields should always be writen as zero) */
    /*offset 0 */
    #define SYS_CTRL_SFCST          0x00000001UL    /* Suppress Auto-FCS Transmission (on this frame) */
    #define SYS_CTRL_TXSTRT         0x00000002UL    /* Start Transmitting Now */
    #define SYS_CTRL_TXDLYS         0x00000004UL    /* Transmitter Delayed Sending (initiates sending when SYS_TIME == TXD_TIME */
    #define SYS_CTRL_CANSFCS        0x00000008UL    /* Cancel Suppression of auto-FCS transmission (on the current frame) */
    #define SYS_CTRL_TRXOFF         0x00000040UL    /* Transceiver Off. Force Transciever OFF abort TX or RX immediately */
    #define SYS_CTRL_WAIT4RESP      0x00000080UL    /* Wait for Response */
    /*offset 8 */
    #define SYS_CTRL_RXENAB         0x00000100UL    /* Enable Receiver Now */
    #define SYS_CTRL_RXDLYE         0x00000200UL    /* Receiver Delayed Enable (Enables Receiver when SY_TIME[0x??] == RXD_TIME[0x??] CHECK comment*/
    /*offset 16 */
    /*offset 24 */
    #define SYS_CTRL_HSRBTOGGLE     0x01000000UL    /* Host side receiver buffer pointer toggle - toggles 0/1 host side data set pointer */
    #define SYS_CTRL_HRBT           (SYS_CTRL_HSRBTOGGLE)
    #define SYS_CTRL_HRBT_OFFSET    (3)

    // Defines for enable_clocks function
    #define FORCE_SYS_XTI  0
    #define ENABLE_ALL_SEQ 1
    #define FORCE_SYS_PLL  2
    #define READ_ACC_ON    7
    #define READ_ACC_OFF   8
    #define FORCE_OTP_ON   11
    #define FORCE_OTP_OFF  12
    #define FORCE_TX_PLL   13
    #define FORCE_LDE      14

    /* Function Definitions */
    #define dwt_write32bitreg(x,y)  dwt_write32bitoffsetreg(x,0,y)
    #define dwt_read32bitreg(x)     dwt_read32bitoffsetreg(x,0)

    /*!
     * Static data for DW1000 DecaWave Transceiver control
     */

    // TX/RX call-back data
    typedef struct
    {
        uint32 status;      //initial value of register as ISR is entered
        uint16 datalength;  //length of frame
        uint8  fctrl[2];    //frame control bytes
        uint8  rx_flags;    //RX frame flags, see above
    } dwt_cb_data_t;

    // Call-back type for all events
    typedef void (*dwt_cb_t)(const dwt_cb_data_t *);

    typedef int decaIrqStatus_t ; // Type for remembering IRQ status

    // -------------------------------------------------------------------------------------------------------------------
    // Structure to hold device data
    typedef struct
    {
        uint32      partID ;            // IC Part ID - read during initialisation
        uint32      lotID ;             // IC Lot ID - read during initialisation
        uint8       longFrames ;        // Flag in non-standard long frame mode
        uint8       otprev ;            // OTP revision number (read during initialisation)
        uint32      txFCTRL ;           // Keep TX_FCTRL register config
        uint8       init_xtrim;         // initial XTAL trim value read from OTP (or defaulted to mid-range if OTP not programmed)
        uint8       dblbuffon;          // Double RX buffer mode flag
        uint32      sysCFGreg ;         // Local copy of system config register
        uint16      sleep_mode;         // Used for automatic reloading of LDO tune and microcode at wake-up
        uint8       wait4resp ;         // wait4response was set with last TX start command
        dwt_cb_data_t cbData;           // Callback data structure
        dwt_cb_t    cbTxDone;           // Callback for TX confirmation event
        dwt_cb_t    cbRxOk;             // Callback for RX good frame event
        dwt_cb_t    cbRxTo;             // Callback for RX timeout events
        dwt_cb_t    cbRxErr;            // Callback for RX error events
    } dwt_local_data_t ;

    static dwt_local_data_t dw1000local[DWT_NUM_DW_DEV];
    // static dwt_local_data_t dw1000local[DWT_NUM_DW_DEV] ; // Static local device data, can be an array to support multiple DW1000 testing applications/platforms
    //static dwt_local_data_t *pdw1000local = dw1000local ; // Static local data structure pointer
    static dwt_local_data_t *pdw1000local; // Static local data structure pointer

    //static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */

    /*
     * deca_params_init.c
     */

    #define NUM_BR 3
    #define NUM_PRF 2
    #define NUM_PACS 4
    #define NUM_BW 2            //2 bandwidths are supported
    #define NUM_SFD 2           //supported number of SFDs - standard = 0, non-standard = 1
    #define NUM_CH 6            //supported channels are 1, 2, 3, 4, 5, 7
    #define NUM_CH_SUPPORTED 8  //supported channels are '0', 1, 2, 3, 4, 5, '6', 7
    #define PCODES 25           //supported preamble codes

    typedef struct {
        uint32 lo32;
        uint16 target[NUM_PRF];
    } agc_cfg_struct ;

    #define PEAK_MULTPLIER  (0x60) //3 -> (0x3 * 32) & 0x00E0
    #define N_STD_FACTOR    (13)
    #define LDE_PARAM1      (PEAK_MULTPLIER | N_STD_FACTOR)
    #define LDE_PARAM3_16 (0x1607)
    #define LDE_PARAM3_64 (0x0607)

    const uint16 lde_replicaCoeff[PCODES] =
    {
        0, // No preamble code 0
        LDE_REPC_PCODE_1,
        LDE_REPC_PCODE_2,
        LDE_REPC_PCODE_3,
        LDE_REPC_PCODE_4,
        LDE_REPC_PCODE_5,
        LDE_REPC_PCODE_6,
        LDE_REPC_PCODE_7,
        LDE_REPC_PCODE_8,
        LDE_REPC_PCODE_9,
        LDE_REPC_PCODE_10,
        LDE_REPC_PCODE_11,
        LDE_REPC_PCODE_12,
        LDE_REPC_PCODE_13,
        LDE_REPC_PCODE_14,
        LDE_REPC_PCODE_15,
        LDE_REPC_PCODE_16,
        LDE_REPC_PCODE_17,
        LDE_REPC_PCODE_18,
        LDE_REPC_PCODE_19,
        LDE_REPC_PCODE_20,
        LDE_REPC_PCODE_21,
        LDE_REPC_PCODE_22,
        LDE_REPC_PCODE_23,
        LDE_REPC_PCODE_24
    };

    /****************************************************************************//**
     * @brief Bit definitions for register SYS_STATUS
    **/
    #define SYS_STATUS_ID           0x0F            /* System event Status Register */
    #define SYS_STATUS_OFFSET       0x00
    #define SYS_STATUS_LEN          (5)             /* Note 40 bit register */
    /*masks */
    #define SYS_STATUS_MASK_32      0xFFF7FFFFUL    /* System event Status Register access mask (all unused fields should always be writen as zero) */
    /*offset 0 */
    #define SYS_STATUS_IRQS         0x00000001UL    /* Interrupt Request Status READ ONLY */
    #define SYS_STATUS_CPLOCK       0x00000002UL    /* Clock PLL Lock */
    #define SYS_STATUS_ESYNCR       0x00000004UL    /* External Sync Clock Reset */
    #define SYS_STATUS_AAT          0x00000008UL    /* Automatic Acknowledge Trigger */
    #define SYS_STATUS_TXFRB        0x00000010UL    /* Transmit Frame Begins */
    #define SYS_STATUS_TXPRS        0x00000020UL    /* Transmit Preamble Sent */
    #define SYS_STATUS_TXPHS        0x00000040UL    /* Transmit PHY Header Sent */
    #define SYS_STATUS_TXFRS        0x00000080UL    /* Transmit Frame Sent: This is set when the transmitter has completed the sending of a frame */
    /*offset 8 */
    #define SYS_STATUS_RXPRD        0x00000100UL    /* Receiver Preamble Detected status */
    #define SYS_STATUS_RXSFDD       0x00000200UL    /* Receiver Start Frame Delimiter Detected. */
    #define SYS_STATUS_LDEDONE      0x00000400UL    /* LDE processing done */
    #define SYS_STATUS_RXPHD        0x00000800UL    /* Receiver PHY Header Detect */
    #define SYS_STATUS_RXPHE        0x00001000UL    /* Receiver PHY Header Error */
    #define SYS_STATUS_RXDFR        0x00002000UL    /* Receiver Data Frame Ready */
    #define SYS_STATUS_RXFCG        0x00004000UL    /* Receiver FCS Good */
    #define SYS_STATUS_RXFCE        0x00008000UL    /* Receiver FCS Error */
    /*offset 16 */
    #define SYS_STATUS_RXRFSL       0x00010000UL    /* Receiver Reed Solomon Frame Sync Loss */
    #define SYS_STATUS_RXRFTO       0x00020000UL    /* Receive Frame Wait Timeout */
    #define SYS_STATUS_LDEERR       0x00040000UL    /* Leading edge detection processing error */
    #define SYS_STATUS_reserved     0x00080000UL    /* bit19 reserved */
    #define SYS_STATUS_RXOVRR       0x00100000UL    /* Receiver Overrun */
    #define SYS_STATUS_RXPTO        0x00200000UL    /* Preamble detection timeout */
    #define SYS_STATUS_GPIOIRQ      0x00400000UL    /* GPIO interrupt */
    #define SYS_STATUS_SLP2INIT     0x00800000UL    /* SLEEP to INIT */
    /*offset 24 */
    #define SYS_STATUS_RFPLL_LL     0x01000000UL    /* RF PLL Losing Lock */
    #define SYS_STATUS_CLKPLL_LL    0x02000000UL    /* Clock PLL Losing Lock */
    #define SYS_STATUS_RXSFDTO      0x04000000UL    /* Receive SFD timeout */
    #define SYS_STATUS_HPDWARN      0x08000000UL    /* Half Period Delay Warning */
    #define SYS_STATUS_TXBERR       0x10000000UL    /* Transmit Buffer Error */
    #define SYS_STATUS_AFFREJ       0x20000000UL    /* Automatic Frame Filtering rejection */
    #define SYS_STATUS_HSRBP        0x40000000UL    /* Host Side Receive Buffer Pointer */
    #define SYS_STATUS_ICRBP        0x80000000UL    /* IC side Receive Buffer Pointer READ ONLY */
    /*offset 32 */
    #define SYS_STATUS_RXRSCS       0x0100000000ULL /* Receiver Reed-Solomon Correction Status */
    #define SYS_STATUS_RXPREJ       0x0200000000ULL /* Receiver Preamble Rejection */
    #define SYS_STATUS_TXPUTE       0x0400000000ULL /* Transmit power up time error */

    #define SYS_STATUS_TXERR        (0x0408)        /* These bits are the 16 high bits of status register TXPUTE and HPDWARN flags */

    /* All RX events after a correct packet reception mask. */
    #define SYS_STATUS_ALL_RX_GOOD (SYS_STATUS_RXDFR | SYS_STATUS_RXFCG | SYS_STATUS_RXPRD | \
                                    SYS_STATUS_RXSFDD | SYS_STATUS_RXPHD | SYS_STATUS_LDEDONE)

    /* All double buffer events mask. */
    #define SYS_STATUS_ALL_DBLBUFF (SYS_STATUS_RXDFR | SYS_STATUS_RXFCG)

    /* All RX errors mask. */
    #define SYS_STATUS_ALL_RX_ERR  (SYS_STATUS_RXPHE | SYS_STATUS_RXFCE | SYS_STATUS_RXRFSL | SYS_STATUS_RXSFDTO \
                                    | SYS_STATUS_AFFREJ | SYS_STATUS_LDEERR)

    /* User defined RX timeouts (frame wait timeout and preamble detect timeout) mask. */
    #define SYS_STATUS_ALL_RX_TO   (SYS_STATUS_RXRFTO | SYS_STATUS_RXPTO)

    /* All TX events mask. */
    #define SYS_STATUS_ALL_TX      (SYS_STATUS_AAT | SYS_STATUS_TXFRB | SYS_STATUS_TXPRS | \
                                    SYS_STATUS_TXPHS | SYS_STATUS_TXFRS )

    /****************************************************************************//**
     * @brief Bit definitions for register  SYS_MASK
    **/
    #define SYS_MASK_ID             0x0E            /* System Event Mask Register */
    #define SYS_MASK_LEN            (4)
    /*masks */
    #define SYS_MASK_MASK_32        0x3FF7FFFEUL    /* System Event Mask Register access mask (all unused fields should always be writen as zero) */
    /*offset 0 */
    #define SYS_MASK_MCPLOCK        0x00000002UL    /* Mask clock PLL lock event    */
    #define SYS_MASK_MESYNCR        0x00000004UL    /* Mask clock PLL lock event    */
    #define SYS_MASK_MAAT           0x00000008UL    /* Mask automatic acknowledge trigger event */
    #define SYS_MASK_MTXFRB         0x00000010UL    /* Mask transmit frame begins event */
    #define SYS_MASK_MTXPRS         0x00000020UL    /* Mask transmit preamble sent event    */
    #define SYS_MASK_MTXPHS         0x00000040UL    /* Mask transmit PHY Header Sent event  */
    #define SYS_MASK_MTXFRS         0x00000080UL    /* Mask transmit frame sent event   */
    /*offset 8 */
    #define SYS_MASK_MRXPRD         0x00000100UL    /* Mask receiver preamble detected event    */
    #define SYS_MASK_MRXSFDD        0x00000200UL    /* Mask receiver SFD detected event */
    #define SYS_MASK_MLDEDONE       0x00000400UL    /* Mask LDE processing done event   */
    #define SYS_MASK_MRXPHD         0x00000800UL    /* Mask receiver PHY header detect event    */
    #define SYS_MASK_MRXPHE         0x00001000UL    /* Mask receiver PHY header error event */
    #define SYS_MASK_MRXDFR         0x00002000UL    /* Mask receiver data frame ready event */
    #define SYS_MASK_MRXFCG         0x00004000UL    /* Mask receiver FCS good event */
    #define SYS_MASK_MRXFCE         0x00008000UL    /* Mask receiver FCS error event    */
    /*offset 16 */
    #define SYS_MASK_MRXRFSL        0x00010000UL    /* Mask receiver Reed Solomon Frame Sync Loss event */
    #define SYS_MASK_MRXRFTO        0x00020000UL    /* Mask Receive Frame Wait Timeout event    */
    #define SYS_MASK_MLDEERR        0x00040000UL    /* Mask leading edge detection processing error event   */
    #define SYS_MASK_MRXOVRR        0x00100000UL    /* Mask Receiver Overrun event  */
    #define SYS_MASK_MRXPTO         0x00200000UL    /* Mask Preamble detection timeout event    */
    #define SYS_MASK_MGPIOIRQ       0x00400000UL    /* Mask GPIO interrupt event    */
    #define SYS_MASK_MSLP2INIT      0x00800000UL    /* Mask SLEEP to INIT event */
    /*offset 24*/
    #define SYS_MASK_MRFPLLLL       0x01000000UL    /* Mask RF PLL Loosing Lock warning event   */
    #define SYS_MASK_MCPLLLL        0x02000000UL    /* Mask Clock PLL Loosing Lock warning event    */
    #define SYS_MASK_MRXSFDTO       0x04000000UL    /* Mask Receive SFD timeout event   */
    #define SYS_MASK_MHPDWARN       0x08000000UL    /* Mask Half Period Delay Warning event */
    #define SYS_MASK_MTXBERR        0x10000000UL    /* Mask Transmit Buffer Error event */
    #define SYS_MASK_MAFFREJ        0x20000000UL    /* Mask Automatic Frame Filtering rejection event   */

    // map the channel number to the index in the configuration arrays below
    // 0th element is chan 1, 1st is chan 2, 2nd is chan 3, 3rd is chan 4, 4th is chan 5, 5th is chan 7
    const uint8 chan_idx[NUM_CH_SUPPORTED] = {0, 0, 1, 2, 3, 4, 0, 5};

    //-----------------------------------------
    // map the channel number to the index in the configuration arrays below
    // 0th element is chan 1, 1st is chan 2, 2nd is chan 3, 3rd is chan 4, 4th is chan 5, 5th is chan 7
    // const uint8 chan_idx[NUM_CH_SUPPORTED] = {0, 0, 1, 2, 3, 4, 0, 5};

    //-----------------------------------------
    const uint32 tx_config[NUM_CH] =
    {
        RF_TXCTRL_CH1,
        RF_TXCTRL_CH2,
        RF_TXCTRL_CH3,
        RF_TXCTRL_CH4,
        RF_TXCTRL_CH5,
        RF_TXCTRL_CH7,
    };

    //Frequency Synthesiser - PLL configuration
    const uint32 fs_pll_cfg[NUM_CH] =
    {
        FS_PLLCFG_CH1,
        FS_PLLCFG_CH2,
        FS_PLLCFG_CH3,
        FS_PLLCFG_CH4,
        FS_PLLCFG_CH5,
        FS_PLLCFG_CH7
    };

    //Frequency Synthesiser - PLL tuning
    const uint8 fs_pll_tune[NUM_CH] =
    {
        FS_PLLTUNE_CH1,
        FS_PLLTUNE_CH2,
        FS_PLLTUNE_CH3,
        FS_PLLTUNE_CH4,
        FS_PLLTUNE_CH5,
        FS_PLLTUNE_CH7
    };

    //bandwidth configuration
    const uint8 rx_config[NUM_BW] =
    {
        RF_RXCTRLH_NBW,
        RF_RXCTRLH_WBW
    };

    const agc_cfg_struct agc_config =
    {
        AGC_TUNE2_VAL,
        { AGC_TUNE1_16M , AGC_TUNE1_64M }  //adc target
    };

    //DW non-standard SFD length for 110k, 850k and 6.81M
    const uint8 dwnsSFDlen[NUM_BR] =
    {
        DW_NS_SFD_LEN_110K,
        DW_NS_SFD_LEN_850K,
        DW_NS_SFD_LEN_6M8
    };

    // SFD Threshold
    const uint16 sftsh[NUM_BR][NUM_SFD] =
    {
        {
            DRX_TUNE0b_110K_STD,
            DRX_TUNE0b_110K_NSTD
        },
        {
            DRX_TUNE0b_850K_STD,
            DRX_TUNE0b_850K_NSTD
        },
        {
            DRX_TUNE0b_6M8_STD,
            DRX_TUNE0b_6M8_NSTD
        }
    };

    const uint16 dtune1[NUM_PRF] =
    {
        DRX_TUNE1a_PRF16,
        DRX_TUNE1a_PRF64
    };

    const uint32 digital_bb_config[NUM_PRF][NUM_PACS] =
    {
        {
            DRX_TUNE2_PRF16_PAC8,
            DRX_TUNE2_PRF16_PAC16,
            DRX_TUNE2_PRF16_PAC32,
            DRX_TUNE2_PRF16_PAC64
        },
        {
            DRX_TUNE2_PRF64_PAC8,
            DRX_TUNE2_PRF64_PAC16,
            DRX_TUNE2_PRF64_PAC32,
            DRX_TUNE2_PRF64_PAC64
        }
    };

    /*! ------------------------------------------------------------------------------------------------------------------
     * Structure typedef: dwt_config_t
     *
     * Structure for setting device configuration via dwt_configure() function
     *
     */
    typedef struct
    {
        uint8 chan ;           //!< channel number {1, 2, 3, 4, 5, 7 }
        uint8 prf ;            //!< Pulse Repetition Frequency {DWT_PRF_16M or DWT_PRF_64M}
        uint8 txPreambLength ; //!< DWT_PLEN_64..DWT_PLEN_4096
        uint8 rxPAC ;          //!< Acquisition Chunk Size (Relates to RX preamble length)
        uint8 txCode ;         //!< TX preamble code
        uint8 rxCode ;         //!< RX preamble code
        uint8 nsSFD ;          //!< Boolean should we use non-standard SFD for better performance
        uint8 dataRate ;       //!< Data Rate {DWT_BR_110K, DWT_BR_850K or DWT_BR_6M8}
        uint8 phrMode ;        //!< PHR mode {0x0 - standard DWT_PHRMODE_STD, 0x3 - extended frames DWT_PHRMODE_EXT}
        uint16 sfdTO ;         //!< SFD timeout value (in symbols)
    } dwt_config_t ;

    static dwt_config_t config;

    // static dwt_config_t config = {
    //        5,                /* Channel number. */
    //        DWT_PRF_64M,      /* Pulse repetition frequency. */
    //        DWT_PLEN_128,     /* Preamble length. Used in TX only. */
    //        DWT_PAC8,         /* Preamble acquisition chunk size. Used in RX only. */
    //        10,               /* TX preamble code. Used in TX only. */
    //        10,               /* RX preamble code. Used in RX only. */
    //        0,                /* 0 to use standard SFD, 1 to use non-standard SFD. */
    //        DWT_BR_6M8,       /* Data rate. */
    //        DWT_PHRMODE_STD,  /* PHY header mode. */
    //        (129 + 8 - 8)     /* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
    // };

    // Buffer to store received response message.
    // TODO figure out how big the receive buffer should be
    #define RX_BUF_LEN 128
    // static uint8 rx_buffer;
    static uint8 rx_buffer[RX_BUF_LEN];

    /* Frame sequence number, incremented after each transmission. */
    // static uint8 frame_seq_nb = 0;
    static uint8 frame_seq_nb;

    /* Hold copy of status register state here for reference so that it can be examined at a debug breakpoint. */
    // static uint32 status_reg = 0;
    static uint32 status_reg;

    /* Frames used in the ranging process. See NOTE 2,3 below. */
    // static uint8 expected_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 0, MY_ID, 0, 1 - MY_ID, 0xE0, 0, 0};
    // static uint8 outgoing_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 0, 1 - MY_ID, 0, MY_ID, 0xE0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    // static uint8 final_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 0, 1 - MY_ID, 0, MY_ID, 0xE0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static uint8 expected_msg[];
    static uint8 outgoing_msg[];
    static uint8 final_msg[];

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

    //! Command reads the DW1000 device registers using the header provided and stores the read data in the input buffer
    int readfromspi(
         uint16 headerLength,
         const uint8 *headerBuffer,
         uint32 readlength,
         uint8 *readBuffer
     );

    //! Command writes to the DW1000 device registers using the header and data provided
    int writetospi(
            uint16 headerLength,
            const uint8 *headerBuffer,
            uint32 bodylength,
            uint8 *bodyBuffer
     );

    //! Command used to read from the DW1000 device registers
    void dwt_readfromdevice(
         uint16  recordNumber,
         uint16  index,
         uint32  length,
         uint8   *buffer
     );

    //! Command used to read from the DW1000 device registers
    void dwt_writetodevice(
         uint16  recordNumber,
         uint16  index,
         uint32  length,
         uint8   *buffer
     );

    //! Command used to read 8-bit values from the DW1000 device registers
    uint8 dwt_read8bitoffsetreg(
         int regFileID,
         int regOffset
    );

    //! Command used to read 16-bit values from the DW1000 device registers
    uint16 dwt_read16bitoffsetreg(
         int regFileID,
         int regOffset
    );

    //! Command used to read 32-bit values from the DW1000 device registers
    uint32 dwt_read32bitoffsetreg(
         int regFileID,
         int regOffset
     );

    //! Command used to write 8-bit values from the DW1000 device registers
    void dwt_write8bitoffsetreg(
         int regFileID,
         int regOffset,
         uint8 regval
     );

    //! Command used to write 16-bit values from the DW1000 device registers
    void dwt_write16bitoffsetreg(
         int regFileID,
         int regOffset,
         uint16 regval
     );

    //! Command used to write 32-bit values from the DW1000 device registers
    void dwt_write32bitoffsetreg(
         int regFileID,
         int regOffset,
         uint32 regval
    );

    //! Command to return the read device type and revision information of the DW1000 device (MP part is 0xDECA0130)
    uint32 dwt_readdevid(
         void
     );

    //! Command to enables/disables clocks to particular digital blocks/system
    void _dwt_enableclocks(
        int clocks
    );

    //! Command to disable the TX blocks sequencing. It disables PMSC control of RF blocks, system clock is also set to XTAL
    void _dwt_disablesequencing(
         void
    );

    //! Command to uploads always on (AON) data array and configuration
    void _dwt_aonarrayupload(
         void
    );

    //! Command to tell the DW1000 to sleep for the specified period of time
    void deca_sleep(
         unsigned int time_ms
    );

    //! Command to resets the DW1000
    void dwt_softreset(
         void
     );

    //! Command to read the OTP memory and ensures that MR,MRa,MRb are reset to 0.
    uint32 _dwt_otpread(
         uint32 address
    );

    //! Command to adjust the crystal frequency
     void dwt_setxtaltrim(
         uint8 value
     );

    //! Command to loads ucode from OTP MEMORY or ROM
     void _dwt_loaducodefromrom(
         void
     );

    //! Command to initiate communications with the DW1000 transceiver
     int dwt_initialise(
         uint16 config
     );

     //! Command to reset the digital part of DW1000
     void reset_DW1000(
         void
     );

     //! Command to configure LDE algorithm parameters
     void _dwt_configlde(
         int prfIndex
     );

     //! Command to configure the DW1000
     void dwt_configure(
         dwt_config_t *config
     );

     //! Command to write the antenna delay (in time units) to RX registers
     void dwt_setrxantennadelay(
         uint16 rxDelay
     );

     //! Command to write the antenna delay (in time units) to RX registers
     void dwt_settxantennadelay(
         uint16 txDelay
     );

     //! Command to initiate and configure the DW1000
     int txrx_configure(
         void
     );

     //! Command to read the system time and write it to the buffer provided.
     void dwt_readsystime(
          uint8 * timestamp
     );

     //! Command to gets the sys time-stamp in a 64-bit variable
     uint64 get_sys_timestamp_u64(
          void
     );

     //! Command to configure the delayed transmit time or the delayed RX on time
     void dwt_setdelayedtrxtime(
          uint32 starttime
     );

     //! Command to fill in a given timestamp field in the response message.
     void resp_msg_set_ts(
          uint8 *ts_field,
          const uint64 ts
     );

     //! Command to writes the supplied TX data into the DW1000's TX buffer.
     int dwt_writetxdata(
          uint16 txFrameLength,
          uint8 *txFrameBytes,
          uint16 txBufferOffset
     );

     //! Command to configure the TX frame control register before the transmission of a frame
     void dwt_writetxfctrl(
          uint16 txFrameLength,
          uint16 txBufferOffset,
          int ranging
     );

     //! Command to configure the TX frame control register before the transmission of a frame
     int dwt_starttx(
          uint8 mode
     );

     //! Command to reset the receiver of the DW1000
     void dwt_rxreset(
          void
     );

     //! Command to modify the send value, create and send the message
     int make_and_send_response(
          uint64 rx_ts_u64,
          uint32 *tx_ts,
          uint8 *mess_function,
          uint8 *msg_arr,
          size_t arr_size
     );

     //! Command to synchronizes RX buffer pointers
     void dwt_syncrxbufptrs(
          void
     );

     //! Currently does nothing
     decaIrqStatus_t decamutexon(
          void
     );

     //! Currently does nothing
     void decamutexoff(
          decaIrqStatus_t s
     );

     //! Command to turn off the transceiver
     void dwt_forcetrxoff(
          void
     );

     //! Command to turn on the receiver
     int dwt_rxenable(
          int mode
     );

     //! Command to read data from the RX buffer, from an offset location given by offset parameter
     void dwt_readrxdata(
          uint8 *buffer,
          uint16 length,
          uint16 rxBufferOffset
     );

     //! Command to read a given timestamp value from the response message
     void resp_msg_get_ts(
          uint8 *ts_field,
          uint32 *ts
     );

     //! Command to receive the message sent and check the message
     int recv_and_parse(
          uint32 *their_rx_ts,
          uint32 *their_tx_ts,
          uint8 *mess_function
     );

     //! Command to reads the RX timestamp (adjusted time of arrival) and stores it to the buffer provided
     void dwt_readrxtimestamp(
          uint8 * timestamp
     );

     //! Command to get the RX time-stamp in a 64-bit variable
     uint64 get_rx_timestamp_u64(
          void
     );

     //! Command to modify the send value passed in and creates a message to send to the responder
     int ns_start_chain(
          uint32 *tx_ts,
          uint8 *mess_function,
          uint8 *msg_arr,
          size_t arr_size
     );

     //! Command to modify the send and recv values passed in, parse through the previous message, and create the next message to be sent
     int ns_continue_chain(
          uint32 *their_rx_ts,
          uint32 *their_tx_ts,
          uint32 *rx_ts,
          uint32 *tx_ts,
          uint8 *mess_function,
          uint8 *msg_arr,
          size_t arr_size
     );

     //! Command to modifies the send and recv values passed in, parses through the previous message, and modifies the final received message
     int ns_end_chain(
          uint32 *their_rx_ts,
          uint32 *their_tx_ts,
          uint32 *rx_ts,
          uint8 *mess_function
     );

     //! Command to modifies the send and recv values passed in, parses through the previous message, and modifies the final received message
     int calc_distance(
          uint32 leg1_send_ts,
          uint32 leg1_recv_ts,
          uint32 leg2_send_ts,
          uint32 leg2_recv_ts,
          uint32 leg3_send_ts,
          uint32 leg3_recv_ts
     );

     //! Command to write leg values into a buffer
     void write_leg_values(
         char *char_buffer,
         uint32 leg_send_ts,
         uint32 leg_recv_ts
     );

     //! Command to retrieve leg values for initiator and stores the leg values into a buffer
     int start_ds_twr(
         char *char_buffer
     );

     //! Command to read the data from the Accumulator buffer, from an offset location give by offset parameter
     void dwt_readaccdata(
         uint8 *buffer,
         uint16 len,
         uint16 accOffset
     );

     //! Command to read CIR data from DW1000 and store them to a buffer
     void Get_CIRdata(
          char *char_buffer
     );

     //! Implementation for Get_Time command handler
     //! Command to get the time recordings from the UWB module
     void Get_Time_cmdHandler(
         const FwOpcodeType opCode, /*!< The opcode*/
         const U32 cmdSeq /*!< The command sequence number*/
     );

     //! Implementation for Get_Data command handler
     //! Command to get the full time and data recordings from the UWB module
     void Get_Data_cmdHandler(
         const FwOpcodeType opCode, /*!< The opcode*/
         const U32 cmdSeq /*!< The command sequence number*/
     );

     U32 m_bytesSent;
     U32 m_callbackId;

    };

} // end namespace CubeRover

#endif
