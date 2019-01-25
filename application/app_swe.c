/* app_swe.c */

#include "app_swe.h"
#include "msg_handle.h"
#include "str_prase.h"
#include "config_file_manage.h"

INT32 swe_str_val_map(INT8 * str)
{
    if(!strcmp((char *)str, "ON"))
    {
        return SWE_ON;
    }
    else if(!strcmp((char *)str, "OFF"))
    {
        return SWE_OFF;
    }
    else if(!strcmp((char *)str, "ms"))
    {
        return SWE_MS;
    }
    else if(!strcmp((char *)str, "s"))
    {
        return SWE_S;
    }
    else if(!strcmp((char *)str, "SING"))
    {
        return SWE_REP_SING;
    }
    else if(!strcmp((char *)str, "CONT"))
    {
        return SWE_REP_CONT;
    } 
    else if(!strcmp((char *)str, "IMM"))
    {
        return SWE_STRG_IMM;
    } 
    else if(!strcmp((char *)str, "EXT"))
    {
        return SWE_STRG_EXT;
    }
    else if(!strcmp((char *)str, "KEY"))
    {
        return SWE_STRG_KEY;
    }
    else if(!strcmp((char *)str, "EXTN"))
    {
        return SWE_STRG_SLOP_EXTN;
    }
    else if(!strcmp((char *)str, "EXTP"))
    {
        return SWE_STRG_SLOP_EXTP;
    }
    else if(!strcmp((char *)str, "UP"))
    {
        return SWE_DIR_UP;
    }
    else if(!strcmp((char *)str, "DOWN"))
    {
        return SWE_DIR_DOWN;
    }     

    return -1;
}

INT32 swe_data_check(INT8 **keylist, INT32 len, double data1,INT8 * data3)
{
    VAL_LIST * list_ptr;
    list_ptr = (VAL_LIST *)data3;
    INT32 tmp_data2;

    if((len > 3) || (strcmp("SWE",(char *)keylist[0])))
        return -1;

    if((strcmp("AMPL",(char *)keylist[1])) && \
        (strcmp("RF",(char *)keylist[1])) && \
        (strcmp("STEP",(char *)keylist[1])) && \
        (strcmp("REP",(char *)keylist[1])) && \
        (strcmp("STRG",(char *)keylist[1])) && \
        (strcmp("DIR",(char *)keylist[1])) && \
        1)
        return -1;

    if((len == 3) && \
        (strcmp("STAT",(char *)keylist[2]))&& \
        (strcmp("POIN",(char *)keylist[2]))&& \
        (strcmp("DWEL",(char *)keylist[2]))&& \
        (strcmp("SLOP",(char *)keylist[2]))&& \
        1)
        return -1;

    if(!(strcmp("REP",(char *)keylist[1]))|| \
        !(strcmp("STRG",(char *)keylist[1]))|| \
        !(strcmp("DIR",(char *)keylist[1]))|| \
        0)
    {
        tmp_data2 = swe_str_val_map((INT8*)list_ptr->value_list[0]);

        if(tmp_data2 < 0)
            return -1;
    }
    else if(!(strcmp("DWEL",(char *)keylist[2])))
    {    
        tmp_data2 = swe_str_val_map((INT8*)list_ptr->value_list[1]);

        if(tmp_data2 < 0)
        {
            return -1;
        }

        if((data1*tmp_data2 > SWE_MAX_DWEL)||(data1*tmp_data2 < SWE_MIN_DWEL))
            return -1;
    }
    else if(!(strcmp("STAT",(char *)keylist[2])))
    {
        if((data1 != SWE_ON) && (data1 != SWE_OFF))
            return -1;
    }
    else if(!(strcmp("POIN",(char *)keylist[2])))
    {
        if((data1 > POINT_MAX) || (data1 < POINT_MIN))
            return -1;
    }    
    
    return OK;
}

INT32 swe_set_logic_reg(INT8 **keylist,INT32 len)
{
    //TODO: 
        (void)keylist;
    (void)len;
    return OK;
}

INT32 swe_handle(INT8 **keylist,INT32 len,double data1,INT32 data2,
                      INT8* data3,
                      INT32 operation,
                      INT8* valstr,
                      INT8 *resp_buf)
{
    INT32 ret;
    INT32 tmp_data2;
    VAL_LIST * list_ptr = (VAL_LIST *)data3;
    
    switch (operation) 
    {
        case OPT_GET_VALUE:
            ret = get_value(keylist,len,resp_buf);
            if(ret <0)
            {
                printf("swe_handle get value failed!\n");
                strcat((char *)resp_buf, (const char *)"ERROR,swe_handle get value failed!\n");
                return -1;
            }
            return 0;
        case OPT_SET_VALUE:
            ret = swe_data_check(keylist, len,data1, data3);

            if(ret < 0)
            {
                printf("ERROR,swe_handle, parameter validation faild ");
                strcat((char *)resp_buf, (const char *)"ERROR,swe_handle, parameter validation faild ");
                return -1;
            }
            /*All strs convert to values was saved in data2*/
            if(!(strcmp("REP",(char *)keylist[1]))|| \
                !(strcmp("STRG",(char *)keylist[1]))|| \
                !(strcmp("DIR",(char *)keylist[1]))|| \
                0)
            {
                tmp_data2 = swe_str_val_map((INT8*)list_ptr->value_list[0]);
            }
            else if(data2 == STR_VALUE)
            {
                tmp_data2 = swe_str_val_map((INT8 *)list_ptr->value_list[1]);
            }

            ret = add_target_kv_node(keylist,len,data1,tmp_data2,data3,valstr);
            if(ret < 0)
            {
                printf("ERROR,swe_handle, add CONFIG faild ");
                strcat((char *)resp_buf, (const char *)"ERROR,swe_handle, add CONFIG faild ");
                return -1;
            }      

            ret = swe_set_logic_reg(keylist, len);
            if(ret < 0)
            {
                printf("ERROR,swe_handle, reg config faild ");
                strcat((char *)resp_buf, (const char *)"ERROR,swe_handle, reg config faild ");
                return -1;
            }                         
            return 0;
        case OPT_CHECK_KEY_LIST:
        default:
            printf("swe_handle Operation no support! %d \n", operation);
            strcat((char *)resp_buf, (const char *)"ERROR,swe_handle, Operation no support!");
            return ERROR;

    }
}

