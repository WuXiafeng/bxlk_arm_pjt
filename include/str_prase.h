/* str_prase.h */

#ifndef __STR_PRASE_H__
#define __STR_PRASE_H__
#include "types.h"

#define MAX_KEY_STR_LEN   (16)
#define MAX_VALUE_STR_LEN  (128)

/*pare 4 values at most*/
#define VAL_LIST_LEN (4)
#define STR_VALUE (666)
#define DIGI_VALUE (777)

typedef enum cmd_opra{
    OPT_GET_VALUE = 0x3923,
    OPT_CHECK_KEY_LIST, /* command operation */
    OPT_SET_VALUE
}CMD_OPRA;

typedef enum rf_scal{
    LOG = 0x1000,
    LIN
}FSCAL;

typedef INT32 (*STRFUNCPTR)(INT32 c);

typedef struct val_list{
    char * value_list[VAL_LIST_LEN];
}VAL_LIST;

#define BACK_UP_BUF_LEN (sizeof(VAL_LIST) + MAX_VALUE_STR_LEN)

INT32 commd_str_prase(INT8 *buf,
                              INT8 **keylist,
                              INT32 *len,
                              double *data1,
                              INT32 *data2,
                              INT8* data3,
                              INT32 *operation,
                              INT8 *valstr);

void combine_keylist(INT8 *dst, INT8 ** keylist, INT32 len);
INT32 convet_str_2_value(INT8 * str, double * db);

#endif /* __STR_PRASE_H__ */

