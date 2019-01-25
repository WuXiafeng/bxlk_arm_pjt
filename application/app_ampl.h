/* app_ampl.h */

#ifndef __APP_AMPL_H__
#define  __APP_AMPL_H__
#include "types.h"

INT32 ampl_handle(INT8 **keylist,INT32 len,double data1,INT32 data2,
                      INT8* data3,
                      INT32 operation,
                      INT8* valstr,
                      INT8 *resp_buf);

#define UNIT_DBV (7788)
#define UNIT_DBMV (5566)
#define UNIT_DBM (3344)
#define UINT_MW (1000)
#define UNIT_UV (1)

#define MAX_DBM (20)
#define MIN_DBM (-127)
#define MAX_DBMV (67)
#define MIN_DBMV (-80)
#define MAX_DBV (127)
#define MIN_DBV (-20)
#define MAX_UV  (10000000)
#define MIN_UV  (1)
#endif /* __APP_AMPL_H__ */

