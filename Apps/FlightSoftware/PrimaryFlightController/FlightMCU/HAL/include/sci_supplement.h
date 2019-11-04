#ifndef __SCI_SUPPLEMENT_H__
#define __SCI_SUPPLEMENT_H__

#include "reg_sci.h"

#ifdef __cplusplus
extern "C" {
#endif

  int32_t sciReceiveWithTimeout(sciBASE_t *sci, uint32 length, uint8 * data, uint32_t timeoutMs);

#ifdef __cplusplus
}
#endif

#endif
