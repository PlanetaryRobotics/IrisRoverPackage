#ifndef MAIN_HPP_
#define MAIN_HPP_

typedef enum controlJpeg_t{
    NEW_COMPRESSION = 0,
    CONTINUE_COMPRESSION = 1,
    END_COMPRESSION = 2,
    UNDEFINED = 3
}controlJpeg_t;

typedef enum cameraCommand_t{
    TAKE_PICTURE_CAM_0 = 0,
    TAKE_PICTURE_CAM_1 = 1,
    GET_PICTURE = 2
}cameraCommand_t;

#define IMAGE_WIDTH        2592
#define IMAGE_HEIGHT       (1944 / DOWNSAMPLING)

#define DOWNSAMPLING        2
#define DOWNSAMPLED_IMG_WIDTH   (IMAGE_WIDTH / DOWNSAMPLING)
#define DOWNSAMPLE_IMG_HEIGHT   (IMAGE_HEIGHT / DOWNSAMPLING)

#define CAMERA_1_SELECT         0x0001
#define CAMERA_2_SELECT         0x0001

#endif /* MAIN_HPP_ */
