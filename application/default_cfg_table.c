/* default_cfg_table.c */

#include "types.h"

const INT8 * default_cfg_table[] = 
{
    (INT8*)":FREQ:CW <1> <GHz>",
    (INT8*)":FREQ:RF:STAR <100> <MHz>",
    (INT8*)":FREQ:RF:STOP <100> <MHz>",
    (INT8*)":FREQ:RF:SCAL LOG", /* miss default , set LOG tmplately*/
    (INT8*)":AMPL:CW <-127.0> <dBm>",
    (INT8*)":AMPL:STAR <-127.0> <dBm>",
    (INT8*)":AMPL:STOP <-127.0> <dBm>",
    
    (INT8*)":SWEep:RF:STATe 0",
    (INT8*)":SWEep:AMPLitude:STATe 0",
    (INT8*)":SWEep:STEP:POINts <10>",
    (INT8*)":SWEep:STEP:DWELl <10> <ms>",
    (INT8*)":SWEep:REPeat CONTinuous",
    (INT8*)":SWEep:STRG IMMediate",
    (INT8*)":SWEep:STRG:SLOPe EXTP",
    (INT8*)":SWEep:DIRection UP",
    (INT8*)":PULM:STATe 0",
    (INT8*)":PULM:SOURce INT",

    (INT8*)":PULM:PERiod <200> <us>",
    (INT8*)":PULM:WIDTh <100> <us>",   
}

