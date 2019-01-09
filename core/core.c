/*core.c*/
/*Add by Wuxiafeng 20181202 first version */

#include "core.h"
#include "comm_isr.h"
#include "usb_rx_tx.h"
#include "msg_handle.h"

INT32 rx_work_thread_id;
INT32 tx_work_thread_id;

#define DATA_QUEUE_TYPE  0x22389483uL

int core_init(void)
{
    int ret;
    
	/*create work thread for bram handle */
    ret = new_queue(DATA_QUEUE_TYPE, NULL);

    if(ret < 0)
    {
        return ret;
    }

    rx_work_thread_id = create_work_thread(ret, 
                                            rx_msg_handle,
                                            NULL,
                                            1,
                                            (INT8 *)"rx work thread");

    if(rx_work_thread_id < 0)
    {
        remove_queue(ret);
        return rx_work_thread_id;
    }    

    ret = new_queue(DATA_QUEUE_TYPE,NULL);

    if(ret < 0)
    {
        return ret;
    }

    tx_work_thread_id = create_work_thread(ret, 
                                            start_usb_tx,
                                            NULL,
                                            1,
                                            (INT8 *)"tx work thread");

    if(tx_work_thread_id < 0)
    {
        remove_queue(ret);
        return tx_work_thread_id;
    }  

	/* regist the ISR */
    /* ISR handle could not re-entry*/
    ret = comm_ISR_register(usb_rx_handle, ISR_NUM_USB_RX);
    if(ret)
        return ret;
        
    ret = comm_ISR_register(usb_tx_done, ISR_NUM_USB_TX);
    if(ret)
        return ret;
    
	return 0;
}

INT32 session_fd_lock = -1;
INT32 usb_socket_state_lock = -1;

pthread_t core_state_mana_thread_id;
pthread_attr_t core_state_mana_thread_attr;

INT32 usb_act_flag = -1;

#define USB_IS_ON (usb_act_flag >= 0)
#define USB_DOG_EAT (usb_act_flag--)
#define USB_DOG_FEED (usb_act_flag = USB_CHAN_TIMEOUT_DURA)

VOID core_feed_usb(VOID)
{
    PT_OPT.mute_lock(usb_socket_state_lock);
    USB_DOG_FEED;
    PT_OPT.mute_unlock(usb_socket_state_lock);
}

void *core_state_manager(void * arg)
{
    (void)arg;
    for(;;)
    {
        usleep(1000000);
        PT_OPT.mute_lock(usb_socket_state_lock);
        if(USB_IS_ON)
        {
            USB_DOG_EAT;
        }
        PT_OPT.mute_unlock(usb_socket_state_lock);
    }
}

INT32 core_state_manager_init(VOID)
{
    INT32 ret;

    ret = PT_OPT.mute_init(&usb_socket_state_lock,0);
    if(ret < 0)
    {
        printf("mute init failed!\n");
        return ERROR;        
    }
    
    ret = pthread_attr_init(&core_state_mana_thread_attr);
    if(ret < 0){
        printf("Attrbute init failed!\n");
        return ERROR;
    }    
	ret = pthread_attr_setdetachstate(&core_state_mana_thread_attr, \
                                            PTHREAD_CREATE_DETACHED);
    if(ret < 0){
        printf("Set detach state failed!\n");
        return ERROR;
    }

    ret = pthread_create(&core_state_mana_thread_id, \
                        &core_state_mana_thread_attr, \
                        core_state_manager, NULL);
    if(ret < 0){
        printf("Create thread failed!\n");    
        return ERROR;
    }
    pthread_setname_np(core_state_mana_thread_id,"core state manager");

    return OK;
}

INT32 core_socket_fd = -1;
struct sockaddr_in sock_addr; 

INT32 session_fd = -1;
INT8 recv_buf[2048];

INT32 data_send_timeout(INT32 sockfd,INT8 *data,
                                        INT32 length,INT32 sec)
{
    INT32 sendbytes;
    INT32 total=0;
    fd_set rdfds;
    struct timeval tv;
    FD_ZERO(&rdfds);
    FD_SET(sockfd, &rdfds);
    tv.tv_sec = sec;
    tv.tv_usec = 0;
    INT32 ret;

    while(total<length){

        ret = select(sockfd + 1, NULL, &rdfds, NULL, &tv);
        if(ret < 0) 
        {
            return -1;
        }else if(ret==0)
        {
            return -1;
        }else{
            if(FD_ISSET(sockfd, &rdfds)){
                if((sendbytes=send(sockfd,data+total,length-total,0))==-1){
		                printf("data send timeout\n");
                    return -1;
                }
                total+=sendbytes;                
            }
        }
    }
    return total;
}

int32_t data_recv_length_timeout(int32_t sockfd,int8_t *data, int32_t sec)
{
    int32_t recvbytes;
    int32_t ret;    
    fd_set rdfds;
    struct timeval tv;
    FD_ZERO(&rdfds);
    FD_SET(sockfd, &rdfds);

    if (sec ==0)
    {
        tv.tv_sec = 5;
    }
    else
    {
        tv.tv_sec = sec;
    }
    tv.tv_usec = 0;

    {
        ret = select(sockfd + 1, &rdfds, NULL, NULL, &tv);
        if(ret < 0) 
        {
            printf("IN data_recv_length_timeout select error :  <<< %s\n", strerror(errno));
            return -1;
        }else if(ret==0)
        {
            //printf("IN data_recv_length_timeout select NO DATA\n");
            return 0;
        }else{
            if(FD_ISSET(sockfd, &rdfds))
            {
                //while(1)
                {
                    recvbytes=recv(sockfd,data,1024,0);
                    
                    //printf("recv msgf:%s, recvbytes:%d\n", data, recvbytes);
                    
                    if(recvbytes < 0)
                    {
                        printf("IN data_recv_length_timeout recv error :  <<< %s\n", strerror(errno));
                        return -1;                    
                    }
                    else if(recvbytes==0)
                    {
                        //printf("IN data_recv_length_timeout recv finish  ! \n");
                        return 0;
                    }
                
                }
            }
        }
    }
    return recvbytes;
}


VOID exit_session(VOID)
{
    PT_OPT.mute_lock(session_fd_lock);    
    if(session_fd >= 0)
    {
        close(session_fd);  
        session_fd = -1;
    }
    PT_OPT.mute_unlock(session_fd_lock);
}

void start_socket_session_control(void)
{
    INT32 ret;

    ret = PT_OPT.mute_init(&session_fd_lock,0);
    if(ret < 0)
    {
        printf("mute init failed!\n");
        return;        
    }

    core_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(core_socket_fd < 0)
    {
        fprintf(stderr,"start_socket_session_control create Socket failed,"
            " exit! Error is %d.\n",errno);
        exit(1);
    }

    memset(&sock_addr,0,sizeof(struct sockaddr_in));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(SOCKET_PORT);
    sock_addr.sin_addr.s_addr = inet_addr(SOCKET_SER_IP_ADDR);

    ret = bind(core_socket_fd,(struct sockaddr*)&sock_addr,  \
                        sizeof(struct sockaddr_in));

    if(ret<0)
    {  
        fprintf(stderr,"start_socket_session_control cannot bind socket,"
                            " error is %d\n",errno);
        exit(1);  
    }

    ret = listen(core_socket_fd,5);  
    if(ret<0)
    {  
        fprintf(stderr,"start_socket_session_control cannot listen,"
                            " error is %d\n",errno);  
        exit(1);  
    }  

	/* todo: add socket session handler */
    struct sockaddr_in client_addr;
    socklen_t len;
    
	for(;;)
	{
		usleep(1000000);        
WATI_SESSION:        
        memset(&client_addr,0,sizeof(struct sockaddr_in));
        len = sizeof(client_addr);
        session_fd=accept(core_socket_fd,(struct sockaddr*)&client_addr,&len);

        if(session_fd<0)
        {  
            ret = errno;
            fprintf(stderr,"cannot accept requst, error is %d,"
                            " sleep 10 seconds!\n",ret);  

            usleep(10000000);
            goto WATI_SESSION;  
        } 

        INT32 opt = 1;
        //disable nagle algorithm of TCP,  
        //or TCP will melt two package together automaticlly.
        ret = setsockopt(session_fd, SOL_SOCKET, TCP_NODELAY,\
                                                    &opt, sizeof(opt));

        INT32 keepAlive = 1;
        INT32 keepIdle = 2;
        INT32 keepInterval = 2;
        INT32 keepCount = 2;

        ret += setsockopt(session_fd, SOL_SOCKET, SO_KEEPALIVE, \
                                                &keepAlive,sizeof(keepAlive));
        ret += setsockopt(session_fd, SOL_TCP, TCP_KEEPIDLE,  \
                                                &keepIdle,sizeof(keepIdle));
        ret += setsockopt(session_fd, SOL_TCP, TCP_KEEPINTVL,  \
                                          &keepInterval,sizeof(keepInterval));
        ret += setsockopt(session_fd, SOL_TCP, TCP_KEEPCNT,  \
                                                &keepCount,sizeof(keepCount));

        if(ret < 0)
        {
            fprintf(stderr, "Set socket attribute failed! errno is %d\n",errno);
            exit(1);
        }

        /*send wellcome*/
        ret = data_send_timeout(session_fd,(INT8*)SOCKET_WELLCOME_MSG,\
                              sizeof(SOCKET_WELLCOME_MSG),5); 

        if(ret < 0)
        {
            exit_session();
            continue;
        }

        INT32 len;
        INT8 * buf;

        for(;;)
        {
            memset(recv_buf,0,sizeof(recv_buf));
            ret = data_recv_length_timeout(session_fd, recv_buf, 3);

            if(ret == 0)
            {
                continue;
            }
            else if(ret > 0)
            {
                len = strlen((char *)recv_buf);
                PT_OPT.mem_alloc(len, (VOID **)&buf, 0);
                if(!buf)
                {
                    fprintf(stderr, "receive msg,alloc mem failed!"
                                            " errno is %d\n",errno);
                    exit_session();
                    break;
                }
                add_work_to_thread(rx_work_thread_id, (VOID*)buf, len);
            }
            else if(ret < 0)
            {
                    fprintf(stderr, "receive msg failed!"
                                            " errno is %d\n",errno);
                    exit_session();
                    break;                
            }
        }
	}
}

core_send_filter_func core_sock_msg_filter = NULL;

VOID regst_socket_msg_filter(core_send_filter_func func)
{
    core_sock_msg_filter = func;
}

VOID unregst_socket_msg_filter(void)
{
    core_sock_msg_filter = NULL;
}

VOID core_msg_send(INT8 *buf, INT32 len)
{
    INT32 ret;

    /* usb send first */
    PT_OPT.mute_lock(usb_socket_state_lock);
    ret = USB_IS_ON;
    PT_OPT.mute_unlock(usb_socket_state_lock); 

    if(ret)
    {
        usb_tx_send(buf,len);
        return;
    }

    /* filter */
    if(core_sock_msg_filter){
        if(core_sock_msg_filter(buf,len))
        {
            PT_OPT.mem_free(buf);
            return;
        }
    }
    
    PT_OPT.mute_lock(session_fd_lock);
    ret = (session_fd >= 0);
    PT_OPT.mute_unlock(session_fd_lock);
    
    if(ret)
    {
        data_send_timeout(session_fd,buf,len,5);
        return;
    }
    else
    {
        PT_OPT.mem_free(buf);
        return;
    }
}


