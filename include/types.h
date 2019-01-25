/*types.h*/

#ifndef __types
#define __types

#ifdef __cplusplus
extern "C" {
#endif

//Add gnu source, so that related MACROs are defined
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include <elf.h>
#include <dlfcn.h>
#include <sys/msg.h>  
#include <sys/un.h>  
#include <signal.h>
#include <sys/ioctl.h>
#include <ucontext.h>
#include <dirent.h>
#include <sys/mman.h>
#include <linux/netlink.h>
#include <ctype.h>
#include <arpa/inet.h>

#include "glb_conf.h"

typedef enum device_type{
    LINUX_ETH_DEMO,
    VXWORK_ETH_DEMO,
    BARE_ETH_CMC_DEMO,
    PSCU,
    ZAU,
    CIU,
    CMC,
    NUM_OF_DEV
}DEV_TYPE;

typedef enum config_type{
	DYNAMIC_MODE,
	STATIC_MODE,
	AUTO_GET_MODE
}COFIG_TP;

typedef enum link_type{
    ETH_SOCKET,
    ETH_RAW,
    RS429,
    RS485
}LINK_TYPE;

#define MIDD_NAME_MAX_NUM (24)
#define INVALID_ID (-1)

typedef signed char     INT8;
typedef signed short    INT16;
typedef signed int      INT32;
typedef signed long long INT64;

typedef unsigned char   UINT8;
typedef unsigned short  UINT16;
typedef unsigned int    UINT32;
typedef unsigned long long UINT64;

typedef unsigned char   UCHAR;
typedef unsigned short  USHORT;
typedef unsigned int    UINT;
typedef unsigned long   ULONG;
typedef int             ARGINT;
typedef void            VOID;
typedef int             STATUS;

#ifndef OK
#define OK          0
#endif

#ifndef ERROR
#define ERROR       (-1)
#endif

#ifndef NULL
#define NULL        (void *)0
#endif

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

#define max(x, y)	(((x) < (y)) ? (y) : (x))
#define min(x, y)	(((x) < (y)) ? (x) : (y))
#define NUM_ELEMENTS(x) ((unsigned) (sizeof(x) / sizeof((x)[0])))
#define DATA_IN_RANGE(x, min, max) (((x) >= (min)) && ((x) < (max)))
#define ROUND(a,b)		(((a) + (b) - 1) & ~((b) - 1))
#define ISR_NUM_USB_RX 0x238292a2Ul
#define ISR_NUM_USB_TX 0Xaab39373UL

typedef UINT64 pttime;
typedef enum {false, true} bool;
typedef enum comm_err_type{
    COMM_ERR_NULL_PTR = -800,
    COMM_ERR_AGAIN,
    COMM_AIO_PLATFORM_NOT_INIT,
	COMM_AIO_USER_CANCEL,
	COMM_AIO_TIME_OUT,
	COMM_LAYER_PT_NOT_INIT,
	COMM_LAYER_MUTE_LOCK_ERROR,
	COMM_LAYER_MUTE_INIT_FAILD,
	COMM_PLATFORM_NOT_INIT,
	WORK_THREAD_MAX,
	WORK_THREAD_INVALID_PARA,
	COMM_SEMA_ERROR,
	COMM_MEM_ALLOC_ERR
}COMM_ERROR;

typedef enum API_FUNCTION_ERROR{
    API_LAYER_ERROR_BASE = -900,
	API_LAYER_GET_OPS_HANDLE_ERROR	
}API_ERROR_TYPE;

typedef enum CORE_FUNCTION_ERROR{
    CORE_LAYER_ERROR_BASE = -1000,
	CORE_LAYER_GET_PLATFORM_OBJ_ERROR,
	CORE_LAYER_PLATFORM_INIT_ERROR,
	CORE_LAYER_INIT_NODE_TABLE_ERROR,
	CORE_LAYER_INIT_SOCKET_ERROR,
	CORE_QUEUE_MAX_LEN
}CORE_ERROR_TYPE;

typedef enum ROUTE_FUNCTION_ERROR{
    ROUTE_LAYER_ERROR_BASE = -1100,
	ROUTER_GET_IP_ERROR,
	ROUTER_GET_DEV_ID_ERROR,
	ROUTER_MALLOC_MEM_ERROR,
	ROUTER_ADD_LINK_IDX_ERROR,
	ROUTER_CREAT_RX_QUEUE_ERROR,
	ROUTER_CREAT_TX_QUEUE_ERROR
}ROUTE_ERROR_TYPE;

typedef enum LINK_FUNCTION_ERROR{
    LINK_LAYER_ERROR_BASE = -1200
}LINK_ERROR_TYPE;

typedef enum NODE_FUNCTION_ERROR{
    NODE_MODULE_ERROR_BASE = -1300,
	NODE_SET_NODE_NULL_ERROR,
	NODE_QUERY_NODE_NULL_ERROR,
	NODE_SET_MODE_ERROR,
	NODE_CREAT_NODE_NULL_ERROR,
	NODE_CREAT_NODE_ALLOC_MEM_ERROR,
	NODE_CREAT_NODE_ALREADY_EXIST_ERROR,
	NODE_ADD_NODE_ERROR,
	NODE_DELETE_NODE_NULL_ERROR,
	NODE_QUERY_PARA_NULL_ERROR,
	NODE_MODULE_DEL_PARA_INVALID_ERROR,
	NODE_MODULE_DEL_NO_FIND_NODE_ERROR,
	NODE_MODULE_FIND_PARA_INVALID_ERROR,
	NODE_MODULE_NO_FIND_NODE_ERROR,
	NODE_MODULE_GET_OPS_ERROR,
	NODE_MODULE_NEW_NODE_NULL_ERROR,
	NODE_MODULE_OPS_INIT_ERROR,
	NODE_MODULE_ADD_IDX_ERROR
}NODE_ERROR_TYPE;

typedef enum QUEUE_FUNCTION_ERROR{
	QUEUE_MODULE_ERROR_BASE = -1400,
    QUEUE_MODULE_MAX_QUEUE,
    QUEUE_MODULE_QUEUE_NOT_EXIST,
    QUEUE_MODULE_QUEUE_FULL
}QUEUE_ERROR_TYPE;

typedef enum XML_FUNCTION_ERROR{
	XML_PRASE_MODULE_ERROR_BASE = -1500
}XML_ERROR_TYPE;

typedef enum PLATFORM_FUNCTION_ERROR{
	PLATFORM_LAYER_ERROR_BASE = -1600,
    PLATFORM_LAYER_INVALIDE_CV_IDX,
    PLATFORM_LAYER_CV_NOT_EXIST,
    LLATFORM_LAYER_TIME_OUT,
    PLATFORM_LAYER_INVALIDE_MUTE_IDX,
    PLATFORM_LAYER_MUTE_NOT_EXIST,
    PLATFORM_LAYER_SEMA_FULL,
    PLATFORM_LAYER_INVALID_PARA,
    PLATFORM_LAYER_TCP_SOCK_NO_CONN,
    PLATFORM_LAYER_SOCK_NO_DATA,
    PLATFORM_LAYER_SELECT_TIME_OUT,
}PLATFORM_ERROR_TYPE;

typedef enum{
    TRANSPORT_TYPE_TCP = 500,
    TRANSPORT_TYPE_UDP,
    NUM_TRANSPORT_TYPE
}TRANSPORT_TYPE;

typedef VOID (*isr_func)(VOID);

#ifndef NELEMENTS
#define NELEMENTS(array)        /* number of elements in an array */ \
        (sizeof (array) / sizeof ((array) [0]))
#endif

#ifdef __cplusplus
}
#endif

#endif /* __types */


