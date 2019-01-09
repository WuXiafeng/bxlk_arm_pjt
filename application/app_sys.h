/* app_sys.h */

#ifndef __APP_sys_H__
#define  __APP_sys_H__
#include "types.h"

INT32 sys_handle(INT8 **keylist,INT32 len,double data1,INT32 data2,
                      INT8* data3,
                      INT32 operation,
                      INT8* valstr,
                      INT8 *resp_buf);

#endif /* __APP_sys_H__ */
