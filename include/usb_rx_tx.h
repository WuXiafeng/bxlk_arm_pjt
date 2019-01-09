/* usb_rx_tx.h */

#ifndef __USB_RX_TX_H__
#define __USB_RX_TX_H__

#include "types.h"
#include "queuehandle.h"

VOID usb_rx_handle(VOID);
VOID usb_tx_done(VOID);
VOID start_usb_tx(QUE_BLK * blk, VOID * para);
VOID set_power_on_flag(VOID);
VOID usb_tx_send(INT8 * buf, INT32 len);

#endif /*__USB_RX_TX_H__*/

