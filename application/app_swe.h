/*app_swe.h */

#ifndef __APP_swe_H__
#define  __APP_swe_H__
#include "types.h"

#define SWE_ON 1
#define SWE_OFF 0

#define POINT_MAX 1001
#define POINT_MIN 2

#define SWE_MS 1
#define SWE_S 1000
#define SWE_MAX_DWEL 1000
#define SWE_MIN_DWEL 10

#define SWE_REP_SING 9988
#define SWE_REP_CONT 8877

#define SWE_STRG_IMM 8833
#define SWE_STRG_EXT 9933
#define SWE_STRG_KEY 2255

#define SWE_STRG_SLOP_EXTN 2389
#define SWE_STRG_SLOP_EXTP 2619

#define SWE_DIR_UP  2893
#define SWE_DIR_DOWN 1621

INT32 swe_handle(INT8 **keylist,INT32 len,double data1,INT32 data2,
                      INT8* data3,
                      INT32 operation,
                      INT8* valstr,
                      INT8 *resp_buf);

#endif /* __APP_swe_H__ */
