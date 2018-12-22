/* hw.h */
/*Add by Wuxiafeng 20181202 first version */

#ifndef __HW_H__
#define __HW_H__
#include "types.h"

extern isr_func rx_func;
extern isr_func tx_func;

int pt_hw_init(void);
#endif /* __HW_H__ */

