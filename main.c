/*       main.c                                    */
/* Add by Wuxf 20181202 First version */

#include "types.h"
#include "hw.h"
#include "pf.h"
#include "core.h"
#include "comm.h"
#include "bram.h"
#include "config_file_manage.h"
#include "msg_handle.h"

VOID exit_bxlk(VOID)
{
    bram_release();
}

VOID main(const INT32 argc, const INT8 ** argv)
{
    (VOID)argc;
    (VOID)argv;
    INT32 ret;

    atexit(exit_bxlk);

    ret = pt_hw_init();

    if(ret != OK)
    {
        printf("pt_hw_init failed!\n");
        exit(1);
    }
	
    ret = pltf_init();
    if(ret != OK)
    {
        printf("pltf_init failed!\n");
        exit(1);
    }    	

    ret = comm_init();
    if(ret != OK)
    {
        printf("comm_init failed!\n");
        exit(1);
    }    
    
    ret = core_init();
    if(ret != OK)
    {
        printf("core_init failed!\n");
        exit(1);
    } 

    ret = cfg_init_load_file();
    if(ret != OK)
    {
        printf("cfg_init_load_file failed!\n");
        exit(1);
    }

    check_and_set_the_default_value();

    ret = core_state_manager_init();
    if(ret != OK)
    {
        printf("core_state_manager_init failed!\n");
        exit(1);
    } 

    ret = app_init();
    if(ret != OK)
    {
        printf("app_init failed!\n");
        exit(1);
    } 

	set_system_on();
	
    start_socket_session_control();

}


