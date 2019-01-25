/* pf.h */
/*  Add by Wuxf 20181202 First version */

#ifndef __PF_H__
#define __PF_H__

#include "types.h"

#define THREAD_NO_JOIN 0x00000010ul
#define ALL_IP_ADDR ((UINT32)0x00000000)
#define TASK_PRIO_NONE 0xA0000001
#define TASK_PRIO_HIGH 0xA0000002
#define TASK_PRIO_MID 0xA0000003
#define TASK_PRIO_LOW 0xA0000004

#define DO_SELECT_OPER_RX  (0x00000010ul)
#define DO_SELECT_OPER_TX  (0x00000020ul)
#define DO_SELECT_OPER_ERR (0x00000040ul)

#define MAX_FD (1024)

typedef struct pf_opera{
    INT32 (*init)(VOID);
    /* print */
    INT32 (*print)(INT8 *tmplate,...);
    
    /* buffer alloc */
    INT32 (* mem_alloc)(INT32 size, VOID **ptr, INT32 option);
    VOID (* mem_free)(VOID * ptr);

    /* interface manage */
    INT32 (*get_if_info)(INT32 option, VOID** if_list, INT32* num);

    /* schedule */
    INT32 (*create_thread)(unsigned long * id, UINT32 option, VOID* (*routine) (VOID *), VOID * arg);
    INT32 (*timer_isr_reg)(VOID* option, \
						VOID *(*isr_func) (VOID *), VOID * arg);
    INT32 (*get_pid)(VOID);
    unsigned long (*get_pthread_id)(VOID);
    INT32 (*join)(unsigned long id);
    VOID (*sleep)(INT32 time);
    /* select */
    INT32 (*do_select)(INT32* fd, UINT32 operation, \
                        INT32 second,INT32 * ready_fd_list);

    /* semaphore */
    INT32 (*sema_init)(INT32 value);
    INT32 (*sema_close)(INT32 idx);
    INT32 (*sema_wait)(INT32 idx);
    INT32 (*sema_post)(INT32 idx);

    /* mutelock */
    INT32 (*mute_init)(INT32* idx, INT32 option);
    INT32 (*mute_close)(INT32 idx);
    INT32 (*mute_lock)(INT32 idx);
    INT32 (*mute_unlock)(INT32 idx);
    INT32 (*mute_try_lock)(INT32 idx);

    /* pthread condition */
    INT32 (*cv_wait)(INT32 idx);
    INT32 (*cv_wake)(INT32 idx);
    INT32 (*cv_until)(INT32 idx, pttime time);
    INT32 (*cv_init)(INT32* idx, INT32 mute_idx);

    /*file*/
    INT32 (*open)(INT8 * path, INT32 option);
    INT32 (*close)(INT32 fd);
    INT32 (*read)(INT32 fd, INT8* buf, INT32 size);
    INT32 (*write)(INT32 fd, INT8* buf, INT32 len);
    INT32 (*lseek)(INT32 fd, INT32 off, INT32 whence);
    INT32 (*get_available_len)(INT32 fd);

    /* timing */
    INT32 (*get_timestamp)(UINT64 * stamp);
    INT32 (*get_cycle)(UINT64 * cycle);
    pttime (*get_plat_clock)(VOID);
    INT32 (*get_core_num)(VOID);

    /* package handle */
    INT32 (*pkt_send)(UINT8 *buf, INT32 len, INT32 option, VOID* extra);
    INT32 (*pkt_state)(INT32 option, VOID* extra);
    INT32 (*pkt_recv)(UINT8 *buf, INT32 len, INT32 option, VOID* extra);
    INT32 (*pkt_recv_cb_register)(INT32 (*callback)(UINT8* msg, INT32 len), \
                                INT32 option);
    INT32 (*pkt_recv_isr_registerpkt_recv_cb_register)(VOID *(*isr_func) \
                                  (VOID* para, UINT8 * data, \
                                   UINT32 ip, INT16 port), \
                                    VOID* arg, INT32 option);

    /* socket */
    INT32 (*socket_open)(INT32 option, UINT8 ip[4], INT32 port, INT32 protocol);
    INT32 (*socket_send)(UINT8 *buf, INT32 len, INT32 option, INT32 socket);
    INT32 (*socket_recv)(UINT8 *buf, INT32 len, INT32 option, INT32 socket);
    INT32 (*sock_sendto)(UINT8 *buf, INT32 len, INT32 option, \
                            INT32 socket, UINT8 ip[4], INT32 port);
    INT32 (*sock_recvfrom)(UINT8 *buf, INT32 len, INT32 option, \
                                    INT32 socket, UINT8 ip[4], INT16* port);
    INT32 (*sock_listen)(INT32 socket);
    INT32 (*sock_accept)(INT32 socket, INT32 option, UINT8 ip[4], INT32* port);
    INT32 (*sock_connect)(INT32 socket, INT32 option, UINT8 ip[4], INT32 port);
    INT32 (*sock_close)(INT32 socket);

    /*signal*/
    INT32 (*sig_block)(INT32 sig_num);

    /* ISR operation*/
    INT32 (*isr_lock)(VOID);
    INT32 (*isr_unlock)(VOID);
    INT32 (*pkt_recv_isr_register)(VOID * (*func)(VOID * para), VOID * para, \
                                    INT32 option);
}PF_OPERA;

/* Platform object */
typedef struct pltf_obj{
    const INT8* pt_name;
    PF_OPERA* opra;
    VOID* priv;
}PLTF_OBJ;

INT32 pltf_init(void);

extern PLTF_OBJ * pt;
#define PT_OPT (*(PF_OPERA *)(pt->opra))

#define PT_INIT(x)

#endif /* __PF__ */


