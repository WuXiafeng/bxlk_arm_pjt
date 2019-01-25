/* pf.c */
/*  Add by Wuxf 20181202 First version */

#include "pf.h"
#include "array.h"
#include <semaphore.h>

#define MAX_CV_NUM 20
#define MAX_MUTE_NUM 40
#define MAX_SEMA_NUM 20
 
 typedef struct condition_info{
	 UINT32 state;
    pthread_cond_t c;
	 pthread_mutex_t *m;
 }CV_INFO;
 
 typedef struct mute_info{
	 UINT32 state;	  
    pthread_mutex_t m;
 }MUTE_INFO;
 
typedef struct sema_info{
    UINT32 state;
    sem_t sem;
}SEMA_INFO;
 CV_INFO cv_array[MAX_CV_NUM];
 MUTE_INFO mute_arrary[MAX_MUTE_NUM];
SEMA_INFO sema_arrary[MAX_SEMA_NUM];
 
 pthread_condattr_t  cvattr;
 pthread_mutexattr_t mxattr;
pthread_attr_t thread_attr;

 INT32 linux_pt_init(VOID)
 {
    INT32 ret;
    memset(cv_array, 0 , sizeof(cv_array));
    memset(mute_arrary, 0, sizeof(mute_arrary));
	 memset(sema_arrary, 0, sizeof(sema_arrary));
 
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
    ret = pthread_attr_init(&thread_attr);
    if(ret < 0){
        pthread_condattr_destroy(&cvattr);
        return ERROR;
    }    
	ret = pthread_attr_setdetachstate(&thread_attr, \
                                            PTHREAD_CREATE_DETACHED);
    if(ret < 0){
        pthread_condattr_destroy(&cvattr);
        return ERROR;
    }
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
	 
    pthread_cond_t *c = &cv_array[idx].c;
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
	 
    pthread_cond_t *c = &cv_array[idx].c;
 
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
	 
    pthread_cond_t *c = &cv_array[idx].c;
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
	 
	 while (pthread_cond_init(&cv_array[cv_idx].c, &cvattr) != 0) 
	 {
		 usleep(10000);
	 }
	 cv_array[cv_idx].m = &mute_arrary[mute_idx].m;
 
	 *idx = cv_idx;
 
	 return OK;
 }
 
 INT32 linux_mute_init(INT32 * idx, INT32 option)
 {
	 int mute_idx;
	 (VOID)option;
	 mute_idx = find_array_free((UINT8 *)mute_arrary,  \
							 MAX_MUTE_NUM, sizeof(MUTE_INFO));
     if(mute_idx < 0){
        *idx = mute_idx;
        return ERROR;
     }
	 while ((pthread_mutex_init(&mute_arrary[mute_idx].m, &mxattr) != 0) &&
		 (pthread_mutex_init(&mute_arrary[mute_idx].m, NULL) != 0)) {
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
	 if ((rv = pthread_mutex_lock(&mute_arrary[idx].m)) != 0) 
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
	 if ((rv = pthread_mutex_unlock(&mute_arrary[idx].m)) != 0) 
	 {
		 return ERROR;
	 } 
	 
	 return OK;
 }
 
INT32 linux_mute_close(INT32 idx)
{
    if((idx < 0)||(idx >= MAX_MUTE_NUM))
        return PLATFORM_LAYER_INVALIDE_MUTE_IDX;

    if(STATE_VALIDE != mute_arrary[idx].state)
    {
        return PLATFORM_LAYER_MUTE_NOT_EXIST;
    }

    (void) pthread_mutex_destroy(&mute_arrary[idx].m);

    mute_arrary[idx].state = OK;
    return OK;
}

INT32 linux_mute_try_lock(INT32 idx)
{
    INT32 rv;
    if((idx < 0)||(idx >= MAX_MUTE_NUM))
        return PLATFORM_LAYER_INVALIDE_MUTE_IDX;
    
    if(STATE_VALIDE != mute_arrary[idx].state)
    {
        return PLATFORM_LAYER_MUTE_NOT_EXIST;
    }    
    if ((rv = pthread_mutex_trylock(&mute_arrary[idx].m)) != 0) 
    {
        return ERROR;
    } 
    
    return OK;
}

INT32 linux_create_thread(ULONG * id, UINT32 option, 
                                VOID *(*routine) (VOID *), 
                                VOID * arg)
{
    INT32 ret;
    (void)option;

    ret = pthread_create(id,&thread_attr,routine,arg);

    if(ret)
        return ret;

    return OK;
}

VOID linux_sleep(INT32 ms)
{     
    usleep(ms*1000);
}

INT32 linux_sema_init(INT32 value)
{
    INT32 ret;
    INT32 sema_idx;
    sema_idx = find_array_free((UINT8 *)sema_arrary,  \
                            MAX_MUTE_NUM, sizeof(SEMA_INFO));
    if(sema_idx == ERROR)
        return PLATFORM_LAYER_SEMA_FULL;
    
    ret = sem_init(&sema_arrary[sema_idx].sem, 0, value);

    if(ret < 0){
        sema_arrary[sema_idx].state = 0;
        return ERROR;
    }
    return sema_idx;
}

INT32 linux_sema_close(INT32 idx)
{
    if((idx < 0)||(idx >= MAX_SEMA_NUM))
        return PLATFORM_LAYER_INVALID_PARA;

    if(STATE_VALIDE != sema_arrary[idx].state)
    {
        return PLATFORM_LAYER_INVALID_PARA;
    }

    (void)sem_destroy(&sema_arrary[idx].sem);

    sema_arrary[idx].state = OK;
    return OK;
}

INT32 linux_sema_wait(INT32 idx)
{
    INT32 rc;
    if((idx < 0)||(idx >= MAX_SEMA_NUM))
        return PLATFORM_LAYER_INVALID_PARA;

    if(STATE_VALIDE != sema_arrary[idx].state)
    {
        return PLATFORM_LAYER_INVALID_PARA;
    }
    
    do {
        rc = sem_wait(&sema_arrary[idx].sem);
    } while (rc < 0 && errno == EINTR);

    if (rc < 0) {
        return ERROR;
    }

    return OK;
}

INT32 linux_sema_post(INT32 idx)
{
    INT32 rc;
    if((idx < 0)||(idx >= MAX_SEMA_NUM))
        return PLATFORM_LAYER_INVALID_PARA;

    if(STATE_VALIDE != sema_arrary[idx].state)
    {
        return PLATFORM_LAYER_INVALID_PARA;
    }

    rc = sem_post(&sema_arrary[idx].sem);
    if (rc < 0) {
        return ERROR;
    }

    return OK;    
}

INT32 linux_socket(INT32 type)
{
    INT32 ret;

    ret = socket(PF_INET, type, 0);
    if (ret >= 0) {
        return ERROR;
    }

    return ret;
}

void linux_socket_set_nonblock(INT32 fd)
{
    INT32 f;
    f = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, f | O_NONBLOCK);
}

/* Rebind ip */
INT32 linux_socket_set_fast_reuse(INT32 fd)
{
    INT32 val = 1, ret;

    ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
                     (const INT8 *)&val, sizeof(val));
    return ret;
}

/* no nagle */
INT32 linux_socket_set_nodelay(INT32 fd)
{
    INT32 value = 1;
    return setsockopt(fd, SOL_SOCKET, TCP_NODELAY, &value, sizeof(value));
}

/* out of buffer in line */
INT32 linux_socket_set_oobinline(INT32 fd)
{
    INT32 value = 1;
    return setsockopt(fd, SOL_SOCKET, SO_OOBINLINE, &value, sizeof(value));
}

INT32 linux_socket_open(INT32 option, UINT8 ip[4], INT32 port, INT32 protocol)
{
    INT32 ret;
    INT32 fd;
    INT32 proto;
    UINT16 tmport;
    UINT32 tmpip;
    (void)option;

    tmport = (UINT16)port;
    tmpip = *((INT32 *)ip);
    
    struct sockaddr_in addr;

    if(protocol == TRANSPORT_TYPE_UDP)
    {
        proto = SOCK_DGRAM;
    }
    else if(protocol == TRANSPORT_TYPE_TCP)
    {
        proto = SOCK_STREAM;
    }
    else
    {
        return PLATFORM_LAYER_INVALID_PARA;
    }
    
    fd = linux_socket(proto);

    if(fd < 0)
        return ERROR;

    ret = linux_socket_set_fast_reuse(fd);

    if(ret < 0){
        close(fd);
        return ERROR;
    }
    
    memset(&addr,0,sizeof(struct sockaddr_in));
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(tmport);
    addr.sin_addr.s_addr = htonl(tmpip);

    ret = bind(fd,(struct sockaddr *)&addr, sizeof(addr));

    if (ret < 0) {
        close(fd);
        return ERROR;
    }    

    linux_socket_set_nonblock(fd);

    if(protocol == TRANSPORT_TYPE_TCP)
        linux_socket_set_nodelay(fd);

    return fd;
}

#define METHOD_ONE
INT32 linux_socket_send(UINT8 *buf, INT32 len, INT32 option, INT32 socket)
{
#ifdef METHOD_ONE
    INT32 ret;
    INT32 err;

    for(;;)
    {
        ret = send(socket,buf,len,option);
        if(ret < 0)
        {
            err = errno;
            if((err == EINTR))
            {
                continue;
            }
            else if((err == EWOULDBLOCK) || (err == EAGAIN))
            {
                /* no buffer, just wait */
                usleep(1000);
                continue;
            }
            else
            {
                return ERROR;
            }
        }
        else
        {
            return ret;
        }
    }
#else
    return send(socket,buf,len,option);
#endif
}

INT32 linux_socket_recv(UINT8 *buf, INT32 len, INT32 option, INT32 socket)
{
#ifdef METHOD_ONE
    INT32 ret;
    INT32 err;

    for(;;)
    {
        ret = recv(socket,buf,len,option);
        if(ret < 0)
        {
            err = errno;
            if((err == EINTR))
            {
                continue;
            }
            else if((err == EWOULDBLOCK) || (err == EAGAIN))
            {
                /* no data */
                return OK;
            }
            else
            {
                return ERROR;
            }
        }
        else
        {
            return ret;
        }
    }
#else
    return recv(socket,buf,len,option);
#endif    
    
}

INT32 linux_socket_sendto(UINT8 *buf, INT32 len, INT32 option,
                                 INT32 socket, UINT8 ip[4], INT32 port)
{
    UINT16 tmport;
    UINT32 tmpip;
    struct sockaddr_in remote_addr;
    
    tmport = (UINT16)port;
    tmpip = *((UINT32 *)ip);
    memset(&remote_addr,0,sizeof(struct sockaddr_in));
    
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(tmport);
    remote_addr.sin_addr.s_addr = htonl(tmpip);

#ifdef METHOD_ONE
    INT32 ret;
    INT32 err;

    for(;;)
    {
        ret = sendto(socket,buf,len,option, \
                        (struct sockaddr *)&remote_addr,sizeof(remote_addr));
        if(ret < 0)
        {
            err = errno;
            if((err == EINTR))
            {
                continue;
            }
            else if((err == EWOULDBLOCK) || (err == EAGAIN))
            {
                /* no buffer, just wait */
                usleep(1000);
                continue;
            }
            else
            {
                return ERROR;
            }
        }
        else
        {
            return ret;
        }
    }
#else
    return sendto(socket,buf,len,option,  \
                       (struct sockaddr *)&remote_addr,sizeof(remote_addr));
#endif        
}

INT32 linux_socket_recvfrom(UINT8 *buf, INT32 len, INT32 option,
                                    INT32 socket, UINT8 ip[4], INT16* port)
{
    INT32 ret;
    socklen_t length;
    struct sockaddr_in remote_addr;

    length = sizeof(remote_addr);
#ifdef METHOD_ONE
    INT32 err;
    
    for(;;)
    {
        ret = recvfrom(socket,buf,len,option, \
                          (struct sockaddr *)&remote_addr,&length);
        if(ret < 0)
        {
            err = errno;
            if((err == EINTR))
            {
                continue;
            }
            else if((err == EWOULDBLOCK) || (err == EAGAIN))
            {
                /* no data */
                return OK;
            }
            else
            {
                return ERROR;
            }
        }
        else
        {
            break;
        }
    }
#else
    ret = recvfrom(socket,buf,len,option, \
                    (struct sockaddr *)&remote_addr,&length);
    if(ret < 0)
        return ret;
#endif

    if(ip)
        *((UINT32 *)ip) = ntohl(remote_addr.sin_addr.s_addr);
    if(port)
        *port = ntohs(remote_addr.sin_port);

    return ret;
}

INT32 linux_socket_listen(INT32 socket)
{
    return listen(socket, 5);
}

INT32 linux_socket_accept(INT32 socket, INT32 option, UINT8 ip[4], INT32* port)
{
    struct sockaddr_in remote_addr;
    INT32 remote_socket;
    socklen_t addrlen;
    INT32 ret;
    addrlen = sizeof(remote_addr);
    memset(&remote_addr,0,sizeof(remote_addr));
    (void)option;

#ifdef METHOD_ONE
    INT32 err;

    for(;;)
    {
        remote_socket = accept(socket, \
                            (struct sockaddr *)&remote_addr, &addrlen);
        if(remote_socket < 0)
        {
            err = errno;
            if((err == EINTR))
            {
                continue;
            }
            else if((err == EWOULDBLOCK) || (err == EAGAIN))
            {
                return PLATFORM_LAYER_TCP_SOCK_NO_CONN;
            }
            else
            {
                return ERROR;
            }                
        }
    }
#else
    remote_socket = accept(socket, (struct sockaddr *)&remote_addr, &addrlen);
    if(remote_socket < 0)
        return remote_socket;
#endif

    linux_socket_set_nonblock(remote_socket);
    ret = linux_socket_set_nodelay(remote_socket);
    if(ret)
    {
        close(remote_socket);
        return ERROR;
    }
    
    if(ip)
        *((UINT32 *)ip) = ntohl(remote_addr.sin_addr.s_addr);
    if(port)
        *port = ntohs(remote_addr.sin_port);
    return remote_socket;
}

INT32 linux_socket_connect(INT32 socket, INT32 option, 
                                   UINT8 ip[4], INT32 port)
{
    (void)option;
    UINT16 tmport;
    UINT32 tmpip;
    struct sockaddr_in saddr;

    tmport = (UINT16)port;
    tmpip = *((UINT32 *)ip);
    memset(&saddr,0,sizeof(struct sockaddr_in));

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(tmport);
    saddr.sin_addr.s_addr = htonl(tmpip);

    /*
        TODO:
        Method 1 ,the function will blocked here!  no posix return 
                       value can be seen in the top levels;
        Method 2 ,do not block the function, top level analysis the return value and do sync
                        operation use AIO;
      */
#ifdef METHOD_ONE
    INT32 err;
    INT32 ret;

    for(;;)
    {
        /* will block */
        ret = connect(socket, (struct sockaddr *)&saddr, sizeof(saddr));
        if(ret < 0)
        {
            err = errno;
            if((err == EINTR)||(err == EWOULDBLOCK))
            {
                continue;
            }
            else if(err == EINPROGRESS)
            {
                return OK;
            }
            else
            {
                return ERROR;
            }
        }
        else
        {
            return OK;
        }
    }
    return ERROR;
#else
    return connect(socket, (struct sockaddr *)&saddr, sizeof(saddr));
#endif

}

INT32 linux_socket_close(INT32 socket)
{
    return close(socket);
}

INT32 linux_get_core_num(VOID)
{
#ifdef _SC_NPROCESSORS_ONLN
	return (sysconf(_SC_NPROCESSORS_ONLN));
#else
	return (1);
#endif    
}

INT32 linux_sig_block(INT32 sig_num)
{
	sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, sig_num);
	(void) pthread_sigmask(SIG_BLOCK, &set, NULL);
    return OK;
}

INT32 linux_do_select(INT32* fd, UINT32 operation, 
                           INT32 second,INT32 * ready_fd_list)
{
    fd_set tmpfds,rxfds,txfds,errfds;
    struct timeval tv;
    INT32 ret, i;
    INT32 fdd = MAX_FD;

    FD_ZERO(&tmpfds);
    for(i=0; fd[i] != -1;i++)
    {
        FD_SET(fd[i], &tmpfds);
        if(fdd > fd[i])
            fdd = fd[i];
    }
    
    tv.tv_sec = second;
    tv.tv_usec = 0;

    if(operation & DO_SELECT_OPER_RX)
        memcpy(&rxfds,&tmpfds,sizeof(fd_set));

    if(operation & DO_SELECT_OPER_TX)
        memcpy(&txfds,&tmpfds,sizeof(fd_set));

    if(operation & DO_SELECT_OPER_ERR)
        memcpy(&errfds,&tmpfds,sizeof(fd_set));
    
    ret = select(fdd + 1, &rxfds,&txfds,&errfds,&tv);

    if(ret < 0)
    {
        return ERROR;
    }
    else if(ret == 0)
    {
        return PLATFORM_LAYER_SELECT_TIME_OUT;
    }
    else
    {
        /* update rx ready_fd_list */
        if((ready_fd_list)&&(operation & DO_SELECT_OPER_RX))
        {
            for(;i>0;i--)
            {
                FD_ISSET(fd[i-1],&rxfds);
                *ready_fd_list++ = fd[i-1];
            }
        }
        
        return ret;
    }
}

INT32 linux_get_ava_len(INT32 fd)
{
    INT32 ava_len;
    INT32 ret;

    ret = ioctl(fd,FIONREAD,&ava_len);

    if(ret < 0)
        return 0;

    return ava_len;
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
    .mute_close = linux_mute_close,
    .mute_try_lock = linux_mute_try_lock,
	 .init = linux_pt_init,
    .create_thread = linux_create_thread,
    .sleep = linux_sleep,
    .sema_init = linux_sema_init,
    .sema_close = linux_sema_close,
    .sema_wait = linux_sema_wait,
    .sema_post = linux_sema_post,
    .socket_open = linux_socket_open,
    .socket_send = linux_socket_send,
    .socket_recv = linux_socket_recv,
    .sock_sendto = linux_socket_sendto,
    .sock_recvfrom = linux_socket_recvfrom,
    .sock_listen = linux_socket_listen,
    .sock_accept = linux_socket_accept,
    .sock_connect = linux_socket_connect,
    .sock_close = linux_socket_close,
    .get_core_num = linux_get_core_num,
    .sig_block = linux_sig_block,
    .do_select = linux_do_select,
    .get_available_len = linux_get_ava_len,
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

 PLTF_OBJ * pt;

 INT32 pltf_init(void)
 {
	 pt = pltf_obj_get();
	 return PT_OPT.init();
 }

