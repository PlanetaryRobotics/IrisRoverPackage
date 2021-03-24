/* =================================================================================
File name:       PI.H
===================================================================================*/

// TODO : switch from _iq to something else?

#ifndef __PI_H__
#define __PI_H__


uint16_t Kp_heater = 500;

/* temperature stored as int where 0= -40C and 65534= 125C
 *      ideal temp is 23C [optimal battery discharge]
 *          65534 / 165 * 63 = 25022
*/
uint16_t battery_target_temp = 25022;
uint8_t thermistor_ref_len = 34;

// voltage, where LSB = 0.0008056640625V
//    from voltage divider with thermistor and 10k resistor
//    temperatures start at -40C and go to 125C in 5C increments
unsigned short thermistor_ref_voltages[34] ={
                                             3977,
                                             3933,
                                             3876,
                                             3803,
                                             3712,
                                             3601,
                                             3468,
                                             3312,
                                             3135,
                                             2938,
                                             2726,
                                             2503,
                                             2275,
                                             2048,
                                             1828,
                                             1618,
                                             1423,
                                             1245,
                                             1084,
                                             941,
                                             815,
                                             705,
                                             609,
                                             527,
                                             456,
                                             395,
                                             342,
                                             297,
                                             259,
                                             226,
                                             197,
                                             173,
                                             152,
                                             133
};


#endif // __PI_H__

