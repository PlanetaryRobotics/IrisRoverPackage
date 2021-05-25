#ifndef CAMERA_BITFIELDS_
#define CAMERA_BITFIELDS_

/*
Bit 00 : 01	Compression (0 = 00%, 01 = 25%, 10 = 50%, 11 = 75%)
Bit 02	RGB (0 - Grayscale, 1 - RGB)
Bit 03 : 22	Shutter Width
Bit 23 : 35	Shutter Delay
Bit 36 : 37	Row Bin
Bit 38 : 39	Column Bin
Bit 40 : 51	Horizontal Blanking
Bit 52 : 62	Vertical Blanking
Bit 63	Reserved
*/
struct ConfigureCamera {
    uint64_t compression    : 2;
    uint64_t rgb            : 1;
    uint64_t shutter_width  : 20;
    uint64_t shutter_delay  : 13;
    uint64_t row_bin        : 2;
    uint64_t column_bin     : 2;
    uint64_t horizontal_blanking : 12;
    uint64_t vertical_blanking   : 11;
    uint64_t reserved       : 1
} __atribute__((packed));

/*
Bit 0 : 11	Crop - Upper Left X Value (max: 2592 -> 12b)
Bit 12 : 22	Crop - Upper Left Y Value (max: 1944 -> 11b)
Bit 23 : 34	Crop - Height (max: 2592 -> 12b)
Bit 35 : 45	Crop - Width (max: 1944 -> 11b)
Bit 46 : 63	Reserved 
*/
struct ImageCrop {
    uint64_t upper_left_x : 12;
    uint64_t upper left_y : 11;
    uint64_t height       : 12;
    uint64_t width        : 11;
    uint64_t reserved     : 18;
} __atribute__((packed));

#endif  // CAMERA_BITFIELDS_
