/* app_rfo.h */

#ifndef __APP_rfo_H__
#define  __APP_rfo_H__
#include "types.h"

#define RFO_ON 1
#define RFO_OFF 0

INT32 rfo_handle(INT8 **keylist,INT32 len,double data1,INT32 data2,
                      INT8* data3,
                      INT32 operation,
                      INT8* valstr,
                      INT8 *resp_buf);

#endif /* __APP_rfo_H__ */
