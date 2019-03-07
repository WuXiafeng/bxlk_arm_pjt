/*hw.c*/
/*Add by Wuxiafeng 20181202 first version*/

#include "hw.h"
#include "bram.h"
#include "pf.h"

#define NETLINK_ISR_INFO     30
#define MSG_LEN            125
#define USER_PORT        100
#define MAX_PLOAD        125
#define SELF_PORT 100
#define ISR_MSG_UPDATE_PID  (0xDDEEFFCC) 
#define ISR_MSG_CLEAN_PID  (0xFFAA55EE)

typedef struct isr_msg{
	unsigned int head;
	unsigned int value1;
	unsigned int value2;
	unsigned long long value3;
}ISR_INFO;

typedef struct _user_msg_info
{
    struct nlmsghdr hdr;
    char  msg[MSG_LEN];
} user_msg_info;

INT32 socket_fd = -1;

int spi_init(void){

	return 0;
}

isr_func rx_func = NULL;
isr_func tx_func = NULL;

void sig_handler_rx(int signum)  
{  
    (void)signum;
	if(rx_func)
        rx_func();
}  

void sig_handler_tx(int signum)
{
    (void)signum;
    if(tx_func)
        tx_func();
}

int isr_init(void)
{  
	INT32 ret;
    user_msg_info u_info;
    socklen_t len;
    struct nlmsghdr *nlh = NULL;
    struct sockaddr_nl local_addr, remote_addr;
    ISR_INFO * infoptr;

#ifdef USE_MISC_DEVICE
    int fd;

	fd = open("/dev/mybeep",O_RDWR);  
    fcntl(fd, F_SETOWN, getpid());  
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | FASYNC);  
    printf("waiting key interrupt:\n");
#endif

#ifdef USE_CHAR_DEVICE
    int fd;

	unsigned char key_val;
	int ret;
	int Oflags;

	fd = open("/dev/buttons", O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
	}
 
	//set current process's PID to the driver that current fd point to, 
	//then the driver  will send SIG to current process
	fcntl(fd, F_SETOWN, getpid());
	
	//get the open method 
	Oflags = fcntl(fd, F_GETFL); 
 
    /* Set the fd with FASYNC option (support async notice ), when this option */
    /* was set,  file_operations->fasync function in driver will be called.*/ 
    /* Add the  file_operations->fasync function will call fasync_helper fuction,*/
    /* fasync_helper will init a fasync_struct struct which discripe the process 
       that the signal will send to */
	//PID (fasync_struct->fa_file->f_owner->pid)
	fcntl(fd, F_SETFL, Oflags | FASYNC);
#endif
    /* create net link socket */
    socket_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ISR_INFO);
    if(socket_fd < 0)
    {
        perror("create net link socket error\n");
        return -1;
    }

    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.nl_family = AF_NETLINK; //net link
    local_addr.nl_pid = SELF_PORT;  //port id
    local_addr.nl_groups = 0;
    if(bind(socket_fd,(struct sockaddr *)&local_addr, sizeof(local_addr)) != 0)
    {
        perror("bind() net linkg error\n");
        close(socket_fd);
        return -1;
    }

    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.nl_family = AF_NETLINK;
    remote_addr.nl_pid = 0; // to kernel 
    remote_addr.nl_groups = 0;

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PLOAD));
    memset(nlh, 0, sizeof(struct nlmsghdr));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PLOAD);
    nlh->nlmsg_flags = 0;
    nlh->nlmsg_type = 0;
    nlh->nlmsg_seq = 0;
    nlh->nlmsg_pid = local_addr.nl_pid; //self port

    infoptr = (ISR_INFO *)NLMSG_DATA(nlh);

    infoptr->head = ISR_MSG_UPDATE_PID;
    infoptr->value1 = PT_OPT.get_pid();
    
    ret = sendto(socket_fd, nlh, nlh->nlmsg_len, 0, \
            (struct sockaddr *)&remote_addr, sizeof(struct sockaddr_nl));
    if(!ret)
    {
        perror("sendto error\n");
        close(socket_fd);
        exit(-1);
    }
    printf("send msg to kernel failed !\n");

    memset(&u_info, 0, sizeof(u_info));
    len = sizeof(struct sockaddr_nl);
    ret = recvfrom(socket_fd, &u_info, sizeof(user_msg_info), 0, \
            (struct sockaddr *)&remote_addr, &len);
    if(!ret)
    {
        perror("recv form kernel error\n");
        close(socket_fd);
        exit(-1);
    }

    printf("from kernel:%s\n", u_info.msg);
    
    signal(SIGUSR1, sig_handler_rx);  
	signal(SIGUSR2, sig_handler_tx);	

    free((void *)nlh);
    return 0;
}

INT32 send_msg_to_k(ISR_INFO * buf)
{
    INT32 ret;
    user_msg_info u_info;
    socklen_t len;
    struct nlmsghdr *nlh = NULL;
    struct sockaddr_nl remote_addr;
    
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.nl_family = AF_NETLINK;
    remote_addr.nl_pid = 0; // to kernel 
    remote_addr.nl_groups = 0;

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PLOAD));
    memset(nlh, 0, sizeof(struct nlmsghdr));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PLOAD);
    nlh->nlmsg_flags = 0;
    nlh->nlmsg_type = 0;
    nlh->nlmsg_seq = 0;
    nlh->nlmsg_pid = SELF_PORT; //self port

    memcpy(NLMSG_DATA(nlh),buf,sizeof(ISR_INFO));
    
    ret = sendto(socket_fd, nlh, nlh->nlmsg_len, 0, \
            (struct sockaddr *)&remote_addr, sizeof(struct sockaddr_nl));
    if(!ret)
    {
        perror("sendto error\n");
        close(socket_fd);
        exit(-1);
    }
    printf("send msg to kernel failed!\n");

    memset(&u_info, 0, sizeof(u_info));
    len = sizeof(struct sockaddr_nl);
    ret = recvfrom(socket_fd, &u_info, sizeof(user_msg_info), 0, \
            (struct sockaddr *)&remote_addr, &len);
    if(!ret)
    {
        perror("recv form kernel error\n");
        close(socket_fd);
        exit(-1);
    }

    printf("from kernel:%s\n", u_info.msg);

    free((void *)nlh);    
    return 0;
}

int pt_hw_init(void)
{
	int ret;
	ret = isr_init();
	if(ret)
		return ret;

	ret = bram_init();
	if(ret)
		return ret;	

	ret = spi_init();
	if(ret)
		return ret;	
	
	return 0;
}

