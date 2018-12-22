/*app_freq.c*/

#define UNIT_GHZ 0xaaaa0001
#define UNIT_MHZ 0xaaaa0002
#define UNIT_KHZ 0xaaaa0004

INT32 str_val_map(INT8 * str)
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

INT32 freq_handle(INT8 **keylist,INT32 len,INT32 data1,INT32 data2,
                      void* data3,
                      INT32 operation,
                      INT8 valstr,
                      INT8 *resp_buf)
{

}


