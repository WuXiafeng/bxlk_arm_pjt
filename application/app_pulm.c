/* app_pulm.c */

#include "app_pulm.h"
#include "msg_handle.h"
#include "str_prase.h"
#include "config_file_manage.h"

INT32 pulm_str_val_map(INT8 * str)
{
    if(!strcmp((char *)str, "ON"))
    {
        return PULM_ON;
    }
    else if(!strcmp((char *)str, "OFF"))
    {
        return PULM_OFF;
    }
    else if(!strcmp((char *)str, "ms"))
    {
        return PULM_MS;
    }
    else if(!strcmp((char *)str, "s"))
    {
        return PULM_S;
    }
    else if(!strcmp((char *)str, "us"))
    {
        return PULM_US;
    }
    else if(!strcmp((char *)str, "INT"))
    {
        return PULM_INT;
    } 
    else if(!strcmp((char *)str, "EXT"))
    {
        return PULM_EXT;
    }    

    return -1;
}

INT32 pulm_data_check(INT8 **keylist, INT32 len, double data1,INT8 * data3)
{
    VAL_LIST * list_ptr;
    list_ptr = (VAL_LIST *)data3;
    INT32 tmp_data2;

    if((len > 2) || (strcmp("PULM",(char *)keylist[0])))
        return -1;

    if((strcmp("STAT",(char *)keylist[1])) && \
        (strcmp("SOUR",(char *)keylist[1])) && \
        (strcmp("PER",(char *)keylist[1])) && \
        (strcmp("WIDT",(char *)keylist[1])) && \
        1)
        return -1;

    if(!(strcmp("SOUR",(char *)keylist[1])))
    {
        tmp_data2 = pulm_str_val_map((INT8*)list_ptr->value_list[0]);

        if(tmp_data2 < 0)
            return -1;
    }
    else if(!(strcmp("WIDT",(char *)keylist[2])))
    {    
        tmp_data2 = pulm_str_val_map((INT8*)list_ptr->value_list[1]);

        if(tmp_data2 < 0)
        {
            return -1;
        }

        if((data1*tmp_data2 > PULM_MAX_WIDT)||(data1*tmp_data2 < PULM_MIN_WIDT))
            return -1;
    }
    else if(!(strcmp("PER",(char *)keylist[2])))
    {
        tmp_data2 = pulm_str_val_map((INT8*)list_ptr->value_list[1]);

        if(tmp_data2 < 0)
        {
            return -1;
        }

        if((data1*tmp_data2 > PULM_MAX_PER)||(data1*tmp_data2 < PULM_MIN_PER))
            return -1;
    } 
    
    return OK;
}

INT32 pulm_set_logic_reg(INT8 **keylist,INT32 len)
{
    //TODO: 
    (void)keylist;
    (void)len;    
    return OK;
}

INT32 pulm_handle(INT8 **keylist,INT32 len,double data1,INT32 data2,
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
                printf("pulm_handle get value failed!\n");
                strcat((char *)resp_buf, (const char *)"ERROR,pulm_handle get value failed!\n");
                return -1;
            }
            return 0;
        case OPT_SET_VALUE:
            ret = pulm_data_check(keylist, len,data1, data3);

            if(ret < 0)
            {
                printf("ERROR,pulm_handle, parameter validation faild ");
                strcat((char *)resp_buf, (const char *)"ERROR,pulm_handle, parameter validation faild ");
                return -1;
            }
            /*All strs convert to values was saved in data2*/
            if(!(strcmp("SOUR",(char *)keylist[1])))
            {
                tmp_data2 = pulm_str_val_map((INT8*)list_ptr->value_list[0]);
            }
            else if(data2 == STR_VALUE)
            {
                tmp_data2 = pulm_str_val_map((INT8 *)list_ptr->value_list[1]);
            }

            ret = add_target_kv_node(keylist,len,data1,tmp_data2,data3,valstr);
            if(ret < 0)
            {
                printf("ERROR,pulm_handle, add CONFIG faild ");
                strcat((char *)resp_buf, (const char *)"ERROR,pulm_handle, add CONFIG faild ");
                return -1;
            }      

            ret = pulm_set_logic_reg(keylist, len);
            if(ret < 0)
            {
                printf("ERROR,pulm_handle, reg config faild ");
                strcat((char *)resp_buf, (const char *)"ERROR,pulm_handle, reg config faild ");
                return -1;
            }                         
            return 0;
        case OPT_CHECK_KEY_LIST:
        default:
            printf("pulm_handle Operation no support! %d \n", operation);
            strcat((char *)resp_buf, (const char *)"ERROR,pulm_handle, Operation no support!");
            return ERROR;
    }

}


