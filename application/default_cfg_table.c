/* default_cfg_table.c */

#include "types.h"

const INT8 * default_cfg_table[] = 
{
    (INT8*)":FREQ:CW <1> <GHz>",
    (INT8*)":FREQ:RF:STAR <100> <MHz>",
    (INT8*)":FREQ:RF:STOP <100> <MHz>",
    (INT8*)":FREQ:RF:SCAL LOG",
    (INT8*)":AMPL:CW <-127.0> <dBm>",
    (INT8*)":AMPL:STAR <-127.0> <dBm>",
    (INT8*)":AMPL:STOP <-127.0> <dBm>",
    
    (INT8*)":SWEep:RF:STAT 0",
    (INT8*)":SWEep:AMPL:STAT 0",
    (INT8*)":SWEep:STEP:POIN <10>",
    (INT8*)":SWEep:STEP:DWEL <10> <ms>",
    (INT8*)":SWEep:REP CONT",
    (INT8*)":SWEep:STRG IMM",
    (INT8*)":SWEep:STRG:SLOP EXTP",
    (INT8*)":SWEep:DIR UP",
    (INT8*)":PULM:STAT 0",
    (INT8*)":PULM:SOUR INT",

    (INT8*)":PULM:PER <200> <us>",
    (INT8*)":PULM:WIDT <100> <us>",
    /*terminator*/
    NULL,
};

