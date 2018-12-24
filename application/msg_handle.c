/* msg_handle.c */
/* 20181203 first version */

#include "msg_handle.h"

INT32 test_func(char * str, int len);

typedef INT32 (*msg_func)(INT8 **keylist,INT32 len,INT32 data1,INT32 data2,
                      INT8* data3,
                      INT32 operation,
                      INT8 valstr,
                      INT8 *resp_buf);

struct {
    char * head;
    msg_func  func;
} head_func_map_table[] = {
    {"testhead1", test_func},
    {":FREQ:CW", freq_handle},
    {":FREQ:RF:STAR", freq_handle},
    {":FREQ:RF:STOP", freq_handle},
    {":FREQ:RF:SCAL", freq_handle},
};

#define RESP_DEFAULT_LEN (0x200)

INT32 test_func(char * str, int len)
{
    printf("received buf is:  \n\t%s\n\n\t len is %d\n",str,len);
    return 0;
}

msg_func get_func_by_msg_head(char * msg_head)
{
    int i;

    for(i=0;i<(int)NUM_ELEMENTS(head_func_map_table);i++)
    {
        if(!strcmp(head_func_map_table[i].head,msg_head))
            return head_func_map_table[i].func;
    }

    return NULL;
}

INT32 get_cmd_func_and_run(INT8 **keylist,
                      INT32 len,
                      INT32 data1,
                      INT32 data2,
                      INT8* data3,
                      INT32 operation,
                      INT8 valstr,
                      INT8 *resp_buf)
{
    msg_func  func;
    INT8 tmplist[128];

    memset(tmplist,0,128);

    combine_keylist(tmplist, keylist, len);

    func = get_func_by_msg_head(tmplist);

    if(!func)
    {
        printf("get_cmd_func_and_run, no match function found!, " \
            "the keylist is %s \n",\
                tmplist);
        return -1;
    }

    return func(keylist, len,data1,data2,data3,operation,valstr,resp_buf);
}

INT32 cmd_handle(INT8 * cmd)
{
    int ret;
    INT8 keylist[8][MAX_KEY_STR_LEN];
    INT32 len;
    INT32 data1;
    INT32 data2;
    INT8 data3[BACK_UP_BUF_LEN];
    INT8 valstr[128];
    INT32 operation;
    INT8 * resp_buf;

    memset(keylist, 0, sizeof(keylist));
    memset(valstr, 0, 128);

    ret = commd_str_prase(cmd,keylist,&len,&data1,&data2, \
                    data3,&operation,&valstr);

    if(ret < 0)
    {
        printf("rx_msg_handle, prase msg failed!\n");
        return -1;
    }

    ret = PT_OPT.mem_alloc(RESP_DEFAULT_LEN, &resp_buf, 0);

    if(resp_buf)
        memset(resp_buf,0,RESP_DEFAULT_LEN);

    /* if resp_buf is NULL, the cmd_handle will ignore it*/
    ret = get_cmd_func_and_run(keylist,len,data1,data2,data3,operation,resp_buf);

    if(ret < 0)
    {
        printf("rx_msg_handle, handle msg failed!\n");
    }

    /* if there's any string to send back, send it out, if no string ,just free the memory */
    if(resp_buf)
    {
        if(strlen(resp_buf))
        {
            core_msg_send(resp_buf, strlen(resp_buf));
        }
        else
        {
            PT_OPT.mem_free(resp_buf);
        }
    }

    return 0;
}

VOID rx_msg_handle(QUE_BLK * blk, VOID * para)
{
    char * msg;
    int ret;

    if((!blk) || (blk->buf))
    {
        printf("rx_msg_handle NULL buffer received!\n");
        return;
    }

    msg = (char *)blk->buf;
    ret = cmd_handle(msg);

    if(ret < 0)
    {
        printf("rx_msg_handle, handle msg failed!\n");
        return;
    }
}

INT32 get_value(INT8 **keylist, INT32 len, INT8* resp_buf)
{
    INT32 ret;

    ret = get_target_kv_node(keylist, len, NULL, NULL, NULL, resp_buf);

    if(ret < 0)
        return -1;

    return 0;
}

