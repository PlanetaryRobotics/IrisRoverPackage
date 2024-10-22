// ======================================================================
// \title  CameraComponentImpl.hpp
// \author Connor Colombo, Raewyn Duvall
// \brief  hpp file for Camera component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Camera_HPP
#define Camera_HPP

#include "CubeRover/Camera/CameraComponentAc.hpp"
#include "CubeRover/WatchDogInterface/WatchDogInterface.hpp" // need the `reset_values_possible` enum definitions

#include "S25fl512l.hpp"

// --- SYSTEM IMAGE PARAMS ---
#define IMAGE_WIDTH 2592
// Number of pages used by each image line:
#define IMAGE_PAGE_WIDTH 6
#define IMAGE_HEIGHT 1944

// --- DUMMY IMAGE PARAMS ---

#define VIA_FLASH // Read & Write dummy image w/ FPGA Flash

#define DUMMY_IMG_GRID_n 5 // Dummy image is grid of NxN squares
#define DUMMY_IMAGE_WIDTH IMAGE_WIDTH
#define DUMMY_IMAGE_HEIGHT IMAGE_HEIGHT

namespace CubeRover
{

    // Metadata about how an image line is formatted:
    struct ImageLineFormattingMetadata
    {
        // Has the line been binned:
        uint8_t binned; // only 0xFF or 0x00 are valid
        // Has the line been compressed:
        uint8_t compressed; // only 0xFF or 0x00 are valid
    } __attribute__((packed));

    struct FormattedImageLine
    {
        ImageLineFormattingMetadata formatting;
        uint8_t buffer[PAGE_SIZE * IMAGE_PAGE_WIDTH];
    } __attribute__((packed));

    // Saved settings for capturing a deployment image:
    struct DeploymentImageSettings
    {
        // Status Data:
        bool waitingForTrigger; // currently waiting for a trigger.
        U32 startTimeMs;        // when we started waiting for a trigger.
        // Capture Settings:
        U8 camera_num;
        U16 callback_id;
        bool eraseFirst;
        // Timeout Settings:
        U32 timeoutMs;
        bool triggerOnTimeout = false; // safe default
        // Trigger Settings:
        bool conditionModeAll = true; // safe default
        // IMU XAcc Settings:
        bool imuXAcc_on;
        bool imuXAcc_triggerMode;
        I16 imuXAcc_min;
        I16 imuXAcc_max;
        // IMU YAcc Settings:
        bool imuYAcc_on;
        bool imuYAcc_triggerMode;
        I16 imuYAcc_min;
        I16 imuYAcc_max;
        // IMU ZAcc Settings:
        bool imuZAcc_on;
        bool imuZAcc_triggerMode;
        I16 imuZAcc_min;
        I16 imuZAcc_max;
        // WDI 28V Settings:
        bool wdi28V_on;
        bool wdi28V_triggerMode;
        I16 wdi28V_min;
        I16 wdi28V_max; //
    } __attribute__((packed));

    enum class DummyImageType
    {
        GRID = 0,
        SEQUENCE = 1,
        PRE_BAKED = 2 // pre-baked and stored in Hercules FLASH0
    };

    class CameraComponentImpl : public CameraComponentBase,  public WatchDogInterfaceComponentBaseFriend
    {

    public:
        // ----------------------------------------------------------------------
        // Construction, initialization, and destruction
        // ----------------------------------------------------------------------

        //! Construct object Camera
        //!
        CameraComponentImpl(
#if FW_OBJECT_NAMES == 1
            const char *const compName /*!< The component name*/
#else
            void
#endif
        );

        //! Initialize object Camera
        //!
        void init(
            const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
        );

        //! Destroy object Camera
        //!
        ~CameraComponentImpl(void);

        PRIVATE :

            // ----------------------------------------------------------------------
            // Handler implementations for user-defined typed input ports
            // ----------------------------------------------------------------------

            //! Handler implementation for takeImage
            //!
            void
            takePicture_handler(
                const NATIVE_INT_TYPE portNum, /*!< The port number*/
                U8 CameraNum,                  /*!< Camera Number*/
                U16 CallbackId                 /*!< Identifier downlinked with the image allowing us to map the command which triggered image capture to this image*/
            );

        PRIVATE :

            // ----------------------------------------------------------------------
            // Command handler implementations
            // ----------------------------------------------------------------------

            //! Implementation for Get_Status command handler
            //! Get camera status
            void
            Get_Status_cmdHandler(
                const FwOpcodeType opCode, /*!< The opcode*/
                const U32 cmdSeq           /*!< The command sequence number*/
            );

        //! Implementation for Take_Image command handler
        //! Take an Image
        void Take_Image_cmdHandler(
            const FwOpcodeType opCode, /*!< The opcode*/
            const U32 cmdSeq,          /*!< The command sequence number*/
            U8 camera_num,             /*!<
                                      0: Camera 0     1: Camera 1
                                  */
            U16 callback_id            /*!<
                                     Identifier which will be downlinked with the images from this command, allowing us to map which downlinked images related to which 'take photo' command
                                 */
        );

        /* Take a Full Image but only downlink a subset of the FileGroup Lines from memory (from start_line to end_line). */
        void Take_Image_Section_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            U8 camera_num,       /*!<
                                  0: Camera 0     1: Camera 1
                              */
            U16 startLine,       /*!<
                                  Line in full image memory to start downlinking. Inclusive (max is 1944). In memory space not pixel space. Indexed from 0.
                              */
            U16 endLine,         /*!<
                                    Line in full image memory to stop downlinking. Exclusive (max is 1943). In memory space not pixel space. Indexed from 0.
                                */
            U16 callback_id      /*!<
                                 Identifier which will be downlinked with the images from this command, allowing us to map which downlinked images related to which 'take photo' command
                             */
        );
        /* Take a Full Image but only downlink a subset of the FileGroup Lines from memory (from start_line to end_line). Offers advanced capture and downlink settings. */
        void Take_Image_Section_Adv_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            U8 camera_num,       /*!<
                                  0: Camera 0     1: Camera 1
                              */
            U16 startLine,       /*!<
                                  Line in full image memory to start downlinking. Inclusive (max is 1944). In memory space not pixel space. Indexed from 0.
                              */
            U16 endLine,         /*!<
                                    Line in full image memory to stop downlinking. Exclusive (max is 1943). In memory space not pixel space. Indexed from 0.
                                */
            U16 callback_id,     /*!<
                                 Identifier which will be downlinked with the images from this command, allowing us to map which downlinked images related to which 'take photo' command
                             */
            bool eraseFirst,     /*!< Whether or not to erase the flash before capturing an image. */
            uint8_t n_bin,       /*!< Binning setting: factor by which lines should be downsampled through binning. Values gt 2 will trigger an attempt at binning. This is a Bayer-preserving binning process that skips over any line or frame headers embedded in the image by the FPGA FW. Also RLEs homogeneous lines. */
            bool compressLine    /*!< Whether or not to attempt to compress lines (after binning if performed). Also RLEs homogeneous lines. */
        );
        /* Downlinks an image from flash based on the given settings. Doesn't capture an image or erase flash first, just sends whatever's already there. Camera number, callback_id, and capture time (file group id) are stored in Hercules vRAM since the last command that triggered an image capture. If Hercules has rebooted since then, this will still work but those values will be wrong.*/
        void Read_Image_Section_Adv_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            U16 startLine,       /*!<
                                  Line in full image memory to start downlinking. Inclusive (max is 1944). In memory space not pixel space. Indexed from 0.
                              */
            U16 endLine,         /*!<
                                    Line in full image memory to stop downlinking. Exclusive (max is 1943). In memory space not pixel space. Indexed from 0.
                                */
            uint8_t n_bin,       /*!< Binning setting: factor by which lines should be downsampled through binning. Values gt 2 will trigger an attempt at binning. This is a Bayer-preserving binning process that skips over any line or frame headers embedded in the image by the FPGA FW. Also RLEs homogeneous lines. */
            bool compressLine    /*!< Whether or not to attempt to compress lines (after binning if performed). Also RLEs homogeneous lines. */
        );
        /* Trigger Image Capture but don't downlink it (that can be done later with: `Read_Image_Section_Adv`). */
        void Capture_Image_Only_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq,          /*!< The command sequence number*/
            U8 camera_num,       /*!<
                                  0: Camera 0     1: Camera 1
                              */
            U16 callback_id,     /*!<
                                 Identifier which will be downlinked with the images from this command, allowing us to map which downlinked images related to which 'take photo' command
                             */
            bool eraseFirst      /*!< Whether or not to erase the flash before capturing an image. */
        );

        /* Waits to capture an image until a series of trigger condition is satisfied.

       Triggers conditions are defined as a window around a sensor value. A trigger can be defined as

       The following trigger variables are available:
       - Imu_XAcc (raw reading)
       - Imu_YAcc (raw reading)
       - Imu_ZAcc (raw reading)
       - Voltage28V (raw ADC value, as reported by WatchDogInterface)
       These values correspond to what's reported in telemetry from Hercules.

       A window is specified around each sensor variable and a trigger can be defined as the
       reading being inside or outside the window on a per-sensor basis.

       Each sensor trigger condition can be turned ON or OFF.

       `conditionModeAll` sets whether ALL or just 1 of the trigger conditions must be satisfied

       The camera will stop waiting for a trigger after `timeoutMs`.
       If `triggerOnTimeout`, an image will be captured when timeout occurs.

       This is a late add, definitely a trick, and may or may not work but we'll try it. */
       void Capture_Deployment_Image_cmdHandler(
           FwOpcodeType opCode, /*!< The opcode*/
           U32 cmdSeq, /*!< The command sequence number*/
           U8 camera_num, /*!<
                           0: Camera 0     1: Camera 1
                       */
           U16 callback_id, /*!<
                           Identifier which will be downlinked with the images from this command, allowing us to map which downlinked images related to which 'take photo' command
                       */
           bool eraseFirst, /*!<
                           Whether or not to erase the flash before capturing an image.
                       */
           U32 timeoutMs, /*!<
                           Max time to wait for a trigger condition before timing out (ms).
                       */
           bool triggerOnTimeout, /*!<
                           Whether or not to capture an image on timeout just in case.
                       */
           bool conditionModeAll, /*!<
                           True: ALL trigger conditions that are ON must be met. False: ANY of the trigger conditions that are ON (i.e. just 1) will trigger image capture.
                       */
           bool imuXAcc_on, /*!<
                           Whether or not to use the IMU X Acceleration window as a trigger.
                       */
           bool imuXAcc_triggerMode, /*!<
                           True: Trigger will occur if the sensor value is in the window. False: Trigger will occur if sensor value is outside the window.
                       */
           I16 imuXAcc_min, /*!<
                           Lower bound (inclusive) of window for IMU X Acceleration (raw reading).
                       */
           I16 imuXAcc_max, /*!<
                           Upper bound (exclusive) of window for IMU X Acceleration (raw reading).
                       */
           bool imuYAcc_on, /*!<
                           Whether or not to use the IMU Y Acceleration window as a trigger.
                       */
           bool imuYAcc_triggerMode, /*!<
                           True: Trigger will occur if the sensor value is in the window. False: Trigger will occur if sensor value is outside the window.
                       */
           I16 imuYAcc_min, /*!<
                           Lower bound (inclusive) of window for IMU Y Acceleration (raw reading).
                       */
           I16 imuYAcc_max, /*!<
                           Upper bound (exclusive) of window for IMU Y Acceleration (raw reading).
                       */
           bool imuZAcc_on, /*!<
                           Whether or not to use the IMU Z Acceleration window as a trigger.
                       */
           bool imuZAcc_triggerMode, /*!<
                           True: Trigger will occur if the sensor value is in the window. False: Trigger will occur if sensor value is outside the window.
                       */
           I16 imuZAcc_min, /*!<
                           Lower bound (inclusive) of window for IMU Z Acceleration (raw reading).
                       */
           I16 imuZAcc_max, /*!<
                           Upper bound (exclusive) of window for IMU Z Acceleration (raw reading).
                       */
           bool wdi28V_on, /*!<
                           Whether or not to use the 28V voltage reported by WatchDogInterface as a trigger;
                       */
           bool wdi28V_triggerMode, /*!<
                           True: Trigger will occur if the sensor value is in the window. False: Trigger will occur if sensor value is outside the window.
                       */
           I16 wdi28V_min, /*!<
                           Lower bound (inclusive) of window for 28V voltage reported by WatchDogInterface (raw ADC value).
                       */
           I16 wdi28V_max /*!<
                           Upper bound (exclusive) of window for 28V voltage reported by WatchDogInterface (raw ADC value).
                       */
       );

        //! Implementation for Error command handler
        //! Get camera status
        void Error_cmdHandler(
            const FwOpcodeType opCode, /*!< The opcode*/
            const U32 cmdSeq,          /*!< The command sequence number*/
            U8 action                  /*!<
                                           0x00: Clear Error     0xff: Read Error  !! Not sure how this is being used
                                       */
        );

        //! Implementation for Configure_Camera0 command handler
        //! Configure camera 0
        void Configure_Camera0_cmdHandler(
            const FwOpcodeType opCode, /*!< The opcode*/
            const U32 cmdSeq,          /*!< The command sequence number*/
            U64 config                 /*!<
                                          Packed bitfield containing camera configurations. TODO: Copy the bit fields from C&TL to here
                                      */
        );

        //! Implementation for Camera0_Crop command handler
        //! Defines the crop for camera 0
        void Camera0_Crop_cmdHandler(
            const FwOpcodeType opCode, /*!< The opcode*/
            const U32 cmdSeq,          /*!< The command sequence number*/
            U64 config                 /*!<
                                          Packed bitfield containing crop locations. TODO: Copy bitfields from C&TL to here
                                      */
        );

        //! Implementation for Configure_Camera1 command handler
        //! Configure camera 1
        void Configure_Camera1_cmdHandler(
            const FwOpcodeType opCode, /*!< The opcode*/
            const U32 cmdSeq,          /*!< The command sequence number*/
            U64 config                 /*!<
                                          Packed bitfield containing camera configurations. TODO: Copy the bit fields from C&TL to here
                                      */
        );

        //! Implementation for Camera1_Crop command handler
        //! Defines the crop for camera 1
        void Camera1_Crop_cmdHandler(
            const FwOpcodeType opCode, /*!< The opcode*/
            const U32 cmdSeq,          /*!< The command sequence number*/
            U64 config                 /*!<
                                          Packed bitfield containing crop locations. TODO: Copy bitfields from C&TL to here
                                      */
        );

        //! Implementation for Erase_Image command handler
        //! TBD
        void Erase_Flash_cmdHandler(
            const FwOpcodeType opCode, /*!< The opcode*/
            const U32 cmdSeq,          /*!< The command sequence number*/
            U8 numSectors              /*!< Number of sectors to erase. Default is 40. Only change if you have a reason. */
        );

        //! Implementation for Soft_Camera_Reset command handler
        //! Does a soft reset of the camera (and something with the FPGA)
        void Soft_Camera_Reset_cmdHandler(
            const FwOpcodeType opCode, /*!< The opcode*/
            const U32 cmdSeq,          /*!< The command sequence number*/
            U8 cameraNum               /*!<
                                        Camera Number
                                    */
        );

        //! Implementation for Image_Dump command handler
        void Image_Dump_cmdHandler(
            const FwOpcodeType opCode, /*!< The opcode*/
            const U32 cmdSeq           /*!< The command sequence number*/
        );

        //! Implementation for Downlink_Grid command handler
        void Downlink_Grid_cmdHandler(
            const FwOpcodeType opCode,
            const U32 cmdSeq, /*!< The command sequence number*/
            const bool viaFlash);

        //! Implementation for Downlink_Test_Sequence command handler
        void Downlink_Test_Sequence_cmdHandler(
            const FwOpcodeType opCode,
            const U32 cmdSeq, /*!< The command sequence number*/
            const bool viaFlash);

        //! Handler implementation for schedIn
        //!
        void schedIn_handler(
            const NATIVE_INT_TYPE portNum, /*!< The port number*/
            NATIVE_UINT_TYPE context       /*!< The call order*/
        );

        // ----------------------------------------------------------------------
        // User Methods
        // ----------------------------------------------------------------------

        uint32_t takeImage(uint8_t camera, uint16_t callbackId, bool eraseFirst = true);
        void generateDummyImage(bool viaFlash, DummyImageType type);
        void triggerImageCapture(void);
        void eraseFpgaFlash(uint8_t numSectors = 40);
        void sendImgFromFlash(uint32_t createTime, const uint32_t startLine, const uint32_t endLine, const uint8_t n_bin = 1, const bool compressLine = false);
        void downlinkImageLine(uint8_t *image, int size, uint16_t callbackId, uint32_t createTime, uint16_t lineIndex, uint16_t numLines, bool isFirstOrLastLineToDownlink);

        S25fl512l m_fpgaFlash;

        // Align image buffer to uint64_t boundary since all FPrime buffers get handled as U64 (for some reason?).
        // Can't normally align member variable, but uint64_t will be naturally aligned on an 8-byte boundary
        // thus forcing the m_headerBuffer to also be aligned at such a boundary.
        // Stolen from `WatchDogMpsm` for same purpose.
        union
        {
            FormattedImageLine m_imageLine;
            uint8_t m_formattedImageLineBuffer[sizeof(FormattedImageLine)];
            uint64_t not_used__imageLineBuffer;
        };

        U32 m_numComponentImgsReq;
        U32 m_numGroundImgsReq;
        U32 m_imagesSent;
        U32 m_bytesSent;
        uint8_t m_cameraSelect;
        uint16_t m_lastCallbackId;
        uint8_t m_lastCameraSelected; // which camera was used to capture the most recent image.
        uint32_t m_lastCaptureTime;   // time the most recent image was captured.
        // Settings for capturing a deployment image:
        DeploymentImageSettings m_deploymentImageSettings;
    };

} // end namespace CubeRover

#endif
