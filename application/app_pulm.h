/* app_pulm.h */

#ifndef __APP_pulm_H__
#define  __APP_pulm_H__
#include "types.h"

#define PULM_ON 1
#define PULM_OFF 0
#define PULM_INT 2299
#define PULM_EXT 3399
#define PULM_US 1
#define PULM_MS 1000
#define PULM_S 1000000
#define PULM_MAX_PER 2000000
#define PULM_MIN_PER 200
#define PULM_MAX_WIDT 1000000
#define PULM_MIN_WIDT 100

INT32 pulm_handle(INT8 **keylist,INT32 len,double data1,INT32 data2,
                      INT8* data3,
                      INT32 operation,
                      INT8* valstr,
                      INT8 *resp_buf);

#endif /* __APP_pulm_H__ */
