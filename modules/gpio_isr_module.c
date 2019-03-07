/*gpio_isr_module.c*/
/*Add by Wuxiafeng */
/* method 1 , use netlink + send_sig */
/* method 2, use misc_register regist misc dev +  fasync_helper & kill_fasync */
/* method 3, use chardev  +   fasync_helper & kill_fasync */

#include <linux/kernel.h>            /* Kernel... */
#include <linux/module.h>            /* ...Modul stuff*/
#include <asm/uaccess.h>            /* copy from userspace */
#include <asm/io.h>                    /* for IO read/write functions */
#include <linux/proc_fs.h>            /* for proc fs-functions */
#include <linux/seq_file.h>            /* for sequence file operations */
#include <linux/platform_device.h>    /* for platform driver functions */
#include <linux/slab.h>                /* for  kalloc and kfree */
#include <linux/interrupt.h>        /* interrupt stuff */
#include <linux/cdev.h> /* provides cdev struct, how the kernel represents char devices internally */
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/irq.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_device.h>
#include <linux/types.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/pid.h>
#include <linux/errno.h>   
#include <linux/miscdevice.h>    
#include <linux/gpio.h>  
#include <asm-generic/siginfo.h>   
#include <asm/signal.h>  
#include <linux/timer.h>  
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,10,0)
#include <linux/sched/signal.h>		/* For send_sig(), same_thread_group(), etc. */
#endif

#define NETLINK_ISR_INFO     30
#define MSG_LEN            125
#define USER_PORT        100
#define IRQ_NUM_RX    62
#define IRP_NUM_TX    63

#ifndef IRQF_DISABLED
/*disable IRQ when processing another IRQ, can not reentry*/
#define IRQF_DISABLED 0x00000020
#endif

#ifndef IRQF_TRIGGER_HIGH
/* High level Voltage trigger */
#define IRQF_TRIGGER_HIGH 0x00000004
#endif

#ifndef IRQF_TRIGGER_RISING
/* voltage Rising   trigger */
#define IRQF_TRIGGER_RISING 0x00000001
#endif

#ifndef IRQF_SHARED
/* Shared IRQ ? */
#define IRQF_SHARED 0x00000080
#endif

#define DEVICE_NAME "bxlk-gpio-irq"

#define IRQ_NAME_TX	"bxlk-gpio-irq-tx"
#define IRQ_NAME_RX "bxlk-gpio-irq-rx"

struct sock *nlsk = NULL;
extern struct net init_net;
static struct fasync_struct *async;

static int gpio_irq_fasync(int fd, struct file *filp, int mode)  
{  
    printk("application fasync!\n");
	//regist the application pid value,
	//userplane app call fcntl with FASYNC will trigger this function
	return fasync_helper(fd, filp, mode, &async);
}  

int send_usrmsg(char *pbuf, uint16_t len)
{    
	struct sk_buff *nl_skb;    
	struct nlmsghdr *nlh;
	int ret;  

	/*mem alloc*/
	nl_skb = nlmsg_new(len, GFP_ATOMIC);
	if(!nl_skb)
	{
		printk("netlink alloc failure\n");
		return -1;    
	}   
	
	/*set head*/
	nlh = nlmsg_put(nl_skb, 0, 0, NETLINK_ISR_INFO, len, 0);
	if(nlh == NULL)
	{
		printk("nlmsg_put failaure \n");
		nlmsg_free(nl_skb);
		return -1;
	}    

	/*copy and send data */
	memcpy(nlmsg_data(nlh), pbuf, len);
	ret = netlink_unicast(nlsk, nl_skb, USER_PORT, MSG_DONTWAIT);
	return ret;
}

typedef struct isr_msg{
	unsigned int head;
	unsigned int value1;
	unsigned int value2;
	unsigned long long value3;
}ISR_INFO;

#define ISR_MSG_UPDATE_PID  (0xDDEEFFCC) 
#define ISR_MSG_CLEAN_PID  (0xFFAA55EE)

#define UPDATE_RECEIVE_RESP "PID update msg received!"
#define CLEAR_RECEIVE_RESP "PID clear msg received!"
#define DO_NO_THING "Do nothing"

static int pid_num = 0;
static struct task_struct * p = NULL;

static void netlink_rcv_msg(struct sk_buff *skb)
{
	struct nlmsghdr *nlh = NULL;
	ISR_INFO * umsg = NULL;
    struct pid * pptr = NULL;

	/*if has head*/
	if(skb->len >= nlmsg_total_size(0))
	{
		nlh = nlmsg_hdr(skb);
		umsg = (ISR_INFO *)NLMSG_DATA(nlh);

		switch(umsg->head){
			case ISR_MSG_UPDATE_PID:
				pid_num = umsg->value1;
				printk("You new registed pid value is 0x%-16x \n", pid_num);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)                
                p = find_task_by_vpid(pid_num);
#else
                p = pid_task(find_vpid(pid_num), PIDTYPE_PID);
#endif
				if(!p)
					printk(KERN_ERR "Not task found\n");
                send_usrmsg(UPDATE_RECEIVE_RESP, strlen(UPDATE_RECEIVE_RESP));
				break;
			case ISR_MSG_CLEAN_PID:
				pid_num = -1;
                send_usrmsg(CLEAR_RECEIVE_RESP, strlen(CLEAR_RECEIVE_RESP));
				break;
			default:
			/*do nothing*/
                send_usrmsg(DO_NO_THING, strlen(DO_NO_THING));
		}
	}
}

struct netlink_kernel_cfg cfg = 
{
	.input  = netlink_rcv_msg, /* set recv callback */
};

unsigned int irq_idx1;
unsigned int irq_idx2;
static irqreturn_t key_interrupt_1(int irq, void *dev_id)  
{  
	/*send SIG to userplane pid*/
    kill_fasync(&async, SIGUSR1, POLL_IN);
    return (IRQ_HANDLED);  
}  

static irqreturn_t key_interrupt_2(int irq, void *dev_id)  
{  
	/*send SIG to userplane pid*/
    kill_fasync(&async, SIGUSR2, POLL_IN);
    return (IRQ_HANDLED);  
}  

static irqreturn_t tx_handle_1(int irq, void *dev_id) 
{
	send_sig(SIGUSR1, p, 0);
	return (IRQ_HANDLED);
}

static irqreturn_t rx_handle_1(int irq, void *dev_id) 
{
	send_sig(SIGUSR2, p, 0);
	return (IRQ_HANDLED);
}

int gpio_irq_open(struct inode *inode, struct file *filp)  
{  
    printk(KERN_ALERT "application  open!\n");  
    return 0;  
}  
  
ssize_t gpio_irq_read(struct file *file, char __user *buff, size_t count, loff_t *offp)  
{  
    printk("application  read!\n");  
    return 0;  
}  
  
ssize_t gpio_irq_write(struct file *file, const char __user *buff, size_t count, loff_t *offp)  
{  
    printk("application  write!\n");  
    return 0;  
}  
  
static int gpio_irq_release(struct inode *inode, struct file *file)  
{    
    printk("application  close!\n");  
    return 0;  
}  
  
static int gpio_irq_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)  
{  
    switch(cmd)  
    {  
    case 0:  
        break;  
    case 1:   
        break;  
    default:  
        break;  
    }  
    return 0;  
}  

static struct file_operations gpio_irq_ops = {  
    .owner = THIS_MODULE,  
    .open = gpio_irq_open,  
    .release = gpio_irq_release,  
//    .ioctl = gpio_irq_ioctl,  
    .read = gpio_irq_read,  
    .write = gpio_irq_write,  
    .fasync = gpio_irq_fasync,  
}; 

static struct miscdevice gpio_irq_misc = {  
    .minor = MISC_DYNAMIC_MINOR,  
    .name = DEVICE_NAME,  
    .fops = &gpio_irq_ops,  
};  

static int __init mod_method2_init(void)
{
    int ret;  
    ret=misc_register(&gpio_irq_misc);  
	
    if(ret <0)  
    {  
        printk("register miscdevice error code:%d\n",ret);  
        return ret;  
    } 

	ret = request_irq(IRP_NUM_TX, key_interrupt_1, 0, IRQ_NAME_TX, &irq_idx1);  
	irq_set_irq_type(irq_idx1, IRQ_TYPE_EDGE_RISING);  
	
	if(ret)  
	{  
		disable_irq(irq_idx1);  
		free_irq(irq_idx1, NULL);  
		return -1;	
	}  

	ret = request_irq(IRQ_NUM_RX, key_interrupt_2, 0, IRQ_NAME_RX, &irq_idx2);  
	irq_set_irq_type(irq_idx2, IRQ_TYPE_EDGE_RISING);  
	
	if(ret)  
	{  
		disable_irq(irq_idx2);  
		free_irq(irq_idx2, NULL);  
		return -1;	
	}  	
	
	return 0;  

}

static void __exit mod_method2_exit(void)  
{  
    misc_deregister(&gpio_irq_misc);  
    printk("device delete!\n");  
	free_irq(irq_idx1, NULL);
	free_irq(irq_idx2, NULL);	
} 

/* target device */
static const struct of_device_id my_target_match[] = {
	{ .compatible = "xlnx,xps-gpio-1.00.a", },
	{ /* end of list */ },
};

int check_if_required_device(struct device *dev, void* data)
{
    return of_match_device(my_target_match, dev) != NULL;
}

static int __init mod_method_1_init(void) 
{
	int ret;
	struct resource *res;
    struct device *dev;
    struct platform_device * pdev;

    dev = bus_find_device(&platform_bus_type,  \
                    NULL, NULL, check_if_required_device);

    if(dev == NULL)
    {
        printk("No device found \n");
        return -1;
    }
    
	pdev = to_platform_device(dev);
    
	/* create netlink socket */
	nlsk = (struct sock *)netlink_kernel_create(&init_net, NETLINK_ISR_INFO, &cfg);
	
	if(nlsk == NULL)    
	{
		printk(KERN_ERR "netlink_kernel_create error !\n");
		return -1;
	}	

	res=platform_get_resource(pdev,IORESOURCE_IRQ,1);
    
	if(res == NULL )
	{
		printk("platform_get_resource TX fail\n");
		return -1;
	}
    
    //@TODO: check if use IRQF_TRIGGER_RISING or else
    ret = request_irq(res->start, tx_handle_1, IRQF_TRIGGER_RISING, IRQ_NAME_TX, NULL);
    if(ret)
    {
        printk(KERN_ERR "request IRQ_NUM_TX failed! ret = %d\n", ret);
        return -1;
    }

	res=platform_get_resource(pdev,IORESOURCE_IRQ,1);
    
	if(res == NULL )
	{
		printk("platform_get_resource RX fail\n");
		return -1;
	}

	if (request_irq(res->start, rx_handle_1, IRQF_TRIGGER_RISING, IRQ_NAME_RX, NULL))
	{
		printk(KERN_ERR "request IRQ_NUM_RX failed! ret = %d\n", ret);
		return -1;
	}

	return 0;
}

static void __exit mod_method_1_exit(void)
{
	printk(KERN_INFO "Goodbye - gpio_isr_module\n");

	if (nlsk)
	{
		netlink_kernel_release(nlsk); /* release ..*/
		nlsk = NULL;
	}
	
	free_irq(IRP_NUM_TX, NULL);
	free_irq(IRQ_NUM_RX, NULL);
}

static int chardev_drv_open(struct inode *inode, struct file *file)
{
	int ret;
	ret = request_irq(IRP_NUM_TX, key_interrupt_1, 0, IRQ_NAME_TX, &irq_idx1);  
	irq_set_irq_type(irq_idx1, IRQ_TYPE_EDGE_RISING);  
	
	if(ret)  
	{  
		disable_irq(irq_idx1);  
		free_irq(irq_idx1, NULL);  
		return -1;	
	}  

	ret = request_irq(IRQ_NUM_RX, key_interrupt_2, 0, IRQ_NAME_RX, &irq_idx2);  
	irq_set_irq_type(irq_idx2, IRQ_TYPE_EDGE_RISING);  
	
	if(ret)  
	{  
		disable_irq(irq_idx2);  
		free_irq(irq_idx2, NULL);  
		return -1;	
	}  

	return 0;
}
 
ssize_t chardev_drv_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
#if 0
	if (size != 1)
		return -EINVAL;
 
	/*no key press, sleep */
	wait_event_interruptible(button_waitq, ev_press);
 
	/* if key pressed, return the key value */
	copy_to_user(buf, &key_val, 1);
	ev_press = 0;

	return 1;
#endif
	return 0;
}
 
 
int chardev_drv_close(struct inode *inode, struct file *file)
{
	free_irq(IRP_NUM_TX, NULL);
	free_irq(IRQ_NUM_RX, NULL);
	return 0;
}

static DECLARE_WAIT_QUEUE_HEAD(button_waitq);

static unsigned chardev_drv_poll(struct file *file, poll_table *wait)
{
#if 0
	unsigned int mask = 0;
	
	poll_wait(file, &button_waitq, wait);
 
	if (ev_press)
		mask |= POLLIN | POLLRDNORM;

	return mask;
#endif
	return 0;
}
 
static int chardev_drv_fasync (int fd, struct file *filp, int on)
{
	printk("driver: chardev_drv_fasync\n");
	//init/release fasync_struct  (fasync_struct->fa_file->f_owner->pid)
	return fasync_helper (fd, filp, on, &async);
}
 
 
static struct file_operations sencod_drv_fops = {
/*  point to the var __this_module, which was generated when module was compiled */	
    .owner   =  THIS_MODULE,    
    .open    =  chardev_drv_open,     
	.read	 =	chardev_drv_read,	   
	.release =  chardev_drv_close,
	.poll    =  chardev_drv_poll,
	.fasync	 =  chardev_drv_fasync,
};

static struct class *chardevdrv_class;
static struct device	*chardevdrv_class_dev;
 
int major;
static int chardev_drv_init(void)
{
	major = register_chrdev(0, "chardev_drv", &sencod_drv_fops);
 
	chardevdrv_class = class_create(THIS_MODULE, "chardev_drv");
 	 /* /dev/buttons */
     /* #if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26) 
 	#define CLASS_DEV_CREATE(class, devt, device, name) \ 
 	        device_create(class, device, devt, name)  */
	chardevdrv_class_dev = device_create(chardevdrv_class, NULL, MKDEV(major, 0), NULL, "buttons");

	return 0;
}
 
static void chardev_drv_exit(void)
{
	unregister_chrdev(major, "chardev_drv");
	device_destroy(chardevdrv_class, chardevdrv_class_dev);
	class_destroy(chardevdrv_class);
//	iounmap(gpfcon);
	return;
}

module_init(mod_method_1_init);
module_exit(mod_method_1_exit);

MODULE_AUTHOR("Wu Xiafeng");
MODULE_LICENSE("GPL");

