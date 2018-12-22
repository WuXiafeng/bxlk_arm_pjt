/* queuehandle.h  */
/* Add by Wuxf 20181201 First version */


#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "types.h"
#include "queue.h"
#include "pf.h"

#define QUEUE_TYPE_SOCKET_TX 0x00004000
#define QUEUE_TYPE_SOCKET_RX 0x00004001

typedef struct queue_block{
    TAILQ_ENTRY(queue_block)  tqe;
    UINT32 buf_type;
    UINT32 buf_size;
    void * buf;
}QUE_BLK;

TAILQ_HEAD(queue_head, queue_block);	

typedef struct queue{
	INT32 que_index;  /*queue index*/
	UINT32 type;   /* queue type  */
	UINT32 status;  /* queue current state */
	INT32 que_len;  /* queue len */
    INT32 que_lock;  /* mute lock */
    VOID * priv;  /* private data */
    struct queue_head  head; /* queue head */
}QUE;

#define SEAT_TAKEN 0xaabbddeeUL

typedef struct queue_index_map{
	INT32 que_index;  //not equal to current array index
	QUE * que_ptr;
    UINT32 if_empty;  /* if current seat empty */
}QUE_MAP;

INT32 queue_init(void);
INT32 new_queue(UINT32 type, VOID * priv);
INT32 queue_push(INT32 idx, QUE_BLK * buf);
QUE_BLK * queue_pop(INT32 idx);
INT32 remove_queue(INT32 idx);

#define QUEUE_IDX_VALID(x) (DATA_IN_RANGE((x),0,QUEUE_POOL_NUM))

/* define YOUR queue type here */
#define QUEUE_BUF_TYPE_WORK_THREAD  0x2200AABB

#endif


