/* array.h */
/* Add by Wuxf 20181130 first version */

#ifndef __ARRAY_H__
#define __ARRAY_H__
#include "types.h"

#define STATE_VALIDE 0xBEEFBEEF

typedef struct array_comm_head{
    INT32 state;
}ARR_HEAD;

typedef bool (*walk_func)(VOID* ptr, VOID* para);

static inline INT32 find_array_free(UINT8 * ptr, INT32 len, INT32 size)
{
    int i;
    for(i = 0; i < len; i++)
    {
        if((*(UINT32 *)(ptr + i*size)) != STATE_VALIDE)
            return i;
    }
    return ERROR;
}

static inline VOID* walk_array(UINT8 * ptr, 
                                    INT32 len, 
                                    INT32 size, 
                                    walk_func fc,
                                    VOID * para)
{
    int i;
    for(i = 0; i < len; i++)
    {
        if((*(UINT32 *)(ptr + i*size)) == STATE_VALIDE)
        {
            if(fc((VOID*)(ptr + i*size), para))
            {
                return (VOID*)(ptr + i*size);
            }
        }
    }
    
    return NULL;
}

#endif /* __ARRAY_H__ */

