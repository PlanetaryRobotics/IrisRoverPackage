/*
 * main.hpp
 *
 *  Created on: Aug 13, 2020
 *      Author: cedric
 */

#ifndef MAIN_HPP_
#define MAIN_HPP_

typedef enum packetHeader_t{
  CMD_HEADER =  0xAA,
  TELEM_MOTOR_HEADER = 0xBB,
  TEMP_MOTOR_HEADER = 0xCC,
  TELEM_CAMERA_HEADER = 0xDD,
  ERROR_HEADER = 0xEE
}packetHeader_t;

typedef enum imageTransferStatus_t{
    BEGIN_NEW_TRANSFER= 0x01,
    CONTINUE= 0x02,
    LAST_TRANSFER= 0x03
}imageTransferStatus_t;

typedef enum trajectory_t{
    FW = 0,
    BW = 1,
    CW = 2,
    CCW = 3,
    MAX_NUMBER_TRAJECTORY = 4
}trajectory_t;

typedef enum commandReg_t{
    CAMERA_CMD = 0,
    MOTOR_CONTROL_CMD = 1,
    CONNECT_CMD = 2
}commandReg_t;

typedef enum controlJpeg_t{
    NEW_COMPRESSION = 0,
    CONTINUE_COMPRESSION = 1,
    END_COMPRESSION = 2,
    UNDEFINED = 3
}controlJpeg_t;

typedef enum motorCommand_t{
    SET_TRAJECTORY_TYPE = 0,
    SET_TARGET_POSITION = 1,
    ESTOP = 2,
    START = 3,
    SET_MAX_SPEED = 4,
    START_MOTOR_TELEMETRY = 5,
    STOP_MOTOR_TELEMETRY = 6,
    RESET = 7,
    SET_RAMP_SPEED = 8
}motorCommand_t;

typedef enum cameraCommand_t{
    TAKE_PICTURE_CAM_0 = 0,
    TAKE_PICTURE_CAM_1 = 1,
    GET_PICTURE = 2
}cameraCommand_t;

typedef enum errorCode_t{
    CMD_NO_ERROR = 0,
    BAD_MOTOR_CONTROL_CMD = 1,
    BAD_PACKET_HEADER = 2,
    CMD_NOT_RECOGNIZED = 3,
    MC_NODE_NOT_CONNECTED = 4
}errorCode_t;

#define MOTOR_A_TEMP_CHANNEL    15
#define MOTOR_B_TEMP_CHANNEL    14
#define MOTOR_C_TEMP_CHANNEL    13
#define MOTOR_D_TEMP_CHANNEL    12
#define BATTERY_TEMP_CHANNEL    11


#define IMAGE_WIDTH        2592
#define IMAGE_HEIGHT       (1944 / DOWNSAMPLING)

#define DOWNSAMPLING        2
#define DOWNSAMPLED_IMG_WIDTH   (IMAGE_WIDTH / DOWNSAMPLING)
#define DOWNSAMPLE_IMG_HEIGHT   (IMAGE_HEIGHT / DOWNSAMPLING)

#define CAMERA_1_SELECT         0x0001
#define CAMERA_2_SELECT         0x0001

#define TX_RX_UDP_BUFF_SIZE     128

void sendCameraDataOverWifi(uint8_t *jpegBuffer, uint32_t jpegBuffSize, const imageTransferStatus_t status);

#endif /* MAIN_HPP_ */
