/*bram.h*/
/*Add by Wuxiafeng 20181202 first version */

#ifndef __BRAM_H__
#define __BRAM_H__
#include "types.h"

int bram_read(UINT32 offset, UINT8 * buf, int len);
int bram_write(UINT32 offset, UINT8 * buf, int len);
int bram_release(void);
int bram_init(void);
#endif /* __BRAM_H__ */

