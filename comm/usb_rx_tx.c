/* usb_rx_tx.c */
/* Add by Wuxiafeng 20181203 first version */

#include "bram.h"
#include "queuehandle.h"
#include "work_thread.h"
#include "usb_rx_tx.h"
#include "pf.h"
#include "core.h"

#define BOOT_STATUS 0x0UL
#define PWR_OK_BIT_MASK 0x00000001UL
#define WORK_STATUS 0x1ul
#define RFNC_BIT_MASK 0x00000001UL
#define DRFC_BIT_MASK 0x00000002UL

#define CNT_CMD 0x0aUL
#define CNT_DFC 0x0bUL

#define CMD_Buffer 0x200UL
#define DFC_Buffer 0x1000UL

#define BRAM_TX_DATA_OFFSET DFC_Buffer
#define BRAM_TX_LEN_OFFSET CNT_DFC
#define BRAM_TX_FLAG_OFFSET WORK_STATUS
#define BRAM_MSG_MAX_LEN  0xa00

#define BRAM_RX_DATA_OFFSET CMD_Buffer
#define BRAM_RX_LEN_OFFSET CNT_CMD
#define BRAM_RX_FLAG_OFFSET WORK_STATUS

#define BARM_TX_FLAG DRFC_BIT_MASK
#define BRAM_RX_FLAG RFNC_BIT_MASK

INT32 tx_rx_sema;

extern INT32 rx_work_thread_id;

INT32 usb_tx_rx_init(void)
{
    /* sema  init value is zero */
    tx_rx_sema = PT_OPT.sema_init(0);

    if(tx_rx_sema < 0)
        return ERROR;
#ifndef ARCH_X86 
    UINT32 tmp;
    INT32 ret;
    tmp = RFNC_BIT_MASK;
   
    ret = bram_write(WORK_STATUS,(UINT8*)&tmp,4);

    if(ret)
        return ret;
#endif

    return 0;
}

VOID set_power_on_flag(VOID)
{
    UINT32 tmp;
    tmp = PWR_OK_BIT_MASK;

    bram_write(BOOT_STATUS,(UINT8 *)&tmp,4);
    return;
}

VOID start_usb_tx(QUE_BLK * blk, VOID * para)
{
    int len;
    unsigned int tmp;
    int ret;

    (void)para;

    if((!blk) || (!blk->buf))
        return;

    len = blk->buf_size;
    if(blk->buf_size > BRAM_MSG_MAX_LEN)
    {
        len = BRAM_MSG_MAX_LEN;
    }
    
    ret = bram_write(BRAM_TX_DATA_OFFSET,(UINT8 *)blk->buf,len);

    if(ret)
        return;
    
    ret = bram_write(BRAM_TX_LEN_OFFSET, (UINT8*)&len, 4);
    if(ret)
        return;

    /*set DRFC */
    bram_read(BRAM_RX_FLAG_OFFSET,(UINT8 *)&tmp,4);
    tmp |= DRFC_BIT_MASK;
    ret = bram_write(BRAM_TX_FLAG_OFFSET, (UINT8*)&tmp, 4);
    if(ret)
        return;

    /* make sure function blocked here */
    PT_OPT.sema_wait(tx_rx_sema);
}

/* This function is called in userplane ISR,  it's OK to do any operation */
/* Make sure the ISR should not nest */
VOID usb_rx_handle(VOID)
{
    UINT8 * buf;
    int len;
    int ret;
    int tmp;

    /*TODO: move this function to a better place*/
    core_feed_usb();
    
    bram_read(BRAM_RX_LEN_OFFSET, (UINT8*)&len, 4);

    if(len > BRAM_MSG_MAX_LEN)
        len = BRAM_MSG_MAX_LEN;
    
    PT_OPT.mem_alloc(len, (VOID **)&buf, 0);

    if(!buf)
    {
        printf("usb_rx_handle: alloc memory failed!\n");        
        goto clear_reg;
    }

    ret = bram_read(BRAM_RX_DATA_OFFSET, buf, len);
    if(ret)
        goto clear_reg;

    ret = add_work_to_thread(rx_work_thread_id, (VOID*)buf, len);

    if(ret)
    {
        printf("usb_rx_handle: add buf to rx_work_thread_id failed!\n");        
        goto clear_reg;        
    }

clear_reg:
    /*clear CNT_CMD*/
    tmp = 0;
    bram_write(BRAM_RX_LEN_OFFSET,(UINT8 *)&tmp,4);
    
    /*reset RFNC */
    bram_read(BRAM_RX_FLAG_OFFSET,(UINT8 *)&tmp,4);
    tmp |= RFNC_BIT_MASK;
    bram_write(BRAM_RX_FLAG_OFFSET,(UINT8 *)&tmp,4);

    return;
}

VOID usb_tx_done(VOID)
{
    PT_OPT.sema_post(tx_rx_sema);
}

VOID usb_tx_send(INT8 * buf, INT32 len)
{
    INT32 ret;
    ret = add_work_to_thread(tx_work_thread_id, (VOID*)buf, len);
    if(ret)
    {
        printf("usb_tx_send: add buf to rx_work_thread_id failed!\n");                
    }    
}

