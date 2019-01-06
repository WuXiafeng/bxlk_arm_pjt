/* msg_handle.h */

/* 20181203 first version  */

#ifndef __MSG_HANDLE_H__
#define __MSG_HANDLE_H__
#include "types.h"
#include "queuehandle.h"

VOID rx_msg_handle(QUE_BLK * blk, VOID * para);
INT32 get_value(INT8 **keylist, INT32 len, INT8* resp_buf);
INT32 app_init(void);

#endif /* __MSG_HANDLE_H__ */

