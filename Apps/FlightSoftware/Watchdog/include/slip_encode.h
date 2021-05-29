#ifndef __WATCHDOG_SLIP_ENCODE_H__
#define __WATCHDOG_SLIP_ENCODE_H__

SlipEncode__Status SlipEncode__encode(const uint8_t* input,
                                      size_t inputLen,
                                      size_t* inputUsed,
                                      uint8_t* output,
                                      size_t outputLen,
                                      size_t* outputUsed);

#endif /* __WATCHDOG_SLIP_ENCODE_H__ */