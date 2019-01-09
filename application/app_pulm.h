/* app_pulm.h */

#ifndef __APP_pulm_H__
#define  __APP_pulm_H__
#include "types.h"

INT32 pulm_handle(INT8 **keylist,INT32 len,double data1,INT32 data2,
                      INT8* data3,
                      INT32 operation,
                      INT8* valstr,
                      INT8 *resp_buf);

#endif /* __APP_pulm_H__ */
