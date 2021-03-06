/*core.h*/
/*Add by Wuxiafeng 20181202 first version*/

#ifndef __CORE_H__
#define __CORE_H__

#include "types.h"
#include "queuehandle.h"
#include "work_thread.h"
#include "glb_conf.h"

typedef bool (*core_send_filter_func)(INT8 *buf, INT32 len);

extern INT32 rx_work_thread_id;
extern INT32 tx_work_thread_id;

int core_init(void);

void start_socket_session_control(void);
INT32 core_state_manager_init(VOID);
VOID core_feed_usb(VOID);

VOID regst_socket_msg_filter(core_send_filter_func func);
VOID unregst_socket_msg_filter(void);

VOID core_msg_send(INT8 *buf, INT32 len);

#endif /* __CORE_H__ */

