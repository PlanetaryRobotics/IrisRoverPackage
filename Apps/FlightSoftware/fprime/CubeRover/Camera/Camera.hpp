// ======================================================================
// \title  CameraComponentImpl.hpp
// \author Raewyn
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

#include "S25fl512l.hpp"

// --- SYSTEM IMAGE PARAMS ---
#define IMAGE_WIDTH 2592
#define IMAGE_HEIGHT 1944

// --- DUMMY IMAGE PARAMS ---

// #define DUMMY_IMG_GRID  // Dummy image is grid of NxN squares
#define DUMMY_IMG_GRID_n 5 // Dummy image is grid of NxN squares
#define VIA_FLASH          // Read & Write dummy image w/ FPGA Flash

#define DUMMY_IMAGE_WIDTH IMAGE_WIDTH
#define DUMMY_IMAGE_HEIGHT IMAGE_HEIGHT

// RAD TODO - isn't downsampling a user-defined parameter?
// #define DOWNSAMPLING        1
// #define DOWNSAMPLED_IMG_WIDTH   (IMAGE_WIDTH / DOWNSAMPLING)
// #define DOWNSAMPLE_IMG_HEIGHT   (IMAGE_HEIGHT / DOWNSAMPLING)

namespace CubeRover
{

    class CameraComponentImpl : public CameraComponentBase
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

        //! Implementation for Read_Image command handler
        //! Downlink image generated by a sequence number
        void Read_Image_cmdHandler(
            const FwOpcodeType opCode, /*!< The opcode*/
            const U32 cmdSeq,          /*!< The command sequence number*/
            U16 callbackID             /*!<
                                      DRedownlink an image generated from the specified callbac
                                  */
        );

        //! Implementation for Erase_Image command handler
        //! TBD
        void Erase_Image_cmdHandler(
            const FwOpcodeType opCode, /*!< The opcode*/
            const U32 cmdSeq           /*!< The command sequence number*/
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
        //! TBD
        void Image_Dump_cmdHandler(
            const FwOpcodeType opCode, /*!< The opcode*/
            const U32 cmdSeq           /*!< The command sequence number*/
        );

        // ----------------------------------------------------------------------
        // User Methods
        // ----------------------------------------------------------------------

        void takeImage(uint8_t camera, uint16_t callbackId);
        void generateDummyImage(void);
        void triggerImageCapture(void);
        void eraseFpgaFlash(void);
        void sendImgFromFlash(uint32_t createTime);
        void downlinkImageLine(uint8_t *image, int size, uint16_t callbackId, uint32_t createTime, uint16_t lineIndex);

        S25fl512l m_fpgaFlash;
        uint8_t m_imageLineBuffer[IMAGE_WIDTH];
        U32 m_numComponentImgsReq;
        U32 m_numGroundImgsReq;
        U32 m_imagesSent;
        U32 m_bytesSent;
        uint16_t m_lastCallbackId;
        uint8_t m_cameraSelect;
    };

} // end namespace CubeRover

#endif
