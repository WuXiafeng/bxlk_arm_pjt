/* work_thread.h */
/* Add by Wuxf 20181129 first version */

#ifndef __WORK_THREAD_H__
#define __WORK_THREAD_H__
#include "types.h"
#include "queuehandle.h"

typedef VOID (*work_func)(QUE_BLK * blk, VOID * para);

typedef struct  work_info{
    INT32 queue_idx;
    work_func func;
    VOID * func_para;
    INT32 sema;    
}WORK_INFO;

#define MAX_THREAD_EACH_WORK_OBJ 4
#define MAX_WORK_OBJ_NUM 8
#define THREAD_NUM_VALID(x) (DATA_IN_RANGE((x),0,MAX_THREAD_EACH_WORK_OBJ))

typedef struct work_object{
    INT32 idx;
    INT8  name[32];
    INT32 que_idx;
    INT32 work_thread_num;
    unsigned long thread_id_table[MAX_THREAD_EACH_WORK_OBJ];
    WORK_INFO info;
}WORK_OBJ;

typedef struct work_obj_array{
    UINT32 state;
    WORK_OBJ  obj;
}WORK_OBJ_ARRAY;

#ifdef WORK_THREAD
typedef struct comm_work_thread_obj{
    work_func cb;
    VOID* para;
}COMM_WORK_THREAD_OBJ;

#define COMM_WORK_THREAD_START 0x77778888Ul
#define COMM_WORK_QUEUE_TYPE 0xaaaabbbbUl
#endif

/* Init work thread mode */
INT32 work_thread_init(VOID);

/* Create a new work thread */
INT32 create_work_thread(INT32 que_idx, 
                                work_func func, 
                                VOID * para,
                                INT32 thread_num,
                                const INT8* name);

/* Add a new task to work thread  */
INT32 add_work_to_thread(INT32 idx, VOID * work, INT32 size);

#ifdef WORK_THREAD
/* Add async funtion to comm work thread,  cb will be called later */
INT32 add_task_to_comm_work_thread(VOID * para, work_func cb);
INT32 create_comm_work_thread(VOID);

#endif

#endif /* __WORK_THREAD_H__ */

