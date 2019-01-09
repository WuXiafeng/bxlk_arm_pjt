/* app_freq.h */

#ifndef __APP_FREQ_H__
#define __APP_FREQ_H__

#include "types.h"

#define UNIT_GHZ (1000000)
#define UNIT_MHZ (1000)
#define UNIT_KHZ (1)

#define SCAL_LOG (32923)
#define SCAL_LIN (39235)

#define MAX_FREQ (18000000)
#define MIN_FREQ (100000)

INT32 freq_handle(INT8 **keylist,INT32 len,double data1,INT32 data2,
                      INT8 * data3,
                      INT32 operation,
                      INT8 * valstr,
                      INT8 *resp_buf);

#endif /* __APP_FREQ_H__ */

