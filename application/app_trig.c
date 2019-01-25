/* app_trig.c */

#include "app_trig.h"
#include "msg_handle.h"
#include "str_prase.h"
#include "config_file_manage.h"

INT32 trig_data_check(INT8 **keylist, INT32 len, double data1,INT8 * data3)
{
    (void)data3;
    (void)data1;

    if((len != 2)||(strcmp("TRIG",(char *)keylist[0])))
        return -1;

    if((strcmp("IMM",(char *)keylist[1])))
        return -1;

    return OK;
}

INT32 trig_set_logic_reg(INT8 **keylist,INT32 len)
{
    //TODO: 
    (void)keylist;
    (void)len;    
    return OK;
}

INT32 trig_handle(INT8 **keylist,INT32 len,double data1,INT32 data2,
                      INT8* data3,
                      INT32 operation,
                      INT8* valstr,
                      INT8 *resp_buf)
{
    INT32 ret;
    (VOID)data3;
    (VOID)valstr;
    (void)data2;
    
    switch (operation) 
    {
        case OPT_CHECK_KEY_LIST:
            ret = trig_data_check(keylist, len,data1, data3);

            if(ret < 0)
            {
                printf("ERROR,trig_handle, parameter validation faild ");
                strcat((char *)resp_buf, (const char *)"ERROR,trig_handle, parameter validation faild ");
                return -1;
            }            

            ret = trig_set_logic_reg(keylist, len);
            if(ret < 0)
            {
                printf("ERROR,trig_handle, trig operation faild ");
                strcat((char *)resp_buf, "ERROR,trig_handle, trig operation faild ");
                return -1;
            } 

            return OK;
        case OPT_GET_VALUE:
        case OPT_SET_VALUE:
        default:
            printf("trig_handle Operation no support! %d \n", operation);
            strcat((char *)resp_buf, (const char *)"ERROR,trig_handle, Operation no support!");
            return ERROR;
    }

}


