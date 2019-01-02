/*glb_conf.h*/
/*  Add by Wuxf 20181202 First version */

#ifndef __GLB_DEF_H__
#define __GLB_DEF_H__

/* max queue number */
#define QUEUE_POOL_NUM  20

/* max queue len */
#define QUEUE_LEN  100

/* RX work thread number */
#define RX_THREAD_NUM 2

/* TX work thread number */
#define TX_THRAED_NUM 2

/* MAX node number*/
#define MAX_NODE_NUM 100

/* Enable default work thread for async works */
#define WORK_THREAD

/* Config file path*/
#define CFG_FILE_PATH "/mnt/flash/bxlk.confg"

/* Backup config file path */
#define BK_CFG_FIEL_PATH "/mnt/flash/bxlk.confg.bak"

#endif /* __GLB_DEF_H__ */


