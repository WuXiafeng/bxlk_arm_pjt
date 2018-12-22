/* pf.c */
/*  Add by Wuxf 20181202 First version */

#include "pf.h"
#include "array.h"

#define MAX_CV_NUM 20
#define MAX_MUTE_NUM 40
 
 typedef struct condition_info{
	 UINT32 state;
	 pthread_cond_t *c;
	 pthread_mutex_t *m;
 }CV_INFO;
 
 typedef struct mute_info{
	 UINT32 state;	  
	 pthread_mutex_t *m;
 }MUTE_INFO;
 
 CV_INFO cv_array[MAX_CV_NUM];
 MUTE_INFO mute_arrary[MAX_MUTE_NUM];
 
 pthread_condattr_t  cvattr;
 pthread_mutexattr_t mxattr;
 PLTF_OBJ * pt;

 INT32 linux_pt_init(VOID)
 {
	 memset(cv_array, 0 , sizeof(CV_INFO)*MAX_CV_NUM);
	 memset(mute_arrary, 0, sizeof(MUTE_INFO)*MAX_MUTE_NUM);
 
	 if (pthread_condattr_init(&cvattr) != 0) {
		 return (ERROR);
	 }
 
	 /* TODO: check the parameter  "CLOCK_MONOTONIC " */
	 if (pthread_condattr_setclock(&cvattr, CLOCK_MONOTONIC) != 0) {
		 return (ERROR);
	 }
	 if (pthread_mutexattr_init(&mxattr) != 0) {
		 pthread_condattr_destroy(&cvattr);
		 return (ERROR);
	 }
 
	 // if this one fails we don't care.
	 (void) pthread_mutexattr_settype(
		 &mxattr, PTHREAD_MUTEX_ERRORCHECK);    
 
	 return 0;
 }

 INT32 linux_alloc(INT32 size, VOID **ptr, INT32 option)
 {
	 (VOID)option;
	 *ptr = malloc(size);
 
	 if(!(*ptr))
		 return ERROR;
	 
	 return OK;
 }
 
 VOID linux_free(VOID * ptr)
 {
	 free(ptr);
 }
 
 INT32 linux_pthread_cond_wait(INT32 idx)
 {
	 int rv;
 
	 if((idx < 0)||(idx >= MAX_CV_NUM))
		 return PLATFORM_LAYER_INVALIDE_CV_IDX;
	 
	 if(STATE_VALIDE != cv_array[idx].state)
	 {
		 return PLATFORM_LAYER_CV_NOT_EXIST;
	 }
	 
	 pthread_cond_t *c = cv_array[idx].c;
	 pthread_mutex_t *m = cv_array[idx].m;
 
	 if ((rv = pthread_cond_wait(c, m)) != 0) {
		 return ERROR;
	 }
 
	 return OK;
 }
 
 INT32 linux_pthread_cond_wake(INT32 idx){
	 INT32 rv;
	 if((idx < 0)||(idx >= MAX_CV_NUM))
		 return PLATFORM_LAYER_INVALIDE_CV_IDX;
	 
	 if(STATE_VALIDE != cv_array[idx].state)
	 {
		 return PLATFORM_LAYER_CV_NOT_EXIST;
	 }
	 
	 pthread_cond_t *c = cv_array[idx].c;
 
	 if ((rv = pthread_cond_broadcast(c)) != 0) {
		 return ERROR;
	 }
 
	 return OK;
 }
 
 pttime linux_get_cur_clock(VOID)
 {
	 struct timespec ts;
	 pttime		msec;
 
	 if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
		 return ERROR;
	 }
 
	 msec = ts.tv_sec;
	 msec *= 1000;
	 msec += (ts.tv_nsec / 1000000);
	 return (msec);
 }
 
 int linux_get_pid(void)
 {
	 return getpid();
 }
 
 unsigned long linux_get_pthread(VOID){
    return pthread_self();
 }
 
 INT32 linux_cv_until(INT32 idx, pttime until)
 {
	 struct timespec ts;
	 INT32 rv;
	 
	 if((idx < 0)||(idx >= MAX_CV_NUM))
		 return PLATFORM_LAYER_INVALIDE_CV_IDX;
	 
	 if(STATE_VALIDE != cv_array[idx].state)
	 {
		 return PLATFORM_LAYER_CV_NOT_EXIST;
	 }
	 
	 pthread_cond_t *c = cv_array[idx].c;
	 pthread_mutex_t *m = cv_array[idx].m;
 
	 // Our caller has already guaranteed a sane value for until.
	 ts.tv_sec	= until / 1000;
	 ts.tv_nsec = (until % 1000) * 1000000;
 
	 switch ((rv = pthread_cond_timedwait(c, m, &ts))) 
	 {
		 case 0:
			 return (0);
		 case ETIMEDOUT:
		 case EAGAIN:
			 return (LLATFORM_LAYER_TIME_OUT);
	 }
 
	 return (ERROR);
 }
 
 INT32 linux_cv_init(INT32* idx, INT32 mute_idx)
 {
	 int cv_idx;
	 cv_idx = find_array_free((UINT8 *)cv_array, MAX_CV_NUM, sizeof(CV_INFO));
 
	 if(cv_idx == ERROR)
		 return ERROR;
 
	 if((mute_idx < 0)||(mute_idx >= MAX_MUTE_NUM))
		 return PLATFORM_LAYER_INVALIDE_MUTE_IDX;
 
	 if(STATE_VALIDE != mute_arrary[mute_idx].state)
	 {
		 return PLATFORM_LAYER_MUTE_NOT_EXIST;
	 }
	 
	 while (pthread_cond_init(cv_array[cv_idx].c, &cvattr) != 0) 
	 {
		 usleep(10000);
	 }
	 cv_array[cv_idx].m = mute_arrary[mute_idx].m;
 
	 *idx = cv_idx;
 
	 return OK;
 }
 
 INT32 linux_mute_init(INT32 * idx, INT32 option)
 {
	 int mute_idx;
	 (VOID)option;
	 mute_idx = find_array_free((UINT8 *)mute_arrary,  \
							 MAX_MUTE_NUM, sizeof(MUTE_INFO));	  
	 while ((pthread_mutex_init(mute_arrary[mute_idx].m, &mxattr) != 0) &&
		 (pthread_mutex_init(mute_arrary[mute_idx].m, NULL) != 0)) {
		 usleep(10000);
	 }	  
 
	 *idx = mute_idx;
	 return OK;
 }
 
 INT32 linux_mute_lock(INT32 idx)
 {
	 INT32 rv;
	 if((idx < 0)||(idx >= MAX_MUTE_NUM))
		 return PLATFORM_LAYER_INVALIDE_MUTE_IDX;
	 
	 if(STATE_VALIDE != mute_arrary[idx].state)
	 {
		 return PLATFORM_LAYER_MUTE_NOT_EXIST;
	 }
	 if ((rv = pthread_mutex_lock(mute_arrary[idx].m)) != 0) 
	 {
		 return ERROR;
	 }	  
	 
	 return OK;
 }
 
 INT32 linux_mute_unlock(INT32 idx)
 {
	 INT32 rv;
	 if((idx < 0)||(idx >= MAX_MUTE_NUM))
		 return PLATFORM_LAYER_INVALIDE_MUTE_IDX;
	 
	 if(STATE_VALIDE != mute_arrary[idx].state)
	 {
		 return PLATFORM_LAYER_MUTE_NOT_EXIST;
	 }	  
	 if ((rv = pthread_mutex_unlock(mute_arrary[idx].m)) != 0) 
	 {
		 return ERROR;
	 } 
	 
	 return OK;
 }
 
 PF_OPERA opera = {
	 .mem_alloc = linux_alloc,
	 .mem_free = linux_free,
	 .get_pid = linux_get_pid,
	 .get_pthread_id = linux_get_pthread, 
	 .cv_wait = linux_pthread_cond_wait,
	 .cv_wake = linux_pthread_cond_wake,
	 .get_plat_clock = linux_get_cur_clock,
	 .cv_until = linux_cv_until,
	 .cv_init = linux_cv_init,
	 .mute_init = linux_mute_init,
	 .mute_lock = linux_mute_lock,
	 .mute_unlock = linux_mute_unlock,
	 .init = linux_pt_init,
 };
 
PLTF_OBJ linux_pt = {
	 (INT8*)"linux platform",
	 &opera,
	 NULL
 };

 PLTF_OBJ * pltf_obj_get(void)
 {
    return &linux_pt;
 }

 INT32 pltf_init(void)
 {
	 pt = pltf_obj_get();
	 return linux_pt_init();
 }

