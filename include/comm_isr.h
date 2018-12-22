/*comm_isr.h*/

#ifndef __COMM_ISR_H__
#define __COMM_ISR_H__
#include "types.h"

INT32 comm_ISR_register(isr_func func, UINT32 isr_idx);
INT32 comm_isr_init(VOID);

#endif /*__COMM_ISR_H__*/

