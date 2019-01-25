/* work_thread.c */
/* Add by Wuxf 20181129 first version */

#include "work_thread.h"
#include "array.h"

WORK_OBJ_ARRAY work_array[MAX_WORK_OBJ_NUM];
INT32 work_thread_comm_lock;

INT32 work_thread_init(VOID)
{
    INT32 ret;
    
    PT_INIT();
    ret = PT_OPT.mute_init(&work_thread_comm_lock, 0);

    if(ret < 0 )
    {
        return ret;
    }
    memset(work_array,0,sizeof(WORK_OBJ_ARRAY)*MAX_WORK_OBJ_NUM);

    return create_comm_work_thread();
}

static VOID* work_func_caller(VOID * para)
{
    QUE_BLK * blk;
    WORK_INFO * ptr = (WORK_INFO *)para;
    for(;;)
    {
        PT_OPT.sema_wait(ptr->sema);
        blk =  queue_pop(ptr->queue_idx);

        if(NULL == blk)
        {
            PT_OPT.sleep(1000);
            continue;
        }

        ptr->func(blk, ptr->func_para);

        /* Free buffer here */
        if(blk->buf != NULL)
            PT_OPT.mem_free(blk->buf);

        PT_OPT.mem_free(blk);
    }
    return NULL;
}

static UINT32 work_index = 0x18880000UL;

INT32 create_work_thread(INT32 que_idx, 
                                work_func func, 
                                VOID * para,
                                INT32 thread_num,
                                const INT8* name)
{
    INT32 idx;
    INT32 ret;
    INT32 i;
    WORK_OBJ * objptr;

    if((!QUEUE_IDX_VALID(que_idx)) || (NULL == func) || \
        (!THREAD_NUM_VALID(thread_num)))
    {
        return WORK_THREAD_INVALID_PARA;
    }

    /* Get free index */
    PT_OPT.mute_lock(work_thread_comm_lock);
    idx = find_array_free((UINT8 *)work_array, MAX_WORK_OBJ_NUM, \
                                sizeof(WORK_OBJ_ARRAY));

    if(idx < 0)
    {
        PT_OPT.mute_unlock(work_thread_comm_lock);
        return WORK_THREAD_MAX;
    }

    work_array[idx].state = STATE_VALIDE;
    PT_OPT.mute_unlock(work_thread_comm_lock);

    /* init work obj */
    objptr = &work_array[idx].obj;
    objptr->idx = work_index++;
    
    strcpy((char *)&objptr->name, (const char*)name);
    
    objptr->info.func = func;
    objptr->info.queue_idx = que_idx;
    objptr->info.func_para = para;
   
    objptr->info.sema = PT_OPT.sema_init(0);

    if(objptr->info.sema < 0)
    {
        PT_OPT.mute_lock(work_thread_comm_lock);
        memset(&work_array[idx],0,sizeof(WORK_OBJ_ARRAY));
        PT_OPT.mute_unlock(work_thread_comm_lock);
        return ERROR;
    }

    objptr->work_thread_num = 0;
    for(i=0; i<thread_num; i++)
    {
        ret = PT_OPT.create_thread(&objptr->thread_id_table[i], THREAD_NO_JOIN,\
                        work_func_caller, (WORK_INFO*)&objptr->info);

        if(ret < 0)
        {
            if(objptr->work_thread_num > 0)
            {
                /* If already any work thread create scucessful, do not return error */
                continue;
            }
            else
            {
                PT_OPT.sema_close(objptr->info.sema);
                PT_OPT.mute_lock(work_thread_comm_lock);
                memset(&work_array[idx],0,sizeof(WORK_OBJ_ARRAY));
                PT_OPT.mute_unlock(work_thread_comm_lock);

                return ret;
            }
        }
        
        objptr->work_thread_num++;
    }

    return objptr->idx;
}

static bool check_work_idx(VOID* ptr, VOID* para)
{
    if((!para) || (!ptr))
        return false;
    
    INT32 idx = (*(INT32 *)para);
    WORK_OBJ_ARRAY * obj = (WORK_OBJ_ARRAY *)ptr;

    if(obj->obj.idx == idx)
    {
        return true;
    }

    return false;
}

static WORK_OBJ_ARRAY * get_work_obj_by_idx(INT32 idx)
{
    return (WORK_OBJ_ARRAY *)walk_array((UINT8 *)work_array, MAX_WORK_OBJ_NUM, \
                                sizeof(WORK_OBJ_ARRAY), check_work_idx,&idx);
}

INT32 add_work_to_thread(INT32 idx, VOID * work, INT32 size)
{
    INT32   ret;
    QUE_BLK * blk;
    WORK_OBJ_ARRAY * obj = get_work_obj_by_idx(idx);
    if(!obj)
    {
        return ERROR;
    }

    PT_OPT.mem_alloc(sizeof(QUE_BLK), (VOID**)&blk, 0);

    if(!blk)
        return ERROR;

    blk->buf = work;
    blk->buf_size = size;
    blk->buf_type = QUEUE_BUF_TYPE_WORK_THREAD;

    ret = queue_push(obj->obj.info.queue_idx, blk);

    if(ret < 0)
    {
        PT_OPT.mem_free(blk);
        return ret;
    }

    /* wake up work thread */
    PT_OPT.sema_post(obj->obj.info.sema);
    
    return 0;
}

#ifdef WORK_THREAD
/* Create default work thread */
INT32 comm_work_thread_idx = 0;

static VOID comm_work_func(QUE_BLK * blk, VOID * para)
{
    (VOID)para;
    COMM_WORK_THREAD_OBJ * cb_info = (COMM_WORK_THREAD_OBJ *)blk->buf;

    if((!cb_info)&&(!cb_info->cb))
        return;

    cb_info->cb(NULL, cb_info->para);

    /* If we free the mem, we should set the buf to NULL, so that the work thread 
        won't free the buff again */
    PT_OPT.mem_free(blk->buf);
    blk->buf = NULL;
}

INT32 add_task_to_comm_work_thread(VOID * para, work_func cb)
{
    COMM_WORK_THREAD_OBJ * work = NULL;

    PT_OPT.mem_alloc(sizeof(COMM_WORK_THREAD_OBJ), (VOID **)work, 0);

    if(!work)
    {
        return ERROR;
    }

    work->cb = cb;
    work->para = para;
    
    return add_work_to_thread(comm_work_thread_idx, work, \
                            sizeof(COMM_WORK_THREAD_OBJ));
}

INT32 create_comm_work_thread(VOID)
{
    INT32 ret;
    static UINT32 inited_flag;

    if(inited_flag == COMM_WORK_THREAD_START)
    {
        return 0;
    }
    else
    {
        inited_flag = COMM_WORK_THREAD_START;
    }

    ret = new_queue(COMM_WORK_QUEUE_TYPE, NULL);

    if(ret < 0)
    {
        inited_flag = 0;
        return ret;
    }

    comm_work_thread_idx = create_work_thread(ret, 
                                            comm_work_func,
                                            NULL,
                                            1,
                                            (INT8 *)"comm work");

    if(comm_work_thread_idx < 0)
    {
        inited_flag = 0;
        remove_queue(ret);
        return comm_work_thread_idx;
    }

    return OK;
}

#else
INT32 create_comm_work_thread(VOID)
{
    return OK;
}

#endif


