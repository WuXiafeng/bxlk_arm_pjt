/* app_sys.h */

#ifndef __APP_sys_H__
#define  __APP_sys_H__
#include "types.h"

#define SYS_YEAR_MIN 1980
#define SYS_YEAR_MAX 2030
#define SYS_MONTH_MIN 1
#define SYS_MONTH_MAX 12
#define SYS_DAY_MAX 31
#define SYS_DAY_MIN 1
#define SYS_HOUR_MAX 23
#define SYS_HOUR_MIN 0
#define SYS_MINT_MAX 59
#define SYS_MINT_MIN 0
#define SYS_SECD_MAX 59
#define SYS_SECD_MIN 0
#define SYS_INT10MHZ 8923
#define SYS_EXT10MHZ 1268

INT32 sys_handle(INT8 **keylist,INT32 len,double data1,INT32 data2,
                      INT8* data3,
                      INT32 operation,
                      INT8* valstr,
                      INT8 *resp_buf);

#endif /* __APP_sys_H__ */
