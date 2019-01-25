/* app_rfo. c */

#include "app_rfo.h"
#include "msg_handle.h"
#include "str_prase.h"
#include "config_file_manage.h"

INT32 rfo_data_check(INT8 **keylist, INT32 len, double data1,INT8 * data3)
{
    (void)data3;

    if((len != 2) || (strcmp("RFO",(char *)keylist[0])))
        return -1;

    if(!(strcmp("STAT",(char *)keylist[1])))
    {
        if((data1 !=  0) && (data1 != 1))
            return -1;
    }

    return OK;
}

INT32 rfo_set_logic_reg(INT8 **keylist,INT32 len)
{
    //TODO: 
    (void)keylist;
    (void)len;    
    return OK;
}

INT32 rfo_handle(INT8 **keylist,INT32 len,double data1,INT32 data2,
                      INT8* data3,
                      INT32 operation,
                      INT8* valstr,
                      INT8 *resp_buf)
{
    INT32 ret;
    (void)data2;
    
    switch (operation) 
    {
        case OPT_GET_VALUE:
            ret = get_value(keylist,len,resp_buf);
            if(ret <0)
            {
                printf("rfo_handle get value failed!\n");
                strcat((char *)resp_buf, (const char *)"ERROR,rfo_handle get value failed!\n");
                return -1;
            }
            return 0;
        case OPT_SET_VALUE:
            ret = rfo_data_check(keylist, len,data1, data3);

            if(ret < 0)
            {
                printf("ERROR,rfo_handle, parameter validation faild ");
                strcat((char *)resp_buf, (const char *)"ERROR,rfo_handle, parameter validation faild ");
                return -1;
            }
            
            ret = add_target_kv_node(keylist,len,data1,0,data3,valstr);
            if(ret < 0)
            {
                printf("ERROR,rfo_handle, add CONFIG faild ");
                strcat((char *)resp_buf, (const char *)"ERROR,rfo_handle, add CONFIG faild ");
                return -1;
            }      

            ret = rfo_set_logic_reg(keylist, len);
            
            if(ret < 0)
            {
                printf("ERROR,rfo_handle, reg config faild ");
                strcat((char *)resp_buf, (const char *)"ERROR,rfo_handle, reg config faild ");
                return -1;
            }                         
            return 0;
        case OPT_CHECK_KEY_LIST:
        default:
            printf("rfo_handle Operation no support! %d \n", operation);
            strcat((char *)resp_buf, (const char *)"ERROR,rfo_handle, Operation no support!");
            return ERROR;
    }

}


