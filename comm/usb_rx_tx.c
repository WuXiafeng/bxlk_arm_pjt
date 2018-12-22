/* usb_rx_tx.c */
/* Add by Wuxiafeng 20181203 first version */

#include "bram.h"
#include "queuehandle.h"
#include "work_thread.h"
#include "usb_rx_tx.h"
#include "pf.h"

#define BRAM_TX_DATA_OFFSET 0x20
#define BRAM_TX_LEN_OFFSET 0x800
#define BRAM_TX_FLAG_OFFSET 0x810
#define BRAM_MSG_MAX_LEN  0x7e0

#define BRAM_RX_DATA_OFFSET 0x1020
#define BRAM_RX_LEN_OFFSET 0x1800
#define BRAM_RX_FLAG_OFFSET 0x1810

#define BARM_TX_FLAG 0x5a5a5a5aul
#define BRAM_RX_FLAG 0xa5a5a5a5ul

INT32 tx_rx_sema;

extern INT32 rx_work_thread_id;

INT32 usb_tx_rx_init(void)
{
    INT32 ret;
    /* sema  init value is zero */
    ret = PT_OPT.sema_init(&tx_rx_sema, 0);

    if(ret < 0)
        return ret;
    
    return 0;
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
    
    ret = bram_write(BRAM_TX_DATA_OFFSET,blk->buf,len);

    if(ret)
        return;
    
    ret = bram_write(BRAM_TX_LEN_OFFSET, (UINT8*)&len, 4);
    if(ret)
        return;

    tmp = BARM_TX_FLAG;
    ret = bram_write(BRAM_TX_FLAG_OFFSET, (UINT8*)&tmp, 4);
    if(ret)
        return;

    /* make sure function blocked here */
    PT_OPT.sema_wait(tx_rx_sema);
}

/* This function is called in userplane ISR,  it's OK to do any operation */
VOID usb_rx_handle(VOID)
{
    UINT8 * buf;
    int len;
    int ret;
    int tmp;
    
    bram_read(BRAM_RX_LEN_OFFSET, (UINT8*)&len, 4);

    if(len > BRAM_MSG_MAX_LEN)
        len = BRAM_MSG_MAX_LEN;
    
    PT_OPT.mem_alloc(len, (VOID **)&buf, 0);

    if(!buf)
    {
        printf("usb_rx_handle: alloc memory failed!\n");
        return;
    }

    ret = bram_read(BRAM_RX_DATA_OFFSET, buf, len);
    if(ret)
        return;

    add_work_to_thread(rx_work_thread_id, (VOID*)buf, len);

    tmp = BRAM_RX_FLAG;
    ret = bram_write(BRAM_RX_FLAG_OFFSET, (UINT8*)&tmp, 4);

    if(ret)
        return;
}

VOID usb_tx_done(VOID)
{
    PT_OPT.sema_post(tx_rx_sema);
}

VOID usb_tx_send(INT8 * buf, INT32 len)
{
    add_work_to_thread(tx_work_thread_id, (VOID*)buf, len);
}

