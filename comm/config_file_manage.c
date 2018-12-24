/* config_file_manage.c */
#include "types.h"
#include "pf.h"
#include "array.h"

typedef enum rf_scal{
    LOG = 0x1000,
    LIN
}FSCAL;

SLIST_HEAD(kv_head, key_value);

/* key value obj */
typedef struct key_value{
    SLIST_ENTRY(key_value) next_brother;
    struct kv_head son_head;
    key_value * parent;
    INT32 state;
    INT8 key[MAX_KEY_STR_LEN];
    INT8 value[MAX_VALUE_STR_LEN];
    INT32 data1;
    INT32 data2;
    INT8 * data3;
    INT32 son_num;
}K_V;

K_V cfg_root_node;
INT32 cfg_mute_lock = -1;

INT32 cfg_init_load_file(void)
{
    FILE *fp;
    INT8 buff[256];
    memset(&cfg_root_node, 0, sizeof(K_V));
    strcpy(cfg_root_node.key, "root");
    SLIST_INIT(cfg_root_node.son_head);
    INT32 ret;
    
    PT_OPT.mute_init(&cfg_mute_lock, 0);

    if(cfg_mute_lock < 0)
    {
        printf("config_init_load_file error, mute lock init failed!\n");
        return -1;
    }

    if((access("/mnt/flash/bxlk.confg",0)) != -1)
    {
        fp = fopen("/mnt/flash/bxlk.confg", "r");
    }
    else if((access("/mnt/flash/bxlk.confg.bak",0)) != -1)
    {
        fp = fopen("/mnt/flash/bxlk.confg.bak", "r");
    }
    else
    {
        printf("Warning , no default config file found!\n");
        return 0;
    }

    if (fp == NULL)
    {  
        printf("Warning, cfg_init_load_file can not open file!\n");
        return 0;
    }

    INT8 keylist[8][MAX_KEY_STR_LEN];
    INT32 len;
    INT32 data1;
    INT32 data2;
    INT8 data3[BACK_UP_BUF_LEN];
    INT8 valstr[128];
    INT32 operation;

    memset(buff, 0, 256);
    memset(keylist, 0, sizeof(keylist));
    memset(valstr, 0, 128);
    while (fgets(buff, 256, fp) != NULL)
    {
        ret = commd_str_prase(buff,keylist,&len,&data1,&data2, \
                        data3,&operation,&valstr);
        if(ret < 0){
            memset(buff, 0, 256);
            memset(keylist, 0, sizeof(keylist));
            memset(valstr, 0, 128);
            continue;
        }

        /* Check the paramter validation in this function*/
        /*May be the value is invalid, then we set the default value*/
        ret = get_cmd_func_and_run(keylist,len,data1,data2,data3,operation,NULL);
        if(ret < 0)
        {
            printf("Warning, cfg_init_load_file handle init cmd failed!\n");
            fclose(fp);

            continue;
        }  

        ret = add_target_kv_node(keylist,len,data1,data2,data3,valstr);
        
        if(ret < 0)
        {
            printf("ERROR, cfg_init_load_file prase cfg file failed!\n");
            fclose(fp);
            return -1;
        }

        memset(buff, 0, 256);
        memset(keylist, 0, sizeof(keylist));
        memset(valstr, 0, 128);        
    }

    fclose(fp);
    return 0;
}

bool if_key_equal(K_V * ptr, const INT8 * key)
{
    if(!strcmp(ptr->key, key))
        return true;

    return false;
}

K_V * allc_kv_obj(void)
{
    K_V * tmptr;
    INT32 ret;
    
    ret = PT_OPT.mem_alloc(sizeof(K_V), &tmptr, 0);

    if(!tmptr)
        return NULL;

    memset(tmptr, 0, sizeof(K_V));
    SLIST_INIT(tmptr->son_head);

    return tmptr;
}

K_V * get_son_kv_obj(K_V * pare, const INT8 * key)
{
    K_V * tmptr;

    PT_OPT.mute_lock(cfg_mute_lock); 
    SLIST_FOREACH(tmptr, pare->son_head, next_brother)
    {
        if(if_key_equal(tmptr, key))
        {
            PT_OPT.mute_unlock(cfg_mute_lock);
            return tmptr;
        }
    }
    PT_OPT.mute_unlock(cfg_mute_lock);
    return NULL;
}

void add_son_kv_obj(K_V * pare, K_V * son)
{
    PT_OPT.mute_lock(cfg_mute_lock);    
    SLIST_INSERT_HEAD(pare->son_head, son, next_brother);

    son->parent = pare;
    pare->son_num++;
    PT_OPT.mute_unlock(cfg_mute_lock);    
    return;
}

void remove_son_kv_obj(K_V * pare, K_V * son)
{
    K_V * tmptr;

    if(!SLIST_EMPTY(son->son_head))
    {
        SLIST_FOREACH(tmptr, son->son_head, next_brother)
        {
            remove_son_kv_obj(son, tmptr);
        }
    }
    
    PT_OPT.mute_lock(cfg_mute_lock);
    if(pare)
    {
        SLIST_REMOVE(pare->son_head, son, key_value, next_brother);
        pare->son_num--;
    }
    
    PT_OPT.mute_unlock(cfg_mute_lock);
    if(son->data3)
        PT_OPT.mem_free(son->data3);
    PT_OPT.mem_free(son);
}

K_V * create_and_get_target_kv_node(const INT8 ** keylist, INT32 len)
{
    INT32 i;
    K_V * tmptr = &cfg_root_node;
    K_V * parent;

    for(i = 0;i<len;i++)
    {
        parent = tmptr;
        tmptr = get_son_kv_obj(tmptr, keylist[i]);
        if(!tmptr)
        {
            tmptr = allc_kv_obj();

            if(tmptr == NULL)
            {
                printf("Error, create_and_get_target_kv_node alloc new node failed!\n");
                return NULL;
            }

            strncpy(tmptr->key,keylist[i], \
                (strlen(keylist[i]) > MAX_KEY_STR_LEN ?  \
                        MAX_KEY_STR_LEN : strlen(keylist[i])));

            add_son_kv_obj(parent, tmptr);

        }
    }

    return tmptr;
}

K_V * find_target_kv_node(const INT8 ** keylist, INT32 len)
{
    INT32 i;
    K_V * tmptr = &cfg_root_node;
    
    for(i = 0;i<len;i++)
    {
        tmptr = get_son_kv_obj(tmptr, keylist[i]);
        if(!tmptr)
            return NULL;
    }
    
    return tmptr;
}

void free_target_kv_node(const INT8 ** keylist, INT32 len)
{
    K_V * kvobj;
    K_V * tmptr;
    kvobj = find_target_kv_node(keylist, len);

    if(!kvobj)
    {
        printf("Warning, free_target_kv_node node not exist!\n");
        return;
    }

    remove_son_kv_obj(kvobj->parent,kvobj);
    
    return;
}

INT32 update_target_kv_node(const INT8 ** keylist, INT32 len, 
                                       INT32 data1, INT32 data2, INT8* data3,
                                       INT8 * val_str)
{
    K_V * kvobj;
    kvobj = find_target_kv_node(keylist, len);

    if(!kvobj)
    {
        printf("Warning, update_target_kv_node node not exist!\n");
        return -1;
    }

    PT_OPT.mute_lock(cfg_mute_lock);
    kvobj->data1 = data1;
    kvobj->data2 = data2;
    
    if(val_str){
        memset(kvobj->value,0,MAX_VALUE_STR_LEN);
        strncpy(kvobj->value,val_str, \
            (strlen(val_str) > MAX_VALUE_STR_LEN ? \
                    MAX_VALUE_STR_LEN : strlen(val_str)));
    }
    
    if(data3 != NULL)
    {
        if(kvobj->data3)
        {
            memset(kvobj->data3,0,BACK_UP_BUF_LEN);
        }
        else
        {
            PT_OPT.mem_alloc(BACK_UP_BUF_LEN, &kvobj->data3, 0);
            if(!kvobj->data3)
            {
                printf("Warning, update_target_kv_node allc mem failed!\n");
                PT_OPT.mute_unlock(cfg_mute_lock);
                return -1;
            }
        }

        memcpy(kvobj->data3,data3,BACK_UP_BUF_LEN);
    }
    
    PT_OPT.mute_unlock(cfg_mute_lock);
    return 0;
}

INT32 add_target_kv_node(const INT8 ** keylist, INT32 len, 
                                       INT32 data1, INT32 data2, INT8* data3,
                                       INT8 * val_str)
{
    K_V * kvobj;
    kvobj = create_and_get_target_kv_node(keylist, len);

    if(!kvobj)
    {
        printf("Warning, add_target_kv_node node failed!\n");
        return -1;
    }

    PT_OPT.mute_lock(cfg_mute_lock);
    kvobj->data1 = data1;
    kvobj->data2 = data2;

    if(val_str){
        memset(kvobj->value,0,MAX_VALUE_STR_LEN);
        strncpy(kvobj->value,val_str, \
            (strlen(val_str) > MAX_VALUE_STR_LEN ? \
                    MAX_VALUE_STR_LEN : strlen(val_str)));    
    }

    if(data3 != NULL)
    {

        PT_OPT.mem_alloc(BACK_UP_BUF_LEN, &kvobj->data3, 0);
        if(!kvobj->data3)
        {
            printf("Warning, add_target_kv_node allc mem failed!\n");
            PT_OPT.mute_unlock(cfg_mute_lock);
            return -1;
        }
    
        memcpy(kvobj->data3,data3,BACK_UP_BUF_LEN);
    }

    PT_OPT.mute_unlock(cfg_mute_lock);
    return 0;
}

INT32 get_target_kv_node(const INT8 ** keylist, INT32 len, 
                                       INT32 *data1, INT32 *data2, INT8* data3,
                                       INT8 * val_str)
{
    K_V * kvobj;
    kvobj = find_target_kv_node(keylist, len);

    if(!kvobj)
    {
        printf("Warning, get_target_kv_node node not exist!\n");
        return -1;
    }

    PT_OPT.mute_lock(cfg_mute_lock);
    if(data1)
        *data1 = kvobj->data1;

    if(data2)
        *data2 = kvobj->data2;

    if(data3)
        memcpy(data3, kvobj->data3, BACK_UP_BUF_LEN);

    if(val_str)
        strcpy(val_str,kvobj->value);
    PT_OPT.mute_unlock(cfg_mute_lock);
    return 0;
}                                       

typedef void (*walk)(INT8 ** keylist,INT32 len,INT32 data1, 
                      INT32 data2,INT8* data3, INT8 *valstr, void * para);

INT32 cfg_walk_leaf_node(K_V * node, walk func , 
                                    INT8 ** keylist, INT32 deep, void* para)
{
    if(deep > 4)
    {
        return -1;
    }
    
    keylist[deep] = node->key;

    if(!SLIST_EMPTY(node->son_head))
    {
        K_V * tmptr;
        SLIST_FOREACH(tmptr, node->son_head, next_brother)
        {    
            cfg_walk_leaf_node(tmptr, func, keylist, deep + 1, para);
        }        
    }
    else
    {
        func(keylist, deep, node->data1,\
                node->data2,node->data3,node->value,para);
    }

    return 0;
}

INT32 cfg_walk_all_leaf_node(walk func, void * para)
{
    INT8 * keylist[8];

    memset(keylist, 0, sizeof(keylist));

    return cfg_walk_leaf_node(&cfg_root_node, func, keylist, 0, para);
}

void assemble_cfg_and_write(INT8 ** keylist, INT32 len,
                        INT32 data1, INT32 data2, 
                        INT8* data3, INT8 *valstr, void * para)
{
    INT8 tmpstr[128];
    FILE * fp = (FILE *)para;
    (void)data1;
    (void)data2;
    (void)data3;
    
    memset(tmpstr,0,128);
    //snprintf(tmpstr,128,":");

    combine_keylist(tmpstr, keylist, len);
    
    strcat(tmpstr, valstr);
    fputs (tmpstr, fp);

    return;
}

INT32 sync_to_cfg_file(void)
{
    FILE * fp;
    /*Create bak file*/
    PT_OPT.mute_lock(cfg_mute_lock);
    
    if ((access("/mnt/flash/bxlk.confg",0)) != -1)
    {
        system("mv -f /mnt/flash/bxlk.confg /mnt/flash/bxlk.config.bak");
    }
    
    fp = fopen("/mnt/flash/bxlk.confg", "a+");

    if(fp != 0)
    {
        printf("create config file failed!\n");
        PT_OPT.mute_unlock(cfg_mute_lock);
        return -1;
    }

    cfg_walk_all_leaf_node(assemble_cfg_and_write, (void *)fp);
    
    fclose(fp);
    PT_OPT.mute_unlock(cfg_mute_lock);
    return 0;
}

