/*app_freq.c*/
#include "app_freq.h"
#include "msg_handle.h"
#include "str_prase.h"
#include "config_file_manage.h"

INT32 freq_str_val_map(INT8 * str)
{
    if(!strcmp(str, "GHz"))
    {
        return UNIT_GHZ;
    }
    else if(!strcmp(str, "MHz"))
    {
        return UNIT_MHZ;
    }
    else if(!strcmp(str, "kHz"))
    {
        return UNIT_KHZ;
    }

    return -1;
}

INT32 freq_data_check(INT8 **keylist, INT32 len, double data1,INT8 * data3)
{
    VAL_LIST * list_ptr;
    list_ptr = (VAL_LIST *)data3;
    INT32 tmp_data2;

    if((len > 3) || (strcmp("FREQ",keylist[0])))
        return -1;

    if((strcmp("CW",keylist[1])) && (strcmp("RF",keylist[1])))
        return -1;

    if((len == 3) && (strcmp("STOP",keylist[2]))&& (strcmp("SCAL",keylist[2])) \
        && (strcmp("STAR",keylist[2])))
        return -1;

    tmp_data2 = freq_str_val_map(list_ptr->value_list[1]);

    if(tmp_data2 < 0)
    {
        return -1;
    }

    if((data1*tmp_data2 > MAX_FREQ)||(data1*tmp_data2 < MIN_FREQ))
        return -1;
        
}

INT32 freq_handle(INT8 **keylist,INT32 len,double data1,INT32 data2,
                      INT8 * data3,
                      INT32 operation,
                      INT8 valstr,
                      INT8 *resp_buf)
{
    INT ret;
    
    switch (operation) 
    {
        case OPT_GET_VALUE:
            ret = get_value(keylist,len,resp_buf);
            if(ret <0)
            {
                printf("freq_handle get value failed!\n");
                strcat(resp_buf, "ERROR,freq_handle get value failed!\n");
                return -1;
            }
            return 0;
        case OPT_SET_VALUE:
            INT32 tmp_data2;
            VAL_LIST * list_ptr;
            
            ret = freq_data_check(keylist, len,data1, data3);

            if(ret < 0)
            {
                printf("ERROR,freq_handle, parameter validation faild ");
                strcat(resp_buf, "ERROR,freq_handle, parameter validation faild ");
                return -1;
            }

            if(data2 == STR_VALUE)
            {
                tmp_data2 = freq_str_val_map(list_ptr->value_list[1]);
            }

            ret = add_target_kv_node(keylist,len,data1,tmp_data2,data3,valstr);
            if(ret < 0)
            {
                printf("ERROR,freq_handle, add CONFIG faild ");
                strcat(resp_buf, "ERROR,freq_handle, add CONFIG faild ");
                return -1;
            }      

            ret = freq_set_logic_reg(keylist, len);
            if(ret < 0)
            {
                printf("ERROR,freq_handle, reg config faild ");
                strcat(resp_buf, "ERROR,freq_handle, reg config faild ");
                return -1;
            }                         
            return 0;
        case OPT_CHECK_KEY_LIST:
        default:
            printf("freq_handle Operation no support! %d \n", operation);
            strcat(resp_buf, "ERROR,freq_handle, Operation no support!");
            return ERROR;
    }
}


