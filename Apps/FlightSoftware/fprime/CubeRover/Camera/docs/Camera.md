<title>Camera Component Dictionary</title>
# Camera Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|Get_Status|0 (0x0)|DEPRECATED. Does nothing.| | |
|Take_Image|1 (0x1)|Take an Image| | |
| | | |camera_num|U8|
                        0: Camera 0     1: Camera 1
                    |
| | | |callback_id|U16|
                        Identifier which will be downlinked with the images from this command, allowing us to map which downlinked images related to which 'take photo' command
                    |
|Take_Image_Section|161 (0xa1)|Take a Full Image but only downlink a subset of the FileGroup Lines from memory (from start_line to end_line).| | |
| | | |camera_num|U8|
                        0: Camera 0     1: Camera 1
                    |
| | | |startLine|U16|
                		Line in full image memory to start downlinking. Inclusive (max is 1944). In memory space not pixel space. Indexed from 0.
                	|
| | | |endLine|U16|
                		Line in full image memory to stop downlinking. Exclusive (max is 1943). In memory space not pixel space. Indexed from 0.
                	|
| | | |callback_id|U16|
                        Identifier which will be downlinked with the images from this command, allowing us to map which downlinked images related to which 'take photo' command
                    |
|Take_Image_Section_Adv|162 (0xa2)|Take a Full Image but only downlink a subset of the FileGroup Lines from memory (from start_line to end_line).
                Offers advanced capture and downlink settings.| | |
| | | |camera_num|U8|
                        0: Camera 0     1: Camera 1
                    |
| | | |startLine|U16|
                		Line in full image memory to start downlinking. Inclusive (max is 1944). In memory space not pixel space. Indexed from 0.
                	|
| | | |endLine|U16|
                		Line in full image memory to stop downlinking. Exclusive (max is 1943). In memory space not pixel space. Indexed from 0.
                	|
| | | |callback_id|U16|
                        Identifier which will be downlinked with the images from this command, allowing us to map which downlinked images related to which 'take photo' command
                    |
| | | |eraseFirst|bool|
                        Whether or not to erase the flash before capturing an image.
                    |
| | | |n_bin|U8|
                        Binning setting: factor by which lines should be downsampled through binning. Values gt 2 will trigger an attempt at binning. This is a Bayer-preserving binning process that skips over any line or frame headers embedded in the image by the FPGA FW. Also RLEs homogeneous lines.
                    |
| | | |compressLine|bool|
                        Whether or not to attempt to compress lines (after binning if performed). Also RLEs homogeneous lines.
                    |
|Read_Image_Section_Adv|163 (0xa3)|Downlinks an image from flash based on the given settings. Doesn't capture an image or erase flash first, just sends whatever's already there.
                Camera number, callback_id, and capture time (file group id) are stored in Hercules voltile mem. since the last command that triggered an image capture. If Hercules has rebooted since then, this will still work but those values will be wrong.| | |
| | | |startLine|U16|
                		Line in full image memory to start downlinking. Inclusive (max is 1944). In memory space not pixel space. Indexed from 0.
                	|
| | | |endLine|U16|
                		Line in full image memory to stop downlinking. Exclusive (max is 1943). In memory space not pixel space. Indexed from 0.
                	|
| | | |n_bin|U8|
                        Binning setting: factor by which lines should be downsampled through binning. Values gt 2 will trigger an attempt at binning. This is a Bayer-preserving binning process that skips over any line or frame headers embedded in the image by the FPGA FW. Also RLEs homogeneous lines.
                    |
| | | |compressLine|bool|
                        Whether or not to attempt to compress lines (after binning if performed). Also RLEs homogeneous lines.
                    |
|Capture_Image_Only|164 (0xa4)|Trigger Image Capture but don't downlink it (that can be done later with: `Read_Image_Section_Adv`).| | |
| | | |camera_num|U8|
                        0: Camera 0     1: Camera 1
                    |
| | | |callback_id|U16|
                        Identifier which will be downlinked with the images from this command, allowing us to map which downlinked images related to which 'take photo' command
                    |
| | | |eraseFirst|bool|
                        Whether or not to erase the flash before capturing an image.
                    |
|Capture_Deployment_Image|175 (0xaf)|Waits to capture an image until a series of trigger condition is satisfied. 

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

                This is a late add, definitely a trick, and may or may not work but we'll try it.| | |
| | | |camera_num|U8|
                        0: Camera 0     1: Camera 1
                    |
| | | |callback_id|U16|
                        Identifier which will be downlinked with the images from this command, allowing us to map which downlinked images related to which 'take photo' command
                    |
| | | |eraseFirst|bool|
                        Whether or not to erase the flash before capturing an image.
                    |
| | | |timeoutMs|U32|
                        Max time to wait for a trigger condition before timing out (ms).
                    |
| | | |triggerOnTimeout|bool|
                        Whether or not to capture an image on timeout just in case.
                    |
| | | |conditionModeAll|bool|
                        True: ALL trigger conditions that are ON must be met. False: ANY of the trigger conditions that are ON (i.e. just 1) will trigger image capture.
                    |
| | | |imuXAcc_on|bool|
                        Whether or not to use the IMU X Acceleration window as a trigger.
                    |
| | | |imuXAcc_triggerMode|bool|
                        True: Trigger will occur if the sensor value is in the window. False: Trigger will occur if sensor value is outside the window.
                    |
| | | |imuXAcc_min|I16|
                        Lower bound (inclusive) of window for IMU X Acceleration (raw reading).
                    |
| | | |imuXAcc_max|I16|
                        Upper bound (exclusive) of window for IMU X Acceleration (raw reading).
                    |
| | | |imuYAcc_on|bool|
                        Whether or not to use the IMU Y Acceleration window as a trigger.
                    |
| | | |imuYAcc_triggerMode|bool|
                        True: Trigger will occur if the sensor value is in the window. False: Trigger will occur if sensor value is outside the window.
                    |
| | | |imuYAcc_min|I16|
                        Lower bound (inclusive) of window for IMU Y Acceleration (raw reading).
                    |
| | | |imuYAcc_max|I16|
                        Upper bound (exclusive) of window for IMU Y Acceleration (raw reading).
                    |
| | | |imuZAcc_on|bool|
                        Whether or not to use the IMU Z Acceleration window as a trigger.
                    |
| | | |imuZAcc_triggerMode|bool|
                        True: Trigger will occur if the sensor value is in the window. False: Trigger will occur if sensor value is outside the window.
                    |
| | | |imuZAcc_min|I16|
                        Lower bound (inclusive) of window for IMU Z Acceleration (raw reading).
                    |
| | | |imuZAcc_max|I16|
                        Upper bound (exclusive) of window for IMU Z Acceleration (raw reading).
                    |
| | | |wdi28V_on|bool|
                        Whether or not to use the 28V voltage reported by WatchDogInterface as a trigger;
                    |
| | | |wdi28V_triggerMode|bool|
                        True: Trigger will occur if the sensor value is in the window. False: Trigger will occur if sensor value is outside the window.
                    |
| | | |wdi28V_min|I16|
                        Lower bound (inclusive) of window for 28V voltage reported by WatchDogInterface (raw ADC value).
                    |
| | | |wdi28V_max|I16|
                        Upper bound (exclusive) of window for 28V voltage reported by WatchDogInterface (raw ADC value).
                    |
|Error|2 (0x2)|DEPRECATED. Does nothing.| | |
| | | |action|U8|
                        0x00: Clear Error     0xff: Read Error  !! Not sure how this is being used
                    |
|Configure_Camera0|3 (0x3)|DEPRECATED. Does nothing. Configs are now downlinked as command args in capture commands.| | |
| | | |config|U64|
                        Packed bitfield containing camera configurations. TODO: Copy the bit fields from C&TL to here
                    |
|Camera0_Crop|4 (0x4)|DEPRECATED. Does nothing. Use section commands for cropping.| | |
| | | |config|U64|
                        Packed bitfield containing crop locations. TODO: Copy bitfields from C&TL to here
                    |
|Configure_Camera1|5 (0x5)|DEPRECATED. Does nothing. Configs are now downlinked as command args in capture commands.| | |
| | | |config|U64|
                        Packed bitfield containing camera configurations. TODO: Copy the bit fields from C&TL to here
                    |
|Camera1_Crop|6 (0x6)|DEPRECATED. Does nothing. Use section commands for cropping.| | |
| | | |config|U64|
                        Packed bitfield containing crop locations. TODO: Copy bitfields from C&TL to here
                    |
|Erase_Flash|8 (0x8)|Erases camera flash (writes 0xFF in all data).
                Only erases up to the given sector number.| | |
| | | |numSectors|U8|
                        Number of sectors to erase. Default is 40. Only change if you have a reason.
                    |
|Soft_Camera_Reset|9 (0x9)|DEPRECATED. Does nothing. Command WD directly to power off or reset FPGA.| | |
| | | |cameraNum|U8|
                        Camera Number
                    |
|Image_Dump|10 (0xa)|Dump (downlink) whatever is stored in the the camera flash rn 
                without capturing a new image or writing anything to flash 
                first. Use `Read_Image_Section_Adv` for a more advanced image downlink.
                Camera number, callback_id, and capture time (file group id) are stored in Hercules volatile mem. since the last command that triggered an image capture. If Hercules has rebooted since then, this will still work but those values will be wrong.| | |
|Downlink_Grid|11 (0xb)|Downlinks a test grid. If `viaFlash`, grid will be written to
                flash then read from flash and downlinked. Otherwise, it'll
                just be downlinked directly.| | |
| | | |viaFlash|bool|
                    Whether or not to write to flash and read from flash before
                    downlinking.
                    |
|Downlink_Test_Sequence|12 (0xc)|Downlinks a test image where every 4B are an incrementing U32.
                If `viaFlash`, grid will be written to flash then read from
                flash and downlinked. Otherwise, it'll just be downlinked
                directly.| | |
| | | |viaFlash|bool|
                    Whether or not to write to flash and read from flash before
                    downlinking.
                    |

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|Cam_ComponentImagesRequested|0 (0x0)|U32|Number of images requested by other Fprime components|
|Cam_CommandImagesRequested|1 (0x1)|U32|Number of images commanded from the ground|
|Cam_ImagesSent|2 (0x2)|U32|Number of images sent for downlink|
|Cam_BytesSent|3 (0x3)|U32|Number of bytes sent for downlink|
|Cam_LatestCallbackId|4 (0x4)|U16|The latest callback ID that triggerd an image to be taken|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|Camera_FailedToTakeImage|0 (0x0)|A basic notification that, for some reason, the camera failed to capture an image. Try again.| | | | |
|Camera_ImageCaptureComplete|1 (0x1)|Notice that the camera has finished downlinking the image with the given settings.| | | | |
| | | |fileGroupId|U16||Locally unique identifier for image file, used to match image lines to an image. (hash of U32 time). Not guaranteed to be unique across whole mission but very likely over time scales of hours to days.|
| | | |callbackId|U16||Callback ID sent along with the command which generated this request.|
|Camera_ImageDownlinkComplete|2 (0x2)|Downlink of requested image data is complete.| | | | |
| | | |fileGroupId|U16||Locally unique identifier for image file, used to match image lines to an image. (hash of U32 time). Not guaranteed to be unique across whole mission but very likely over time scales of hours to days.|
| | | |callbackId|U16||Callback ID sent along with the command which generated this request.|
|Camera_DeploymentImageCaptured|3 (0x3)|Event emitted once the trigger conditions for a deployment image have been met and an image has been captured. A deployment image should now reside in memory.| | | | |
| | | |fileGroupId|U16||Locally unique identifier for image file, used to match image lines to an image. (hash of U32 time). Not guaranteed to be unique across whole mission but very likely over time scales of hours to days.|
| | | |callbackId|U16||Callback ID sent along with the command which generated this request.|
|Camera_DeploymentImageCaptureWaiting|4 (0x4)|Confirmation that Hercules received a request to downlink a deployment image. This includes a copy of all capture and trigger settings.| | | | |
| | | |camera_num|U8||0: Camera 0     1: Camera 1|
| | | |callback_id|U16||Identifier which will be downlinked with the images from this command, allowing us to map which downlinked images related to which 'take photo' command|
| | | |eraseFirst|bool||Whether or not to erase the flash before capturing an image.|
| | | |timeoutMs|U32||Max time to wait for a trigger condition before timing out (ms).|
| | | |triggerOnTimeout|bool||Whether or not to capture an image on timeout just in case.|
| | | |conditionModeAll|bool||True: ALL trigger conditions that are ON must be met. False: ANY of the trigger conditions that are ON (i.e. just 1) will trigger image capture.|
| | | |imuXAcc_on|bool||Whether or not to use the IMU X Acceleration window as a trigger.|
| | | |imuXAcc_triggerMode|bool||True: Trigger will occur if the sensor value is in the window. False: Trigger will occur if sensor value is outside the window.|
| | | |imuXAcc_min|I16||Lower bound (inclusive) of window for IMU X Acceleration (raw reading).|
| | | |imuXAcc_max|I16||Upper bound (exclusive) of window for IMU X Acceleration (raw reading).|
| | | |imuYAcc_on|bool||Whether or not to use the IMU Y Acceleration window as a trigger.|
| | | |imuYAcc_triggerMode|bool||True: Trigger will occur if the sensor value is in the window. False: Trigger will occur if sensor value is outside the window.|
| | | |imuYAcc_min|I16||Lower bound (inclusive) of window for IMU Y Acceleration (raw reading).|
| | | |imuYAcc_max|I16||Upper bound (exclusive) of window for IMU Y Acceleration (raw reading).|
| | | |imuZAcc_on|bool||Whether or not to use the IMU Z Acceleration window as a trigger.|
| | | |imuZAcc_triggerMode|bool||True: Trigger will occur if the sensor value is in the window. False: Trigger will occur if sensor value is outside the window.|
| | | |imuZAcc_min|I16||Lower bound (inclusive) of window for IMU Z Acceleration (raw reading).|
| | | |imuZAcc_max|I16||Upper bound (exclusive) of window for IMU Z Acceleration (raw reading).|
| | | |wdi28V_on|bool||Whether or not to use the 28V voltage reported by WatchDogInterface as a trigger;|
| | | |wdi28V_triggerMode|bool||True: Trigger will occur if the sensor value is in the window. False: Trigger will occur if sensor value is outside the window.|
| | | |wdi28V_min|U16||Lower bound (inclusive) of window for 28V voltage reported by WatchDogInterface (raw ADC value).|
| | | |wdi28V_max|U16||Upper bound (exclusive) of window for 28V voltage reported by WatchDogInterface (raw ADC value).|
