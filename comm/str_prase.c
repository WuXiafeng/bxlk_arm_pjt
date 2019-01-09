/*str_prase.c*/
#include "str_prase.h"
#include "pf.h"

INT8* strtrim(INT8 *s, STRFUNCPTR pfunc)
{
    INT8 *in  = s;
    INT8 *out = s;
    while (*in)
    {
        if (pfunc(*in))
        {
            *out++ = *in;
        }
        in++;
    }
     
    *(out) = 0;
    return s;
}

INT8 * get_upper(INT8 * str)
{
    return strtrim(str, isupper);
}

/* Convert only digital string */
INT32 convet_str_2_value(INT8 * str, double * db)
{
    if(!str)
        return -1;

    if((isdigit(*str)) || (*str == '-'))
    {
        if(db)
            *db = atof((char *)str);
        return DIGI_VALUE;
    }
    /* If it's string, just indicate that  */
    *db = 0;
    return STR_VALUE;
}

INT32 value_str_prase(INT8 *valstr,double *data1,
                           INT32 *data2, INT8 *data3, INT32 *operation)
{
    INT8 * tmp_save;
    INT8 * ret_str;
    INT32 i = 0;
    VAL_LIST * list_ptr;
    INT8 * tmptr;
    list_ptr = (VAL_LIST *)data3;

    memset((char *)list_ptr, 0, sizeof(VAL_LIST) + 1 + strlen((char *)valstr));
    tmptr = (INT8 *)list_ptr + sizeof(VAL_LIST) + 1;

    strncpy((char *)tmptr, (char *)valstr, strlen((char *)valstr));

    ret_str = (INT8*)strtok_r((char *)tmptr, " <>", (char **)&tmp_save);
    if(ret_str)
    {
        list_ptr->value_list[i] = (char *)strtrim(ret_str,isalnum);
    }
    else{
        printf("value_str_prase no valid value, str is %s\n",valstr);
        PT_OPT.mem_free(list_ptr);
        return -1;
    }
    
    i++;

    while(ret_str)
    {
        ret_str = (INT8*)strtok_r(NULL," <>",(char **)&tmp_save);
        if(ret_str)
        {
            /* Only get the UPER, LOWER, DIGITAL chars */
            list_ptr->value_list[i] = (char *)strtrim(ret_str,isalnum);
            i++;
        }
        
        if(i>=VAL_LIST_LEN)
            break;
    }

    *operation = OPT_SET_VALUE;
    convet_str_2_value((INT8 *)list_ptr->value_list[0], data1);
    *data2 = convet_str_2_value((INT8 *)list_ptr->value_list[1], NULL);

    return OK;
}

/* NOTICE: data3 will point to a new alloc heap memory  */
/* Optimize this later */
INT32 commd_str_prase(INT8 *buf,
                              INT8 **keylist,
                              INT32 *len,
                              double *data1,
                              INT32 *data2,
                              INT8* data3,
                              INT32 *operation,
                              INT8 *valstr)
{
    if((!buf)||(!(*keylist))||(!len)||(!data1)||(!data2)||(!data3)||(!operation)||(!valstr))
    {
        printf("commd_str_prase invalid parameter!\n");
        return -1;
    }

    if((*buf != ':') || (strlen((char *)buf) < 4))
    {
        printf("syntax error, your CMD str is:\n \t%s \n",buf);
    }

    char * tmp_save;
    char * ret_str;
    INT32 i;

    ret_str = strtok_r((char *)buf,":",&tmp_save);
    strcpy((char *)keylist[*len],(char *)ret_str);
    (*len)++;

    while(ret_str)
    {
        ret_str = strtok_r(NULL,":",&tmp_save);
        if(ret_str)
        {
            strcpy((char *)keylist[*len],(char *)ret_str);
            (*len)++;
        }
    }

    char *tmptr;

    tmptr = strrchr((char *)keylist[*len - 1], '?');

    if(tmptr)
    {
        *operation = OPT_GET_VALUE;
        *tmptr='\0';
        return 0;
    }

    tmptr = strrchr((char *)keylist[*len - 1], ' ');

    if(!tmptr)
    {
        tmptr = strrchr((char *)keylist[*len - 1], '<');

        if(!tmptr)
        {
            *operation = OPT_CHECK_KEY_LIST;
            return 0;
        }
    }

    strcpy((char *)valstr,(char *)tmptr);
    /* Cut the string  keylist[*len - 1] here*/
    *tmptr='\0';

    /* get upper  */
    for(i=0;i<*len;i++){
        strtrim(keylist[i], isupper);
    }

    return value_str_prase(valstr,data1,data2,data3,operation);
}

void combine_keylist(INT8 *dst, INT8 ** keylist, INT32 len)
{
    INT32 i;
    for(i = 0; i< len ; i++)
    {
        strcat((char *)dst, ":");
        strcat((char *)dst, (char *)keylist[i]);
    }
}

