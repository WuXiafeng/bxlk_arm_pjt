/* app_cfg.c */

#include "app_cfg.h"
#include "config_file_manage.h"

INT32 cfg_handle(INT8 **keylist,INT32 len,double data1,INT32 data2,
                      INT8* data3,
                      INT32 operation,
                      INT8* valstr,
                      INT8 *resp_buf)
{
    (VOID)keylist;
    (VOID)len;
    (VOID)data1;
    (VOID)data2;
    (VOID)data3;
    (VOID)operation;
    (VOID)valstr;
    
    INT32 ret;
    
    ret = sync_to_cfg_file();

    if(ret)
    {
        printf("cfg_handle sync config failed!\n");
        strcat((char *)resp_buf, (const char *)"ERROR,cfg_handle sync config failed!");
        return ERROR;
    }

    return OK;
}

