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

void start_socket_session_control(void)

{
	/* todo: add socket session handler */
	for(;;)
	{
		usleep(10000);
	}
}

VOID core_msg_send(INT8 *buf, INT32 len)
{
    //if(USB_IS_ON)
        usb_tx_send(buf,len);
    
    //SOCKET_SEND();        
}


