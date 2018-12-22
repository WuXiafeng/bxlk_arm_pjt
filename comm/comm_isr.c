/* comm_isr */
/*Add by Wuxiafeng 20181204 first version*/
#include "comm_isr.h"
#include "hw.h"

INT32 comm_ISR_register(isr_func func, UINT32 isr_idx)
{
    if(ISR_NUM_USB_TX == isr_idx)
    {
        tx_func = func;
        return 0;
    }

    if(ISR_NUM_USB_RX == isr_idx)
    {
        rx_func = func;
        return 0;
    }

    return 0;
}

INT32 comm_isr_init(VOID){

    /* open device */
    

    /* register current pid to  kernel dirver */


    /* init the sig , regist the isr */

    return 0;
}


