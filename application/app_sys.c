/* app_sys.c */

#include "app_sys.h"
#include "msg_handle.h"
#include "str_prase.h"
#include "config_file_manage.h"

INT32 sys_str_val_map(INT8 * str)
{
    if(!strcmp((char *)str, "INT10MHZ"))
    {
        return SYS_INT10MHZ;
    }
    else if(!strcmp((char *)str, "EXT10MHZ"))
    {
        return SYS_EXT10MHZ;
    }
    
    return -1;
}

INT32 sys_data_check(INT8 **keylist, INT32 len, double data1,INT8 * data3)
{
    VAL_LIST * list_ptr;
    list_ptr = (VAL_LIST *)data3;
    INT32 tmp_data2;
    INT32 tmp;

    if((len > 3) || (strcmp("SYST",(char *)keylist[0])))
        return -1;

    if((strcmp("ERR",(char *)keylist[1])) && \
        (strcmp("DATE",(char *)keylist[1])) && \
        (strcmp("TIME",(char *)keylist[1])) && \
        (strcmp("REF",(char *)keylist[1])) && \
        1)
        return -1;

    if(!(strcmp("FREQ",(char *)keylist[2])) && \
        !(strcmp("REF",(char *)keylist[1]))\
       )
    {
        tmp_data2 = sys_str_val_map((INT8*)list_ptr->value_list[0]);

        if(tmp_data2 < 0)
            return -1;
    }
    else if(!(strcmp("TIME",(char *)keylist[2])))
    {    
        /* hour */
        if((data1 > SYS_HOUR_MAX)||(data1 < SYS_HOUR_MIN))
            return -1;

        /*minute*/
        tmp = strtoul(list_ptr->value_list[1],NULL,0);

        if((tmp > SYS_MINT_MAX)||(tmp < SYS_MINT_MIN))
            return -1;

        /*second*/
        tmp = strtoul(list_ptr->value_list[2],NULL,0);

        if((tmp > SYS_SECD_MAX)||(tmp < SYS_SECD_MIN))
            return -1;
    }
    else if(!(strcmp("DATE",(char *)keylist[2])))
    {
        /* year */
        if((data1 > SYS_YEAR_MAX)||(data1 < SYS_YEAR_MIN))
            return -1;

        /*month*/
        tmp = strtoul(list_ptr->value_list[1],NULL,0);

        if((tmp > SYS_MONTH_MAX)||(tmp < SYS_MONTH_MIN))
            return -1;

        /*day*/
        tmp = strtoul(list_ptr->value_list[2],NULL,0);

        if((tmp > SYS_DAY_MAX)||(tmp < SYS_DAY_MIN))
            return -1;
    } 
    
    return OK;
}

INT32 sys_set_logic_reg(INT8 **keylist,INT32 len)
{
    //TODO: 
    (void)keylist;
    (void)len;    
    return OK;
}

INT32 sys_handle(INT8 **keylist,INT32 len,double data1,INT32 data2,
                      INT8* data3,
                      INT32 operation,
                      INT8* valstr,
                      INT8 *resp_buf)
{
    INT32 ret;
    (VOID)data2;
    INT32 tmp_data2;
    VAL_LIST * list_ptr = (VAL_LIST *)data3;
    
    switch (operation) 
    {
        case OPT_GET_VALUE:
            ret = get_value(keylist,len,resp_buf);
            if(ret <0)
            {
                printf("sys_handle get value failed!\n");
                strcat((char *)resp_buf, (const char *)"ERROR,sys_handle get value failed!\n");
                return -1;
            }
            return 0;
        case OPT_SET_VALUE:
            ret = sys_data_check(keylist, len,data1, data3);

            if(ret < 0)
            {
                printf("ERROR,sys_handle, parameter validation faild ");
                strcat((char *)resp_buf, (const char *)"ERROR,sys_handle, parameter validation faild ");
                return -1;
            }
            /*All strs convert to values was saved in data2*/
            if(!(strcmp("FREQ",(char *)keylist[2])) && \
                !(strcmp("REF",(char *)keylist[1])) \
              )
            {
                tmp_data2 = sys_str_val_map((INT8*)list_ptr->value_list[0]);
            }

            ret = add_target_kv_node(keylist,len,data1,tmp_data2,data3,valstr);
            if(ret < 0)
            {
                printf("ERROR,sys_handle, add CONFIG faild ");
                strcat((char *)resp_buf, (const char *)"ERROR,sys_handle, add CONFIG faild ");
                return -1;
            }      

            ret = sys_set_logic_reg(keylist, len);
            
            if(ret < 0)
            {
                printf("ERROR,sys_handle, reg config faild ");
                strcat((char *)resp_buf, (const char *)"ERROR,sys_handle, reg config faild ");
                return -1;
            }                         
            return 0;
        case OPT_CHECK_KEY_LIST:
        default:
            printf("sys_handle Operation no support! %d \n", operation);
            strcat((char *)resp_buf, (const char *)"ERROR,sys_handle, Operation no support!");
            return ERROR;
    }

}


