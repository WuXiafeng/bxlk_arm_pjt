/* app_ampl.c */

#include "app_ampl.h"
#include "msg_handle.h"
#include "str_prase.h"
#include "config_file_manage.h"

INT32 ampl_str_val_map(INT8 * str)
{
    if(!strcmp((char *)str, "dBm"))
    {
        return UNIT_DBM;
    }
    else if(!strcmp((char *)str, "dBmV"))
    {
        return UNIT_DBMV;
    }
    else if(!strcmp((char *)str, "dBV"))
    {
        return UNIT_DBV;
    }
    else if(!strcmp((char *)str, "mV"))
    {
        return UINT_MW;
    }
    else if(!strcmp((char *)str, "uV"))
    {
        return UNIT_UV;
    }

    return -1;
}

INT32 ampl_data_check(INT8 **keylist, INT32 len, double data1,INT8 * data3)
{
    VAL_LIST * list_ptr;
    list_ptr = (VAL_LIST *)data3;
    INT32 tmp_data2;

    if((len != 2)||(strcmp("AMPL",(char *)keylist[0])))
        return -1;

    if((strcmp("CW",(char *)keylist[1])) && \
        (strcmp("STAR",(char *)keylist[1])) && \
        (strcmp("STOP",(char *)keylist[1])))
        return -1;

    tmp_data2 = ampl_str_val_map((INT8 *)list_ptr->value_list[1]);

    if(tmp_data2 < 0)
    {
        return -1;
    }

    if(tmp_data2 == UNIT_DBM)
    {
        if((data1 > MAX_DBM)||(data1 < MIN_DBM))
            return -1;
    }
    else if(tmp_data2 == UNIT_DBMV)
    {
        if((data1 > MAX_DBMV)||(data1 < MIN_DBMV))
            return -1;
    }
    else if(tmp_data2 == UNIT_DBV)
    {
        if((data1 > MAX_DBV)||(data1 < MIN_DBV))
            return -1;
    }
    else if((tmp_data2 == UINT_MW) || (tmp_data2 == UNIT_UV))
    {
        if((data1*tmp_data2 > MAX_DBV)||(data1*tmp_data2 < MIN_DBV))
            return -1;
    }

    return OK;
}

INT32 ampl_set_logic_reg(INT8 **keylist,INT32 len)
{
    (void)keylist;
    (void)len;
    //TODO: 
    return OK;
}

INT32 ampl_handle(INT8 **keylist,INT32 len,double data1,INT32 data2,
                      INT8* data3,
                      INT32 operation,
                      INT8* valstr,
                      INT8 *resp_buf)                 
{
    INT32 ret;
    INT32 tmp_data2;
    VAL_LIST * list_ptr = (VAL_LIST *)data3;;    
    
    switch (operation) 
    {
        case OPT_GET_VALUE:
            ret = get_value(keylist,len,resp_buf);
            if(ret <0)
            {
                printf("ampl_handle get value failed!\n");
                strcat((char *)resp_buf, "ERROR,ampl_handle get value failed!\n");
                return -1;
            }
            return 0;
        case OPT_SET_VALUE:
            ret = ampl_data_check(keylist, len,data1, data3);

            if(ret < 0)
            {
                printf("ERROR,ampl_handle, parameter validation faild ");
                strcat((char *)resp_buf, "ERROR,ampl_handle, parameter validation faild ");
                return -1;
            }

            if(data2 == STR_VALUE)
            {
                tmp_data2 = ampl_str_val_map((INT8 *)list_ptr->value_list[1]);
            }

            ret = add_target_kv_node(keylist,len,data1,tmp_data2,data3,valstr);
            if(ret < 0)
            {
                printf("ERROR,ampl_handle, add CONFIG faild ");
                strcat((char *)resp_buf, "ERROR,ampl_handle, add CONFIG faild ");
                return -1;
            }      

            ret = ampl_set_logic_reg(keylist, len);
            if(ret < 0)
            {
                printf("ERROR,ampl_handle, reg config faild ");
                strcat((char *)resp_buf, "ERROR,ampl_handle, reg config faild ");
                return -1;
            }                         
            return 0;
        case OPT_CHECK_KEY_LIST:
        default:
            printf("ampl_handle Operation no support! %d \n", operation);
            strcat((char *)resp_buf, "ERROR,ampl_handle, Operation no support!");
            return ERROR;
    }
}


