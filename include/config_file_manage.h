/* config_file_manage.h */

#ifndef __CONFIG_FILE_NAMANAGE_H__
#define __CONFIG_FILE_NAMANAGE_H__
#include "types.h"
#include "queue.h"

SLIST_HEAD(kv_head, key_value);

/* key value obj */
typedef struct key_value{
    SLIST_ENTRY(key_value) next_brother;
    struct kv_head son_head;
    key_value * parent;
    INT32 state;
    INT8 key[MAX_KEY_STR_LEN];
    INT8 value[MAX_VALUE_STR_LEN];
    double data1;
    INT32 data2;
    INT8 * data3;
    INT32 son_num;
}K_V;

INT32 cfg_init_load_file(void);
K_V * create_and_get_target_kv_node(const INT8 ** keylist, INT32 len);
K_V * find_target_kv_node(const INT8 ** keylist, INT32 len);
INT32 update_target_kv_node(const INT8 ** keylist, INT32 len, 
                                       double data1, INT32 data2, INT8* data3,
                                       INT8 * val_str);
INT32 add_target_kv_node(const INT8 ** keylist, INT32 len, 
                                       double data1, INT32 data2, INT8* data3,
                                       INT8 * val_str);
INT32 get_target_kv_node(const INT8 ** keylist, INT32 len, 
                                       double *data1, INT32 *data2, INT8* data3,
                                       INT8 * val_str);
INT32 sync_to_cfg_file(void);
VOID check_and_set_the_default_value(VOID);

#endif /* __CONFIG_FILE_NAMANAGE_H__ */


