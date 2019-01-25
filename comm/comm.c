/* comm.c */
/*Add by Wuxiafeng 20181202 first version */

#include "comm.h"
#include "usb_rx_tx.h"

#define SYSTEM_ON   0x5A5A5A5Aul
#define SYSTEM_OFF  0xA5A5A5A5ul

UINT32 system_state_flag = SYSTEM_OFF;

extern INT32 usb_tx_rx_init(void);
extern INT32 comm_isr_init(VOID);

VOID set_system_on(void){
	if(system_state_flag != SYSTEM_ON)
		system_state_flag = SYSTEM_ON;
    
#ifndef ARCH_X86
    /*set power on flag */
    set_power_on_flag();
#endif
	return;
}

int is_system_on(void){
	return (system_state_flag == SYSTEM_ON);

}

int comm_init(void)
{
    int ret;

    ret = usb_tx_rx_init();

    if(ret)
        return ret;

    ret = comm_isr_init();

    return 0;
}



