/*app_swe.h */

#ifndef __APP_swe_H__
#define  __APP_swe_H__
#include "types.h"

INT32 swe_handle(INT8 **keylist,INT32 len,double data1,INT32 data2,
                      INT8* data3,
                      INT32 operation,
                      INT8* valstr,
                      INT8 *resp_buf);

#endif /* __APP_swe_H__ */
