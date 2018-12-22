/* queuehandle.c */
/* Add by Wuxf 20181202 First version */

#include "queuehandle.h"

QUE_MAP que_map_arr[QUEUE_POOL_NUM];
static INT32 queue_idx = 0x66660000L;
INT32 seat_lock;

INT32 seat_init(void)
{
    int ret;

    ret = PT_OPT.mute_init(&seat_lock, 0);

    if(ret < 0)
    {
        return COMM_LAYER_MUTE_LOCK_ERROR;
    }

    memset(que_map_arr,0,sizeof(QUE_MAP)*QUEUE_POOL_NUM);

    return OK;
}

QUE_MAP * __get_empty_seat(void)
{
    INT32 i;
    for(i=0;i<QUEUE_POOL_NUM;i++)
    {
        if(que_map_arr[i].if_empty != SEAT_TAKEN)
        {
            return &que_map_arr[i];
        }
    }
    
    return NULL;
}

QUE_MAP * __search_seat_by_que_idx(INT32 que_idx)
{
    INT32 i;
    for(i=0;i<QUEUE_POOL_NUM;i++)
    {
        if((que_map_arr[i].que_index == que_idx) && 
            (que_map_arr[i].if_empty == SEAT_TAKEN))
        {
            return &que_map_arr[i];
        }
    }
    
    return NULL;
}

INT32 insert_que_to_seat(QUE * obj)
{
    if(!obj)
    {
        return ERROR;
    }
    
    PT_OPT.mute_lock(seat_lock);    
    QUE_MAP * seat = __get_empty_seat();

    if(!seat)
    {
        PT_OPT.mute_unlock(seat_lock);
        return QUEUE_MODULE_MAX_QUEUE;
    }

    seat->if_empty = SEAT_TAKEN;
    seat->que_index = obj->que_index;
    seat->que_ptr = obj;
    PT_OPT.mute_unlock(seat_lock);

    return OK;
}

INT32 free_seat_by_queue_idx(INT32 idx)
{
    PT_OPT.mute_lock(seat_lock);    
    QUE_MAP * seat = __search_seat_by_que_idx(idx);

    if(!seat)
    {
        PT_OPT.mute_unlock(seat_lock);
        return ERROR;
    }

    memset(seat,0,sizeof(QUE_MAP));
    PT_OPT.mute_unlock(seat_lock);

    return OK;
}

QUE * get_que_by_que_idx(INT32 idx)
{ 
    QUE_MAP * seat = __search_seat_by_que_idx(idx);

    if(!seat)
    {
        return NULL;
    }
    return seat->que_ptr;
}

INT32 queue_init(void)
{
    return seat_init();
}

/* Create new mw queue */
INT32 new_queue(UINT32 type, VOID * priv)
{
    QUE * newque;
    int ret;
    
    ret = PT_OPT.mem_alloc(sizeof(QUE), (VOID**)&newque, 0);
    if(!newque)
    {
        return ERROR;
    }

    TAILQ_INIT(&newque->head);
    newque->que_index = queue_idx++;
    newque->que_len = 0;
    
    ret = PT_OPT.mute_init(&newque->que_lock, 0);
    if(ret < 0)
    {
        PT_OPT.mem_free(newque);
		return ERROR;
    }

    newque->status = 0;
    newque->type = type;
    newque->priv = priv;

    ret = insert_que_to_seat(newque);
    if(ret < 0)
    {
        PT_OPT.mem_free(newque);
        return ERROR;         
    }
    
    return newque->que_index;
}

QUE * find_queue(INT32 idx)
{
    return get_que_by_que_idx(idx);
}

INT32 remove_queue(INT32 idx)
{
    QUE * tmp = get_que_by_que_idx(idx);

    if(tmp->priv)
    {
        PT_OPT.mem_free(tmp->priv);
    }    
    
    if(tmp)
    {
        PT_OPT.mem_free(tmp);
    }
    
    return free_seat_by_queue_idx(idx);
}

INT32 queue_push(INT32 idx, QUE_BLK * buf)
{
    QUE * queue;
    
    if(!buf)
    {        
        return COMM_ERR_NULL_PTR;
    }

    queue = get_que_by_que_idx(idx);
    if(!queue)
    {
        return QUEUE_MODULE_QUEUE_NOT_EXIST;         
    }
    
    PT_OPT.mute_lock(queue->que_lock);
    
    if(queue->que_len >= QUEUE_LEN)
    {
        PT_OPT.mute_unlock(queue->que_lock);
        return QUEUE_MODULE_QUEUE_FULL;         
    }
    
    TAILQ_INSERT_HEAD(&queue->head, buf, tqe);
    queue->que_len++;
    PT_OPT.mute_unlock(queue->que_lock);

    return OK;
}

QUE_BLK * queue_pop(INT32 idx)
{
    QUE * queue;
    QUE_BLK * blk;

    queue = get_que_by_que_idx(idx);
    if(!queue)
    {
        return NULL;         
    }

    PT_OPT.mute_lock(queue->que_lock);
    if(TAILQ_EMPTY(&queue->head))
    {
        PT_OPT.mute_unlock(queue->que_lock);
        return NULL;
    }
    blk = TAILQ_LAST(&queue->head, queue_head);
    TAILQ_REMOVE(&queue->head, blk, tqe);
    queue->que_len--;
    PT_OPT.mute_unlock(queue->que_lock);

    return blk;
}

INT32 queue_cur_len (INT32 idx)
{
    QUE * queue;

    queue = get_que_by_que_idx(idx);
    if(!queue)
    {
        return QUEUE_MODULE_QUEUE_NOT_EXIST;         
    }

    return queue->que_len;
}

INT32 queue_is_empty(INT32 idx)
{
    QUE * queue;
    queue = get_que_by_que_idx(idx);
    if(!queue)
    {
        return QUEUE_MODULE_QUEUE_NOT_EXIST;         
    }

    return TAILQ_EMPTY(&queue->head);
}


